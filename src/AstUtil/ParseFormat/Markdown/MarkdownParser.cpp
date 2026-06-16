///
/// @file      MarkdownParser.cpp
/// @brief     Markdown SAX 解析器 — 流式状态机实现
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownParser.hpp"
#include "MarkdownSax.hpp"
#include "AstUtil/StringView.hpp"
#include <cctype>
#include <cstring>

AST_NAMESPACE_BEGIN

// ============================================================================
// MarkdownInlineStateMachine — 单字符核心 + StringView 薄封装
// ============================================================================

// ---- 字符分类辅助函数 ----

bool MarkdownInlineStateMachine::isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool MarkdownInlineStateMachine::isWordChar(char c)
{
    // Unicode 单词字符：字母、数字、下划线
    if (c >= 'a' && c <= 'z') return true;
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= '0' && c <= '9') return true;
    if (c == '_') return true;
    // 高位字节（UTF-8 多字节序列的一部分）视为单词字符
    if (static_cast<unsigned char>(c) > 127) return true;
    return false;
}

bool MarkdownInlineStateMachine::isPunctuation(char c)
{
    if (c >= 'a' && c <= 'z') return false;
    if (c >= 'A' && c <= 'Z') return false;
    if (c >= '0' && c <= '9') return false;
    if (c == '_') return false;
    if (static_cast<unsigned char>(c) > 127) return false;
    if (isWhitespace(c)) return false;
    return true;
}

// ---- 格式栈操作（封装深度计数器与 state_ 同步） ----

void MarkdownInlineStateMachine::openEmph(char delim)
{
    sax_.startEmphasis();
    emphDepth_++;
    state_ |= EStateFlags::eEmphasis;
    emphDelim_ = delim;
}

void MarkdownInlineStateMachine::closeEmph()
{
    sax_.endEmphasis();
    emphDepth_--;
    if (emphDepth_ == 0) {
        state_ &= ~EStateFlags::eEmphasis;
        emphDelim_ = 0;
    }
}

void MarkdownInlineStateMachine::openStrong()
{
    sax_.startStrong();
    strongDepth_++;
    state_ |= EStateFlags::eStrong;
}

void MarkdownInlineStateMachine::closeStrong()
{
    sax_.endStrong();
    strongDepth_--;
    if (strongDepth_ == 0)
        state_ &= ~EStateFlags::eStrong;
}

void MarkdownInlineStateMachine::triggerPendingDelete(std::string& result)
{
    if (!!(pendingState_ & EStateFlags::eDelete)) {
        flushPending(result);
        toggleState(EStateFlags::eDelete);
        pendingState_ &= ~EStateFlags::eDelete;
    }
}

void MarkdownInlineStateMachine::flushText(std::string& result)
{
    if (!result.empty()) {
        textEmitted_ = true;
        sax_.text(result);
        result.clear();
    }
}

std::string MarkdownInlineStateMachine::stripCodeSpan(const std::string& raw)
{
    if (raw.empty()) return raw;
    bool leading  = (raw[0] == ' ');
    bool trailing = (raw.back() == ' ');
    bool allSpace = (raw.find_first_not_of(' ') == std::string::npos);
    if (allSpace) return raw;
    if (leading && trailing) return raw.substr(1, raw.size() - 2);
    if (leading)            return raw.substr(1);
    if (trailing)           return raw.substr(0, raw.size() - 1);
    return raw;
}

// ---- 定界符游程解析 ----

void MarkdownInlineStateMachine::resolveDelimRun(char next, std::string& result)
{
    if (delimRunLen_ == 0) return;

    char   dc     = delimRunChar_;
    int    length = delimRunLen_;
    char   prev   = runPrevChar_;

    // ---- 1. 计算侧翼条件 ----
    // 右翼：输入首端视为可侧翼（无可抑制的前驱字符）
    // 左翼：输入末端不自动视为可侧翼（无可跟随的字符时不能打开新格式）
    bool leftFlanking  = (next != 0) && (!isWhitespace(next) || prev == 0);
    bool rightFlanking = (prev == 0) || !isWhitespace(prev) || next == 0;

    // 下划线词内规则：当两侧均为单词字符时，不作为定界符
    bool underscoreDisabled = false;
    if (dc == '_' && prev != 0 && next != 0
        && isWordChar(prev) && isWordChar(next))
    {
        underscoreDisabled = true;
    }

    bool canOpenEmphasis = false;
    bool canCloseEmphasis = false;
    bool canOpenStrong = false;
    bool canCloseStrong = false;

    if (dc == '*')
    {
        // 星号无词内限制 —— 仅需侧翼条件
        canOpenEmphasis  = leftFlanking && length >= 1;
        canCloseEmphasis = rightFlanking && length >= 1;
        canOpenStrong    = leftFlanking && length >= 2;
        canCloseStrong   = rightFlanking && length >= 2;
    }
    else // dc == '_'
    {
        // 下划线：需同时满足侧翼条件且不在词内
        canOpenEmphasis  = leftFlanking  && !underscoreDisabled && length >= 1;
        canCloseEmphasis = rightFlanking && !underscoreDisabled && length >= 1;
        canOpenStrong    = leftFlanking  && length >= 2;
        canCloseStrong   = rightFlanking && length >= 2;
    }

    // ---- 2. 确定可关闭的对象 ----
    bool canCloseE = canCloseEmphasis && (emphDepth_ > 0);
    bool canCloseS = canCloseStrong   && (strongDepth_ > 0);

    int remainingLen = length;

    if (canCloseS && canCloseE && remainingLen >= 3)
    {
        if (emphasisOpenedFirst_) {
            closeStrong(); closeEmph();
        } else {
            closeEmph(); closeStrong();
        }
        remainingLen -= 3;
    }
    else if (canCloseS && remainingLen >= 2)
    {
        closeStrong(); remainingLen -= 2;
    }
    else if (canCloseE && remainingLen >= 1)
    {
        closeEmph(); remainingLen -= 1;
    }

    // ---- 3. 用剩余长度打开新格式（外 → 内，允许嵌套） ----
    bool openedEmphThisRun = false;
    bool openedStrThisRun  = false;

    while (remainingLen > 0)
    {
        if (!openedEmphThisRun && !openedStrThisRun
            && canOpenEmphasis && canOpenStrong && remainingLen >= 3)
        {
            openStrong(); openEmph(dc);   // strong 外层，emph 内层
            remainingLen -= 3;
            emphasisOpenedFirst_ = false; // emph 后开
            openedEmphThisRun = true;
            openedStrThisRun  = true;
            continue;
        }

        if (!openedStrThisRun && canOpenStrong && remainingLen >= 2)
        {
            openStrong();
            remainingLen -= 2;
            emphasisOpenedFirst_ = (emphDepth_ > 0); // emph 已打开则 emph 先于 strong
            openedStrThisRun = true;
            continue;
        }

        if (!openedEmphThisRun && canOpenEmphasis && remainingLen >= 1)
        {
            if (emphDepth_ > 0 && emphDelim_ != dc)
                break; // 异种定界符不可打开嵌套斜体
            openEmph(dc);
            remainingLen -= 1;
            emphasisOpenedFirst_ = (strongDepth_ == 0);
            openedEmphThisRun = true;
            continue;
        }

        break;
    }

    // ---- 4. 剩余字符处理 ----
    // 输入末尾且已有文本输出时，不匹配的尾部定界符静默消耗
    if (next == 0 && textEmitted_)
    {
        remainingLen = 0;
    }
    // 若成功打开了格式，剩余字符静默消耗（不输出为字面量）；
    // 若未打开任何格式，剩余字符作为普通文本输出。
    if (!openedEmphThisRun && !openedStrThisRun)
    {
        for (int i = 0; i < remainingLen; ++i)
            result += dc;
    }

    // ---- 5. 清理游程状态 ----
    delimRunChar_ = 0;
    delimRunLen_  = 0;
}

// ---- 单字符 feed 核心 ----

void MarkdownInlineStateMachine::feed(char c, std::string& result)
{
    // ============================================================
    // 1. 转义预处理（仅在普通模式生效；链接文本/图片 Alt 自行处理转义）
    // ============================================================
    if (mode_ == EParseMode::eNormal && !!(pendingState_ & EStateFlags::eEscape))
    {
        pendingState_ &= ~EStateFlags::eEscape;
        result += c;
        return;
    }

    // ============================================================
    // 2. ! 待确认 —— 单字符缓冲，消除 p+1 前瞻
    // ============================================================
    if (pendingBang_)
    {
        pendingBang_ = false;
        if (c == '[')
        {
            // 确认为图片语法 ![
            flushPending(result);
            resolveDelimRun(0, result);
            mode_ = EParseMode::eImageAlt;
            linkText_.clear();
            linkUrl_.clear();
            return;
        }
        // 不是图片：! 作为普通字符输出，继续处理当前 c
        result += '!';
        // fall through
    }

    // ============================================================
    // 3. 模式内字符处理（需完整消费一个字符的模式）
    // ============================================================
    switch (mode_)
    {
    case EParseMode::eCodeSpan:
        // ---- 行内代码段（支持多反引号定界） ----
        if (c == '`')
        {
            codeSpanTickRun_++;
            return;
        }
        // 非反引号字符：先检查累积的反引号是否为关界定界符
        if (codeSpanTickRun_ > 0)
        {
            if (codeSpanTickRun_ == codeSpanBackticks_)
            {
                // 匹配关界定界符，剥离首尾空格后输出代码段
                sax_.codeSpan(stripCodeSpan(codeBuf_));
                codeBuf_.clear();
                codeSpanBackticks_ = 0;
                codeSpanTickRun_ = 0;
                mode_ = EParseMode::eNormal;
                // 继续处理当前字符（触发关界检测的字符）
                feed(c, result);
                return;
            }
            // 未匹配 —— 将累积的反引号写入内容
            for (int i = 0; i < codeSpanTickRun_; ++i)
                codeBuf_ += '`';
            codeSpanTickRun_ = 0;
        }
        codeBuf_ += c;
        return;

    case EParseMode::eLinkURL:
        if (c == ')')
        {
            sax_.startLink(linkUrl_);
            replayBufferedText(linkText_, result);
            sax_.endLink();
            linkText_.clear();
            linkUrl_.clear();
            mode_ = EParseMode::eNormal;
        }
        else { linkUrl_ += c; }
        return;

    case EParseMode::eImageURL:
        if (c == ')')
        {
            sax_.image(linkText_, linkUrl_);
            linkText_.clear();
            linkUrl_.clear();
            mode_ = EParseMode::eNormal;
        }
        else { linkUrl_ += c; }
        return;

    case EParseMode::eLinkTextEnd:
        if (c == '(')
        {
            mode_ = EParseMode::eLinkURL;
            return;
        }
        // 非法的链接语法：回退 [text] 为普通文本
        result += '[';
        result.append(linkText_);
        result += ']';
        linkText_.clear();
        mode_ = EParseMode::eNormal;
        break;  // fall through — 用普通模式继续处理 c

    case EParseMode::eImageAltEnd:
        if (c == '(')
        {
            mode_ = EParseMode::eImageURL;
            return;
        }
        // 非法图片语法：回退 ![alt] 为普通文本
        result += '!';
        result += '[';
        result.append(linkText_);
        result += ']';
        linkText_.clear();
        mode_ = EParseMode::eNormal;
        break;  // fall through

    default: break;
    }

    // ============================================================
    // 4. 模式入口检查（仅在普通模式下触发）
    // ============================================================
    if (mode_ == EParseMode::eNormal)
    {
        if (c == '`')
        {
            // 触发待定删除线
            triggerPendingDelete(result);
            // 解析定界符游程，提交文本
            resolveDelimRun(c, result);
            flushPending(result);
            codeSpanBackticks_ = 1;
            mode_ = EParseMode::eCodeSpan;
            codeBuf_.clear();
            codeSpanTickRun_ = 0;
            return;
        }

        if (c == '!')
        {
            pendingBang_ = true;
            return;
        }

        if (c == '[')
        {
            // 触发待定删除线以包裹后续链接
            triggerPendingDelete(result);
            // 进入链接文本模式前，先提交累积文本，再解析定界符游程
            flushPending(result);
            resolveDelimRun(c, result);
            mode_ = EParseMode::eLinkText;
            linkText_.clear();
            linkUrl_.clear();
            return;
        }
    }

    // ============================================================
    // 5. 文本累积（链接文本 / 图片 Alt 模式下仅缓冲）
    // ============================================================
    if (mode_ == EParseMode::eLinkText)
    {
        // 链接文本内的转义：\\ 为字面量 \，\] 为字面量 ]
        if (!!(pendingState_ & EStateFlags::eEscape))
        {
            pendingState_ &= ~EStateFlags::eEscape;
            // 转义序列：仅 \ 和 ] 被转义，其他字符保留反斜杠
            if (c == '\\' || c == ']')
                linkText_ += c;
            else
            {
                linkText_ += '\\';
                linkText_ += c;
            }
            return;
        }
        if (c == '\\')
        {
            pendingState_ |= EStateFlags::eEscape;
            return;
        }
        if (c == ']') { mode_ = EParseMode::eLinkTextEnd; return; }
        linkText_ += c;
        return;
    }

    if (mode_ == EParseMode::eImageAlt)
    {
        // 图片 Alt 内转义（与链接文本相同逻辑）
        if (!!(pendingState_ & EStateFlags::eEscape))
        {
            pendingState_ &= ~EStateFlags::eEscape;
            if (c == '\\' || c == ']')
                linkText_ += c;
            else
                { linkText_ += '\\'; linkText_ += c; }
            return;
        }
        if (c == '\\')
            { pendingState_ |= EStateFlags::eEscape; return; }
        if (c == ']') { mode_ = EParseMode::eImageAltEnd; return; }
        linkText_ += c;
        return;
    }

    // ============================================================
    // 6. 普通模式 — 内联格式解析
    // ============================================================

    // ---- 转义 ----
    if (c == '\\')
    {
        resolveDelimRun(c, result);
        pendingState_ |= EStateFlags::eEscape;
        return;
    }

    // ---- 换行：关闭所有活跃格式（内联样式不可跨行） ----
    if (c == '\n')
    {
        flushPending(result);
        triggerPendingDelete(result);
        resolveDelimRun(0, result);
        toggleState(state());
        pendingState_ = EStateFlags::ePlain;
        result += '\n';
        return;
    }

    // ---- 强调/粗体定界符（* 或 _） ----
    if (c == '*' || c == '_')
    {
        if (delimRunLen_ > 0 && delimRunChar_ != c)
        {
            // 定界符类型变更：先提交累积文本，再解析之前的游程
            flushPending(result);
            resolveDelimRun(c, result);
        }
        if (delimRunLen_ == 0)
        {
            // 新游程开始 —— 记录左侧上下文
            if (!result.empty())
                runPrevChar_ = result.back();
            else
                runPrevChar_ = 0;
        }
        delimRunChar_ = c;
        delimRunLen_++;
        return;
    }

    // ---- 删除线定界符 ~ ----
    if (c == '~')
    {
        if (tildePending_)
        {
            tildePending_ = false;
            flushText(result);
            resolveDelimRun(c, result);
            flushText(result);

            // 若已有待定 eDelete（前一组 ~~ 未触发），先触发
            if (!!(pendingState_ & EStateFlags::eDelete)) {
                toggleState(EStateFlags::eDelete);
                pendingState_ &= ~EStateFlags::eDelete;
            }
            // 若前一 ~~ 刚打开删除线，本组直接关闭（无文本间隔，如 ~~~~）
            if (!!(state_ & EStateFlags::eDelete)) {
                toggleState(EStateFlags::eDelete);
            } else {
                pendingState_ |= EStateFlags::eDelete;
            }
        }
        else
        {
            if (!!(pendingState_ & EStateFlags::eDelete)) {
                toggleState(EStateFlags::eDelete);
                pendingState_ &= ~EStateFlags::eDelete;
            }
            flushText(result);
            resolveDelimRun(c, result);
            flushText(result);
            tildePending_ = true;
        }
        return;
    }

    // ---- 普通字符：提交文本 → 触发待定删除线 → 解析定界符 → 累积字符 ----
    {
        flushPending(result);
        triggerPendingDelete(result);
        resolveDelimRun(c, result);
        result += c;
    }
}

// ============================================================================
// StringView 薄封装
// ============================================================================

void MarkdownInlineStateMachine::feed(StringView chunk)
{
    for (size_t i = 0; i < chunk.size(); ++i)
    {
        char c = chunk[i];

        // ---- 多反引号行内代码段预扫描 ----
        if (mode_ == EParseMode::eNormal && c == '`')
        {
            // 进入代码段前提交文本、触发待定删除线、解析定界符游程
            flushPending(result_);
            triggerPendingDelete(result_);
            resolveDelimRun(c, result_);
            flushPending(result_);
            // 计算连续反引号数量
            int tickCount = 1;
            while (i + tickCount < chunk.size() && chunk[i + tickCount] == '`')
                ++tickCount;
            codeSpanBackticks_ = tickCount;
            codeSpanTickRun_   = 0;
            codeBuf_.clear();
            mode_ = EParseMode::eCodeSpan;
            i += tickCount - 1;  // 跳过所有开界反引号
            continue;
        }

        feed(chunk[i], result_);
    }
    flushPending(result_);
}

// ============================================================================
// finish / 状态辅助
// ============================================================================

void MarkdownInlineStateMachine::finish()
{
    // ---- 处理无文本输出的独立定界符游程（在 resolveDelimRun 之前） ----
    if (delimRunLen_ > 0 && state_ == EStateFlags::ePlain && !textEmitted_)
    {
        char   dc  = delimRunChar_;
        int    len = delimRunLen_;

        if (len == 1)
        {
            result_ += dc;
        }
        else if (len >= 4 && (dc == '*' || dc == '_'))
        {
            // 4+ 字符独立游程 → 分割线（如 ****, ____, ******）
            sax_.horizontalRule();
        }
        else if (dc == '*')
        {
            while (len >= 3) {
                openStrong(); openEmph(dc);   // strong 外层，emph 内层
                len -= 3;
                emphasisOpenedFirst_ = false;
            }
        }
        else // dc == '_'
        {
            while (len >= 3 && (len % 2) == 1) {
                openStrong(); openEmph(dc);
                len -= 3;
                emphasisOpenedFirst_ = false;
            }
            while (len >= 2) {
                openStrong();
                len -= 2;
            }
        }

        delimRunChar_ = 0;
        delimRunLen_  = 0;
        flushPending(result_);
    }

    // ---- 处理末尾未触发的待定删除线切换 ----
    // 仅当尚无文本时（如独立 ~~~~）才执行，有文本时（如尾部 ~~）静默消耗
    if (!!(pendingState_ & EStateFlags::eDelete))
    {
        if (!textEmitted_)
        {
            toggleState(EStateFlags::eDelete);
        }
        pendingState_ &= ~EStateFlags::eDelete;
    }

    // ---- 解析末尾待提交的定界符游程 ----
    resolveDelimRun(0, result_);

    // ---- 清理未结束的模式（将已缓冲内容作为普通文本输出） ----
    switch (mode_)
    {
    case EParseMode::eLinkText:
        result_ += '[';
        result_.append(linkText_);
        break;
    case EParseMode::eLinkTextEnd:
        result_ += '[';
        result_.append(linkText_);
        result_ += ']';
        break;
    case EParseMode::eImageAlt:
        result_ += '!';
        result_ += '[';
        result_.append(linkText_);
        break;
    case EParseMode::eImageAltEnd:
        result_ += '!';
        result_ += '[';
        result_.append(linkText_);
        result_ += ']';
        break;
    case EParseMode::eLinkURL:
    case EParseMode::eImageURL:
        result_ += ']';
        result_ += '(';
        result_.append(linkUrl_);
        linkUrl_.clear();
        break;
    case EParseMode::eCodeSpan:
        // 空代码段（如 ``）—— 开界反引号兼具关界功能
        if (codeBuf_.empty() && codeSpanTickRun_ == 0 && codeSpanBackticks_ > 0)
        {
            sax_.codeSpan("");
            codeBuf_.clear();
            codeSpanBackticks_ = 0;
            codeSpanTickRun_ = 0;
            break;
        }
        // 检查末尾累积的反引号是否匹配关界定界符
        if (codeSpanTickRun_ == codeSpanBackticks_ && codeSpanBackticks_ > 0)
        {
            // 有效关界 —— 剥离首尾空格后输出代码段
            sax_.codeSpan(stripCodeSpan(codeBuf_));
        }
        else
        {
            // 未闭合：回退开界反引号和内容为普通文本
            for (int i = 0; i < codeSpanBackticks_; ++i)
                result_ += '`';
            for (int i = 0; i < codeSpanTickRun_; ++i)
                result_ += '`';
            result_.append(codeBuf_);
        }
        codeBuf_.clear();
        codeSpanBackticks_ = 0;
        codeSpanTickRun_ = 0;
        break;
    default: break;
    }
    linkText_.clear();
    mode_ = EParseMode::eNormal;

    // ---- 待确认的 ! ----
    if (pendingBang_)
    {
        result_ += '!';
        pendingBang_ = false;
    }

    flushPending(result_);
    closeAllFormats(result_);
}

void MarkdownInlineStateMachine::flushPending(std::string& result)
{
    if (tildePending_)
    {
        result += '~';
        tildePending_ = false;
    }
    if (!result.empty())
    {
        textEmitted_ = true;
        sax_.text(result);
        result.clear();
    }
}

void MarkdownInlineStateMachine::closeAllFormats(std::string& result)
{
    // 循环关闭所有活跃格式（支持多层嵌套）
    while (state_ != EStateFlags::ePlain)
        toggleState(state_);
}

void MarkdownInlineStateMachine::toggleState(EStateFlags state)
{
    auto lastState = state_;

    // ============================================================
    // 第一遍：关闭（end）
    //   若 emph 和 strong 同时需要关闭，根据打开顺序决定先后：
    //   - emph 先开（emph 包 strong） → 先关 strong 再关 emph
    //   - strong 先开（strong 包 emph） → 先关 emph 再关 strong
    // ============================================================
    bool closeBoth = !!(state & EStateFlags::eEmphasis) && !!(lastState & EStateFlags::eEmphasis)
                  && !!(state & EStateFlags::eStrong) && !!(lastState & EStateFlags::eStrong);

    // 关闭阶段 —— 使用深度计数器替代位标记
    if (closeBoth && !emphasisOpenedFirst_) {
        closeEmph(); closeStrong();   // strong wraps emph: close inner first
    }
    else if (closeBoth && emphasisOpenedFirst_) {
        closeStrong(); closeEmph();   // emph wraps strong: close inner first
    }
    else
    {
        if (!!(state & EStateFlags::eDelete) && !!(lastState & EStateFlags::eDelete))
            { sax_.endDelete(); state_ &= ~EStateFlags::eDelete; }
        if (!!(state & EStateFlags::eStrong) && !!(lastState & EStateFlags::eStrong))
            { closeStrong(); }
        if (!!(state & EStateFlags::eEmphasis) && !!(lastState & EStateFlags::eEmphasis))
            { closeEmph(); }
    }

    // 打开阶段
    if (!!(state & EStateFlags::eEmphasis) && !(lastState & EStateFlags::eEmphasis))
        { openEmph('*'); emphasisOpenedFirst_ = (strongDepth_ == 0); }
    if (!!(state & EStateFlags::eStrong) && !(lastState & EStateFlags::eStrong))
        { openStrong(); if (emphDepth_ > 0) emphasisOpenedFirst_ = true; }
    if (!!(state & EStateFlags::eDelete) && !(lastState & EStateFlags::eDelete))
        { sax_.startDelete(); state_ |= EStateFlags::eDelete; }
}

void MarkdownInlineStateMachine::replayBufferedText(const std::string& text,
                                                     std::string& result)
{
    if (text.empty()) return;

    // ---- 保存当前状态 ----
    auto savedMode          = mode_;
    auto savedState         = state_;
    auto savedPendingState  = pendingState_;
    auto savedPendingTilde  = tildePending_;
    auto savedTextEmitted   = textEmitted_;
    auto savedEmphFirst     = emphasisOpenedFirst_;
    auto savedPendingBang   = pendingBang_;
    auto savedDelimRunChar  = delimRunChar_;
    auto savedDelimRunLen   = delimRunLen_;
    auto savedRunPrevChar   = runPrevChar_;
    auto savedCSTickCount   = codeSpanBackticks_;
    auto savedCSTickRun     = codeSpanTickRun_;
    auto savedEmphDepth     = emphDepth_;
    auto savedStrongDepth   = strongDepth_;

    // ---- 重置为干净状态 ----
    mode_               = EParseMode::eNormal;
    state_              = EStateFlags::ePlain;
    pendingState_       = EStateFlags::ePlain;
    tildePending_       = false;
    textEmitted_        = false;
    emphasisOpenedFirst_= false;
    pendingBang_        = false;
    delimRunChar_       = 0;
    delimRunLen_        = 0;
    runPrevChar_        = 0;
    codeSpanBackticks_  = 0;
    codeSpanTickRun_    = 0;
    emphDepth_          = 0;
    strongDepth_        = 0;

    // ---- 逐字符重播（使用单字符核心） ----
    for (char ch : text)
        feed(ch, result);
    flushPending(result);
    closeAllFormats(result);

    // ---- 恢复原状态 ----
    mode_               = savedMode;
    state_              = savedState;
    pendingState_       = savedPendingState;
    tildePending_       = savedPendingTilde;
    textEmitted_        = savedTextEmitted;
    emphasisOpenedFirst_= savedEmphFirst;
    pendingBang_        = savedPendingBang;
    delimRunChar_       = savedDelimRunChar;
    delimRunLen_        = savedDelimRunLen;
    runPrevChar_        = savedRunPrevChar;
    codeSpanBackticks_  = savedCSTickCount;
    codeSpanTickRun_    = savedCSTickRun;
    emphDepth_          = savedEmphDepth;
    strongDepth_        = savedStrongDepth;
}

// ============================================================================
// 行分类
// ============================================================================

bool MarkdownParser::isHeadingLine(StringView line, int& level, StringView& content)
{
    if (line.empty() || line[0] != '#') return false;

    level = 0;
    size_t i = 0;
    while (i < line.size() && line[i] == '#' && level < 6) { ++level; ++i; }

    // # 后必须有空格才是标题，否则 (#tag) 当作普通文本
    if (i >= line.size() || line[i] != ' ') return false;
    ++i; // 跳过空格

    content = line.substr(i);
    return true;
}

bool MarkdownParser::isCodeFenceLine(StringView line, char& fenceChar, int& count, StringView& lang)
{
    if (line.size() < 3) return false;

    fenceChar = line[0];
    if (fenceChar != '`' && fenceChar != '~') return false;

    count = 0;
    while (count < (int)line.size() && line[count] == fenceChar) ++count;
    if (count < 3) return false;

    // 围栏标记后的语言标识（跳过可选空格）
    size_t start = count;
    if (start < line.size() && line[start] == ' ') ++start;
    lang = line.substr(start);

    return true;
}

bool MarkdownParser::isHorizRuleLine(StringView line)
{
    if (line.size() < 3) return false;

    char c = line[0];
    if (c != '-' && c != '*' && c != '_') return false;

    size_t cnt = 0;
    for (size_t i = 0; i < line.size(); ++i)
    {
        if (line[i] == c) ++cnt;
        else if (line[i] != ' ' && line[i] != '\t') return false;
    }
    return cnt >= 3;
}

bool MarkdownParser::isBlockquoteLine(StringView line, StringView& content)
{
    if (line.empty() || line[0] != '>') return false;

    size_t start = 1;
    if (start < line.size() && line[start] == ' ') ++start;
    content = line.substr(start);
    return true;
}

bool MarkdownParser::isUnorderedListItem(StringView line, StringView& content)
{
    if (line.size() < 2) return false;

    char c = line[0];
    if (c != '-' && c != '*' && c != '+') return false;
    if (line[1] != ' ') return false;

    content = line.substr(2);
    return true;
}

bool MarkdownParser::isOrderedListItem(StringView line, StringView& content, int& number)
{
    if (line.empty() || !std::isdigit(static_cast<unsigned char>(line[0]))) return false;

    size_t i = 0;
    number = 0;
    while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i])))
    {
        number = number * 10 + (line[i] - '0');
        ++i;
    }

    if (i >= line.size() || line[i] != '.') return false;
    ++i; // 跳过 '.'
    if (i < line.size() && line[i] == ' ') ++i; // 跳过可选空格

    content = line.substr(i);
    return true;
}

bool MarkdownParser::isClosingFence(StringView line)
{
    if ((int)line.size() < codeFenceCnt_) return false;

    char fc = line[0];
    if (fc != codeFenceChar_) return false;

    int cnt = 0;
    while (cnt < (int)line.size() && line[cnt] == fc) ++cnt;
    if (cnt < codeFenceCnt_) return false;

    // 剩余字符必须全是空白
    for (size_t i = cnt; i < line.size(); ++i)
        if (line[i] != ' ' && line[i] != '\t') return false;

    return true;
}

// ============================================================================
// 块栈管理
// ============================================================================

void MarkdownParser::closeTop()
{
    if (blockStack_.empty()) return;

    BlockFrame& top = blockStack_.back();
    switch (top.type)
    {
    case eBlockParagraph: sax_.endParagraph();   break;
    case eBlockHeading:   sax_.endHeading(top.level); break;
    case eBlockQuote:     sax_.endBlockquote();  break;
    case eBlockListItem: sax_.endListItem();    break;
    case eBlockList:
        sax_.endList();
        inList_ = false;
        break;
    default: break;
    }
    blockStack_.pop_back();
}

void MarkdownParser::closeToBlock(EBlockType type)
{
    while (!blockStack_.empty() && blockStack_.back().type != type)
        closeTop();
}

void MarkdownParser::closeAllBlocks()
{
    while (!blockStack_.empty())
        closeTop();
}

bool MarkdownParser::hasBlock(EBlockType type) const
{
    for (size_t i = 0; i < blockStack_.size(); ++i)
        if (blockStack_[i].type == type) return true;
    return false;
}

void MarkdownParser::flushParagraph()
{
    if (hasBlock(eBlockParagraph))
    {
        closeToBlock(eBlockParagraph);
        closeTop(); // 关闭段落本身
    }
    paraBuf_.clear();
}

void MarkdownParser::ensureParagraph()
{
    if (!hasBlock(eBlockParagraph))
    {
        sax_.startParagraph();
        BlockFrame f = { eBlockParagraph, 0, false };
        blockStack_.push_back(f);
    }
}

void MarkdownParser::ensureDocStarted()
{
    if (!docStarted_)
    {
        sax_.startDocument();
        docStarted_ = true;
    }
}

// ============================================================================
// 公开接口
// ============================================================================

void MarkdownParser::feed(StringView data)
{
    for (size_t i = 0; i < data.size(); ++i)
    {
        char c = data[i];
        lineBuf_ += c;

        if (c == '\n')
        {
            // 去掉尾部 \n / \r\n
            StringView line(lineBuf_);
            if (!line.empty() && line.back() == '\n') line.remove_suffix(1);
            if (!line.empty() && line.back() == '\r') line.remove_suffix(1);

            processLine(line);
            lineBuf_.clear();
        }
    }
}

void MarkdownParser::finish()
{
    // 处理行缓冲中剩余的内容（末行无换行符的情况）
    if (!lineBuf_.empty())
    {
        processLine(lineBuf_);
        lineBuf_.clear();
    }

    // 关闭所有打开的块
    flushParagraph();
    closeAllBlocks();
    inCodeBlock_ = false;
    inList_ = false;

    // 结束文档
    if (docStarted_ && !docEnded_)
    {
        sax_.endDocument();
        docEnded_ = true;
    }
}


void MarkdownParser::reset()
{
    blockStack_.clear();
    lineBuf_.clear();
    paraBuf_.clear();

    inCodeBlock_ = false;
    codeFenceChar_ = 0;
    codeFenceCnt_ = 0;
    codeFenceLang_.clear();

    docStarted_ = false;
    docEnded_ = false;
    inList_ = false;
    listOrdered_ = false;
}



// ============================================================================
// 行处理
// ============================================================================

void MarkdownParser::processLine(StringView line)
{
    // 代码块模式
    if (inCodeBlock_)
    {
        if (isClosingFence(line))
        {
            inCodeBlock_ = false;
            sax_.endCodeBlock();
            return;
        }
        sax_.codeLine(line);
        return;
    }

    // 普通块级模式
    processBlockLine(line);
}

void MarkdownParser::processBlockLine(StringView line)
{
    // ---- 空行 ----
    size_t nonBlank = 0;
    while (nonBlank < line.size() && (line[nonBlank] == ' ' || line[nonBlank] == '\t'))
        ++nonBlank;
    if (nonBlank == line.size())
    {
        flushParagraph();
        if (inList_)
        {
            closeToBlock(eBlockList);
            closeTop(); // 关闭列表
        }
        if (hasBlock(eBlockQuote))
        {
            closeToBlock(eBlockQuote);
            closeTop();
        }
        return;
    }

    StringView content;
    int level = 0, number = 0;

    // ---- 标题 # ----
    if (isHeadingLine(line, level, content))
    {
        flushParagraph();
        closeToBlock(eBlockNone); // 标题打破所有块
        // 确保块栈清空（标题是叶子块，不压栈）
        closeAllBlocks();
        ensureDocStarted();
        sax_.startHeading(level);
        parseInline(content);
        sax_.endHeading(level);
        return;
    }

    // ---- 代码围栏 ``` 或 ~~~ ----
    char fc; int cnt; StringView lang;
    if (isCodeFenceLine(line, fc, cnt, lang))
    {
        flushParagraph();
        if (!inCodeBlock_)
        {
            // 开启代码块
            closeToBlock(eBlockNone);
            closeAllBlocks();
            ensureDocStarted();
            sax_.startCodeBlock(lang);
            inCodeBlock_ = true;
            codeFenceChar_ = fc;
            codeFenceCnt_  = cnt;
            codeFenceLang_.assign(lang.data(), lang.size());
        }
        // 闭合围栏由 processLine() 中 isClosingFence() 处理
        // 这里不会到达（因为 inCodeBlock_ 为 true 时由 processLine 拦截）
        return;
    }

    // ---- 分割线 --- / *** / ___ ----
    if (isHorizRuleLine(line))
    {
        flushParagraph();
        closeToBlock(eBlockNone);
        closeAllBlocks();
        ensureDocStarted();
        sax_.horizontalRule();
        return;
    }

    // ---- 引用 > ----
    if (isBlockquoteLine(line, content))
    {
        flushParagraph();
        ensureDocStarted();

        if (!hasBlock(eBlockQuote))
        {
            sax_.startBlockquote();
            BlockFrame f = { eBlockQuote, 0, false };
            blockStack_.push_back(f);
        }

        if (!content.empty())
        {
            ensureParagraph();
            parseInline(content);
        }
        return;
    }

    // ---- 无序列表 - / * / + ----
    if (isUnorderedListItem(line, content))
    {
        flushParagraph();
        ensureDocStarted();

        // 如果已在引用中，先关闭
        if (hasBlock(eBlockQuote)) { closeToBlock(eBlockQuote); closeTop(); }

        if (!inList_ || listOrdered_)
        {
            if (inList_) { closeToBlock(eBlockList); closeTop(); }
            sax_.startList(false);
            BlockFrame f = { eBlockList, 0, false };
            blockStack_.push_back(f);
            inList_ = true;
            listOrdered_ = false;
        }

        sax_.startListItem();
        parseInline(content);
        sax_.endListItem();
        return;
    }

    // ---- 有序列表 N. ----
    if (isOrderedListItem(line, content, number))
    {
        flushParagraph();
        ensureDocStarted();

        if (hasBlock(eBlockQuote)) { closeToBlock(eBlockQuote); closeTop(); }

        if (!inList_ || !listOrdered_)
        {
            if (inList_) { closeToBlock(eBlockList); closeTop(); }
            sax_.startList(true);
            BlockFrame f = { eBlockList, 0, true };
            blockStack_.push_back(f);
            inList_ = true;
            listOrdered_ = true;
        }

        sax_.startListItem();
        parseInline(content);
        sax_.endListItem();
        return;
    }

    // ---- 默认：段落文本 ----
    ensureDocStarted();

    // 非段落延续行：关闭引用和列表
    if (hasBlock(eBlockQuote)) { closeToBlock(eBlockQuote); closeTop(); }
    if (inList_) { closeToBlock(eBlockList); closeTop(); }

    ensureParagraph();
    // 段落内换行 = 软换行，以空格连接
    if (!paraBuf_.empty())
        parseInline(" ");
    parseInline(line);
    paraBuf_.append(line.data(), line.size());
    paraBuf_ += '\n';
}

// ============================================================================
// 行内解析
// ============================================================================

size_t MarkdownParser::findClosingMarker(StringView text, StringView marker)
{
    char m0 = marker[0];
    char m1 = marker.size() > 1 ? marker[1] : 0;

    for (size_t i = 0; i < text.size(); ++i)
    {
        // 跳过转义
        if (text[i] == '\\' && i + 1 < text.size()) { ++i; continue; }

        if (text[i] != m0) continue;

        if (marker.size() == 1)
        {
            // 单字符标记：确保不是双字符的一部分
            if (m0 == '*' && i + 1 < text.size() && text[i + 1] == '*') continue;
            if (m0 == '_' && i + 1 < text.size() && text[i + 1] == '_') continue;
            if (m0 == '~' && i + 1 < text.size() && text[i + 1] == '~') continue;
            return i;
        }
        else
        {
            if (i + 1 < text.size() && text[i + 1] == m1) return i;
        }
    }
    return StringView::npos;
}

void MarkdownParser::parseInline(StringView text)
{
    size_t pos = 0;

    while (pos < text.size())
    {
        char c = text[pos];

        // ---- 转义 ----
        if (c == '\\' && pos + 1 < text.size())
        {
            sax_.text(text.substr(pos + 1, 1));
            pos += 2;
            continue;
        }

        // ---- 粗体 **...** ----
        if (c == '*' && pos + 1 < text.size() && text[pos + 1] == '*')
        {
            size_t cl = findClosingMarker(text.substr(pos + 2), "**");
            if (cl != StringView::npos)
            {
                sax_.startStrong();
                parseInline(text.substr(pos + 2, cl));
                sax_.endStrong();
                pos += 2 + cl + 2;
                continue;
            }
        }

        // ---- 斜体 *...*（单 *，不匹配 **） ----
        if (c == '*')
        {
            size_t cl = findClosingMarker(text.substr(pos + 1), "*");
            if (cl != StringView::npos)
            {
                sax_.startEmphasis();
                parseInline(text.substr(pos + 1, cl));
                sax_.endEmphasis();
                pos += 1 + cl + 1;
                continue;
            }
        }

        // ---- 斜体 _..._ ----
        if (c == '_')
        {
            size_t cl = findClosingMarker(text.substr(pos + 1), "_");
            if (cl != StringView::npos)
            {
                sax_.startEmphasis();
                parseInline(text.substr(pos + 1, cl));
                sax_.endEmphasis();
                pos += 1 + cl + 1;
                continue;
            }
        }

        // ---- 删除线 ~~...~~ ----
        if (c == '~' && pos + 1 < text.size() && text[pos + 1] == '~')
        {
            size_t cl = findClosingMarker(text.substr(pos + 2), "~~");
            if (cl != StringView::npos)
            {
                // SAX 接口暂无 startStrike/endStrike，保留内容文本
                parseInline(text.substr(pos + 2, cl));
                pos += 2 + cl + 2;
                continue;
            }
        }

        // ---- 行内代码 `...` ----
        if (c == '`')
        {
            size_t cl = text.find('`', pos + 1);
            if (cl != StringView::npos && cl < text.size())
            {
                sax_.codeSpan(text.substr(pos + 1, cl - pos - 1));
                pos = cl + 1;
                continue;
            }
        }

        // ---- 图片 ![alt](url) — 先于链接检查 ----
        if (c == '!' && pos + 1 < text.size() && text[pos + 1] == '[')
        {
            size_t cb = text.find(']', pos + 2);
            if (cb != StringView::npos && cb < text.size() && cb + 1 < text.size() && text[cb + 1] == '(')
            {
                size_t ce = text.find(')', cb + 2);
                if (ce != StringView::npos && ce < text.size())
                {
                    sax_.image(text.substr(pos + 2, cb - pos - 2),
                               text.substr(cb + 2, ce - cb - 2));
                    pos = ce + 1;
                    continue;
                }
            }
        }

        // ---- 链接 [text](url) ----
        if (c == '[')
        {
            size_t cb = text.find(']', pos + 1);
            if (cb != StringView::npos && cb < text.size() && cb + 1 < text.size() && text[cb + 1] == '(')
            {
                size_t ce = text.find(')', cb + 2);
                if (ce != StringView::npos && ce < text.size())
                {
                    sax_.startLink(text.substr(cb + 2, ce - cb - 2));
                    parseInline(text.substr(pos + 1, cb - pos - 1));
                    sax_.endLink();
                    pos = ce + 1;
                    continue;
                }
            }
        }

        // ---- 普通字符 ----
        sax_.text(text.substr(pos, 1));
        ++pos;
    }
}

AST_NAMESPACE_END