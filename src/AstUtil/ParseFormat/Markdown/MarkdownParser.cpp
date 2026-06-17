///
/// @file      MarkdownParser.cpp
/// @brief     Markdown SAX 解析器 — 流式状态机实现
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownParser.hpp"
#include "MarkdownSax.hpp"
#include "MarkdownTableStateMachine.hpp"
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
        if (mode_ == EParseMode::eNormal && c == '`' && !(pendingState_ & EStateFlags::eEscape))
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

void MarkdownInlineStateMachine::flushCell()
{
    // 解析末尾待提交的定界符游程（如行末 * 或 _）
    resolveDelimRun(0, result_);
    // 触发待定的删除线
    if (!!(pendingState_ & EStateFlags::eDelete))
    {
        toggleState(EStateFlags::eDelete);
        pendingState_ &= ~EStateFlags::eDelete;
    }
    // 刷新待提交文本
    flushPending(result_);
    // 关闭所有活跃格式（table cell 之间格式不跨 cell）
    closeAllFormats(result_);
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
// MarkdownBlockStateMachine — 公共接口
// ============================================================================

MarkdownBlockStateMachine::MarkdownBlockStateMachine(MarkdownSax& sax)
    : sax_(sax)
    , inlineSM_(sax)
    , tableSM_(new MarkdownTableStateMachine(sax))
{
}

MarkdownBlockStateMachine::~MarkdownBlockStateMachine() = default;

void MarkdownBlockStateMachine::feed(StringView data)
{
    for (size_t i = 0; i < data.size(); ++i)
        feedChar(data[i]);
    // 每个 chunk 末尾 flush 累积的行内内容（实现流式输出）
    flushInlineContent();
}

void MarkdownBlockStateMachine::finish()
{
    // ---- 刷新待提交的行内内容 ----
    flushInlineContent(true);

    // ---- 处理未闭合的块 ----
    if (state_ == EState::eHeadingContent)
    {
        sax_.endHeading(headingLevel_);
        closeToType(EBlockType::Heading);
    }

    if (state_ == EState::eCodeBlockContent)
    {
        // 输出未闭合的代码块最后一行
        if (!codeLineBuf_.empty())
            sax_.codeLine(codeLineBuf_);
        sax_.endCodeBlock();
        closeToType(EBlockType::CodeBlock);
    }

    // 关闭段落
    if (hasBlock(EBlockType::Paragraph))
        endParagraph();

    // 关闭列表
    if (hasBlock(EBlockType::ListItem))
        closeToType(EBlockType::ListItem);
    if (hasBlock(EBlockType::List))
        closeToType(EBlockType::List);

    // 关闭表格
    tableSM_->finish();

    // 关闭所有剩余块
    closeAllBlocks();

    // ---- 结束行内状态机 ----
    inlineSM_.finish();

    // ---- 结束文档 ----
    if (docStarted_)
    {
        sax_.endDocument();
        docStarted_ = false;
    }
}

void MarkdownBlockStateMachine::reset()
{
    blockStack_.clear();
    classBuf_.clear();
    contentBuf_.clear();
    codeLineBuf_.clear();

    state_ = EState::eLineStart;

    headingLevel_ = 0;

    codeFenceChar_ = 0;
    codeFenceCount_ = 0;
    codeFenceLang_.clear();

    inList_ = false;
    listOrdered_ = false;

    paraAfterNL_ = false;
    paraHadContent_ = false;

    docStarted_ = false;

    // 重置表格状态机
    tableSM_->reset();

    // 重置行内状态机（引用成员不可赋值，使用 placement new 重建）
    inlineSM_.~MarkdownInlineStateMachine();
    new (&inlineSM_) MarkdownInlineStateMachine(sax_);
}

// ============================================================================
// MarkdownBlockStateMachine — 逐字符核心
// ============================================================================

void MarkdownBlockStateMachine::feedChar(char c)
{
    switch (state_)
    {
    case EState::eLineStart:
        // ---- 跳过行首空白 ----
        if (c == ' ' || c == '\t')
            return;

        // ---- 空行（\n 紧跟 \n） ----
        if (c == '\n')
        {
            // 结束当前段落
            if (hasBlock(EBlockType::Paragraph))
                endParagraph();
            // 结束列表
            if (hasBlock(EBlockType::ListItem))
                closeToType(EBlockType::ListItem);
            if (hasBlock(EBlockType::List))
            {
                closeToType(EBlockType::List);
                inList_ = false;
            }
            // 结束引用
            if (hasBlock(EBlockType::Blockquote))
                closeToType(EBlockType::Blockquote);
            paraAfterNL_ = false;
            return;
        }

        // ---- 段落空行检查：若 paraAfterNL_ 且行首字符并非块标记，继续段落 ----
        // 这个判断由 classifyFirstChar 完成
        classifyFirstChar(c);
        break;

    case EState::eHeadingHashes:
        handleHeadingHashes(c);
        break;

    case EState::eHeadingContent:
        handleHeadingChar(c);
        break;

    case EState::eCodeFenceOpen:
        handleCodeFenceOpen(c);
        break;

    case EState::eCodeBlockContent:
        handleCodeBlockChar(c);
        break;

    case EState::eBlockquoteContent:
        handleBlockquoteChar(c);
        break;

    case EState::eListMarker:
        if (c == '\n')
        {
            // 行末 — 检查是否为水平分割线
            handleListMarker(c);
        }
        else
        {
            handleListMarker(c);
        }
        break;

    case EState::eListItemContent:
        if (c == '\n')
        {
            flushInlineContent(true);
            {
                std::string dummy;
                inlineSM_.feed(c, dummy);
            }
            sax_.endListItem();
            closeToType(EBlockType::ListItem);
            state_ = EState::eLineStart;
        }
        else
        {
            contentBuf_ += c;
        }
        break;

    case EState::eTable:
        handleTableChar(c);
        break;

    case EState::eParagraph:
        if (paraAfterNL_)
        {
            // 段落中刚看到 \n，本字符判定软换行还是段落结束
            paraAfterNL_ = false;

            // 空行 → 结束段落
            if (c == '\n')
            {
                endParagraph();
                // 关闭列表 / 引用
                if (hasBlock(EBlockType::ListItem))
                    closeToType(EBlockType::ListItem);
                if (hasBlock(EBlockType::List))
                {
                    closeToType(EBlockType::List);
                    inList_ = false;
                }
                if (hasBlock(EBlockType::Blockquote))
                    closeToType(EBlockType::Blockquote);
                state_ = EState::eLineStart;
                break;
            }

            // 跳过行首空白
            if (c == ' ' || c == '\t')
                break;

            // 检查是否遇到新块元素 → 结束段落并重新分类
            if (c == '#' || c == '>' || c == '`' || c == '~' ||
                c == '-' || c == '*' || c == '+' || c == '_' || c == '|' ||
                (c >= '0' && c <= '9'))
            {
                endParagraph();
                // 重新作为行首分类
                classifyFirstChar(c);
                break;
            }

            // 普通字符 → 软换行（空格连接）
            // 先刷新缓存的内容，再加上空格
            flushInlineContent(true);
            contentBuf_ += ' ';
            contentBuf_ += c;
            paraHadContent_ = true;
        }
        else
        {
            handleParagraphChar(c);
        }
        break;
    }
}

// ============================================================================
// 行内内容刷新 / 段落辅助（classification 之前调用）
// ============================================================================

void MarkdownBlockStateMachine::flushInlineContent()
{
    flushInlineContent(false);
}

void MarkdownBlockStateMachine::flushInlineContent(bool force)
{
    if (contentBuf_.empty()) return;
    // 反引号批处理：非强制模式下，末尾为反引号时不 flush，
    // 等待后续字符以正确计数多反引号序列
    if (!force && contentBuf_.back() == '`') return;
    StringView chunk(contentBuf_.data(), contentBuf_.size());
    inlineSM_.feed(chunk);
    contentBuf_.clear();
}

void MarkdownBlockStateMachine::closeParagraph()
{
    // 结束当前段落（如果存在）
    if (hasBlock(EBlockType::Paragraph))
    {
        flushInlineContent(true);
        closeToType(EBlockType::Paragraph);
    }
    paraAfterNL_ = false;
    paraHadContent_ = false;
}

void MarkdownBlockStateMachine::ensureListBlock(bool ordered)
{
    if (!inList_ || listOrdered_ != ordered)
    {
        if (inList_)
        {
            closeToType(EBlockType::List);
        }
        sax_.startList(ordered);
        openBlock(EBlockType::List, 0, ordered);
        inList_ = true;
        listOrdered_ = ordered;
    }
}

// ============================================================================
// 行首分类
// ============================================================================

void MarkdownBlockStateMachine::classifyFirstChar(char c)
{
    // ---- 正常行首分类 ----
    ensureDocStarted();

    // 非引用延续行：关闭已打开的引用块
    if (c != '>' && hasBlock(EBlockType::Blockquote))
        closeToType(EBlockType::Blockquote);

    switch (c)
    {
    case '#':
        headingLevel_ = 1;
        classBuf_ = '#';
        state_ = EState::eHeadingHashes;
        break;

    case '`':
        codeFenceChar_ = '`';
        codeFenceCount_ = 1;
        classBuf_ = '`';
        state_ = EState::eCodeFenceOpen;
        break;

    case '~':
        codeFenceChar_ = '~';
        codeFenceCount_ = 1;
        classBuf_ = '~';
        state_ = EState::eCodeFenceOpen;
        break;

    case '>':
        // 引用
        closeParagraph();
        if (!hasBlock(EBlockType::Blockquote))
        {
            sax_.startBlockquote();
            openBlock(EBlockType::Blockquote);
        }
        state_ = EState::eBlockquoteContent;
        break;

    case '-':
    case '*':
    case '+':
    case '_':
        classBuf_ = c;
        state_ = EState::eListMarker;
        break;

    case '|':
        // 表格：关闭段落，委托给 TableStateMachine
        closeParagraph();
        ensureDocStarted();
        tableSM_->feedChar(c);
        state_ = EState::eTable;
        break;

    default:
        if (c >= '0' && c <= '9')
        {
            classBuf_ = c;
            state_ = EState::eListMarker;
        }
        else
        {
            // 默认：段落
            closeParagraph();
            ensureParagraph();
            contentBuf_ += c;
            paraHadContent_ = true;
            state_ = EState::eParagraph;
        }
        break;
    }
}

// ============================================================================
// 标题处理
// ============================================================================

void MarkdownBlockStateMachine::handleHeadingHashes(char c)
{
    if (c == '#' && headingLevel_ < 6)
    {
        ++headingLevel_;
        classBuf_ += '#';
        return;
    }

    if (c == ' ')
    {
        // 标题已确认：# 后跟空格
        openBlock(EBlockType::Heading, headingLevel_, false);
        sax_.startHeading(headingLevel_);
        classBuf_.clear();
        state_ = EState::eHeadingContent;
        return;
    }

    // # 后不是空格 → 不是标题，classBuf_ 中的 # 作为段落文本
    closeParagraph();
    ensureParagraph();
    contentBuf_ += classBuf_;
    classBuf_.clear();
    headingLevel_ = 0;
    contentBuf_ += c;
    paraHadContent_ = true;
    state_ = EState::eParagraph;
}

void MarkdownBlockStateMachine::handleHeadingChar(char c)
{
    if (c == '\n')
    {
        flushInlineContent(true);
        {
            std::string dummy;
            inlineSM_.feed(c, dummy);
        }
        sax_.endHeading(headingLevel_);
        closeToType(EBlockType::Heading);
        headingLevel_ = 0;
        state_ = EState::eLineStart;
    }
    else
    {
        contentBuf_ += c;
    }
}

// ============================================================================
// 代码围栏处理
// ============================================================================

void MarkdownBlockStateMachine::handleCodeFenceOpen(char c)
{
    if (c == codeFenceChar_)
    {
        ++codeFenceCount_;
        classBuf_ += c;
        return;
    }

    if (c == '\n')
    {
        if (codeFenceCount_ >= 3)
        {
            // 有效围栏开界
            openBlock(EBlockType::CodeBlock);
            sax_.startCodeBlock(codeFenceLang_);
            classBuf_.clear();
            codeFenceLang_.clear();
            codeLineBuf_.clear();
            state_ = EState::eCodeBlockContent;
        }
        else
        {
            // 不足3个 → 普通段落文本
            closeParagraph();
            ensureParagraph();
            contentBuf_ += classBuf_;
            // 喂入 \n 以关闭行内样式
            {
                std::string dummy;
                inlineSM_.feed('\n', dummy);
            }
            paraHadContent_ = true;
            classBuf_.clear();
            codeFenceChar_ = 0;
            codeFenceCount_ = 0;
            state_ = EState::eLineStart;
        }
        return;
    }

    if (codeFenceCount_ >= 3)
    {
        // 围栏已确认，后续字符为语言标识
        codeFenceLang_ += c;
        classBuf_ += c;
    }
    else
    {
        // 围栏未达3个 → 当作段落文本
        closeParagraph();
        ensureParagraph();
        contentBuf_ += classBuf_;
        contentBuf_ += c;
        paraHadContent_ = true;
        classBuf_.clear();
        codeFenceChar_ = 0;
        codeFenceCount_ = 0;
        state_ = EState::eParagraph;
    }
}

// ============================================================================
// 代码块内容处理
// ============================================================================

void MarkdownBlockStateMachine::handleCodeBlockChar(char c)
{
    if (c == '\n')
    {
        // 检查 codeLineBuf_ 是否为闭合围栏
        if (!codeLineBuf_.empty() && codeLineBuf_[0] == codeFenceChar_)
        {
            int cnt = 0;
            while (cnt < (int)codeLineBuf_.size() && codeLineBuf_[cnt] == codeFenceChar_)
                ++cnt;
            bool ok = true;
            for (size_t i = cnt; i < codeLineBuf_.size(); ++i)
                if (codeLineBuf_[i] != ' ' && codeLineBuf_[i] != '\t') { ok = false; break; }
            if (cnt >= codeFenceCount_ && ok)
            {
                // 闭合围栏
                sax_.endCodeBlock();
                closeToType(EBlockType::CodeBlock);
                codeLineBuf_.clear();
                codeFenceChar_ = 0;
                codeFenceCount_ = 0;
                state_ = EState::eLineStart;
                return;
            }
        }
        // 非闭合围栏 → 输出代码行
        sax_.codeLine(codeLineBuf_);
        codeLineBuf_.clear();
        return;
    }

    codeLineBuf_ += c;
}

// ============================================================================
// 引用处理
// ============================================================================

void MarkdownBlockStateMachine::handleBlockquoteChar(char c)
{
    if (c == '\n')
    {
        flushInlineContent(true);
        {
            std::string dummy;
            inlineSM_.feed(c, dummy);
        }
        // 清空 classBuf_，为下一行引用前缀判定做准备
        classBuf_.clear();
        state_ = EState::eLineStart;
        return;
    }

    // 跳过 > 之后的第一个可选空格（仅当本行尚未消费任何内容时）
    if (contentBuf_.empty() && classBuf_.empty() && c == ' ')
    {
        classBuf_ = "> "; // 标记前缀已消费
        return;
    }

    contentBuf_ += c;
}

// ============================================================================
// 列表标记处理
// ============================================================================

void MarkdownBlockStateMachine::handleListMarker(char c)
{
    char first = classBuf_.empty() ? 0 : classBuf_[0];

    // ---- \n 处理：行末检查是否为分割线 ----
    if (c == '\n')
    {
        // 检查分割线模式：classBuf_ 由 3+ 个相同字符 (-, *, _) 组成（可含空格）
        if (first == '-' || first == '*' || first == '_')
        {
            int sameCount = 0;
            bool onlySameAndSpace = true;
            for (size_t i = 0; i < classBuf_.size(); ++i)
            {
                if (classBuf_[i] == first)
                    ++sameCount;
                else if (classBuf_[i] != ' ' && classBuf_[i] != '\t')
                    { onlySameAndSpace = false; break; }
            }
            if (sameCount >= 3 && onlySameAndSpace)
            {
                // 水平分割线
                closeParagraph();
                ensureDocStarted();
                sax_.horizontalRule();
                classBuf_.clear();
                state_ = EState::eLineStart;
                return;
            }
        }
        // 不是分割线 → 作为段落文本输出
        closeParagraph();
        ensureParagraph();
        contentBuf_ += classBuf_;
        {
            std::string dummy;
            inlineSM_.feed('\n', dummy);
        }
        paraHadContent_ = true;
        classBuf_.clear();
        state_ = EState::eLineStart;
        return;
    }

    if (first == '-' || first == '*' || first == '+')
    {
        // 无序列表：检查 "- "/"* "/"+ " 格式
        if (c == ' ')
        {
            // 确认无序列表
            closeParagraph();
            ensureListBlock(false);
            sax_.startListItem();
            openBlock(EBlockType::ListItem);
            classBuf_.clear();
            state_ = EState::eListItemContent;
        }
        else if (c == first)
        {
            // 同字符累积（可能为分割线如 ---, ***）
            classBuf_ += c;
        }
        else
        {
            // 不是列表也不是分割线 → 段落
            closeParagraph();
            ensureParagraph();
            contentBuf_ += classBuf_;
            contentBuf_ += c;
            paraHadContent_ = true;
            classBuf_.clear();
            state_ = EState::eParagraph;
        }
        return;
    }

    if (first == '_')
    {
        // _ 不视为列表标记（_ 不是有效的无序列表标记），但可能是分割线 ___ 或斜体
        if (c == ' ' || c == first)
        {
            // _ 后跟空格 → 不是列表（与 - / * / + 不同），但可能为斜体段落或分割线
            // 继续累积以检查分割线
            classBuf_ += c;
        }
        else
        {
            // 其他字符 → 段落
            closeParagraph();
            ensureParagraph();
            contentBuf_ += classBuf_;
            contentBuf_ += c;
            paraHadContent_ = true;
            classBuf_.clear();
            state_ = EState::eParagraph;
        }
        return;
    }

    if (first >= '0' && first <= '9')
    {
        // 有序列表：累积数字
        if (c >= '0' && c <= '9')
        {
            classBuf_ += c;
            return;
        }
        if (c == '.')
        {
            classBuf_ += c;
            return;
        }
        if (c == ' ' && classBuf_.size() >= 2 && classBuf_.back() == '.')
        {
            // 确认有序列表 "N. "
            closeParagraph();
            ensureListBlock(true);
            sax_.startListItem();
            openBlock(EBlockType::ListItem);
            classBuf_.clear();
            state_ = EState::eListItemContent;
            return;
        }
        // 不符合有序列表格式 → 段落
        closeParagraph();
        ensureParagraph();
        contentBuf_ += classBuf_;
        contentBuf_ += c;
        paraHadContent_ = true;
        classBuf_.clear();
        state_ = EState::eParagraph;
        return;
    }
}

// ============================================================================
// 段落处理
// ============================================================================

void MarkdownBlockStateMachine::handleParagraphChar(char c)
{
    if (c == '\n')
    {
        flushInlineContent(true);
        {
            std::string dummy;
            inlineSM_.feed(c, dummy);
        }
        paraAfterNL_ = true;
        return;
    }

    contentBuf_ += c;
    paraHadContent_ = true;
}

// ============================================================================
// 表格处理
// ============================================================================

void MarkdownBlockStateMachine::handleTableChar(char c)
{
    tableSM_->feedChar(c);

    if (tableSM_->isIdle())
    {
        // 表格结束，检查是否需要重新处理未消费字符
        if (!tableSM_->isCharConsumed())
        {
            // 当前字符未被表格消费 → 重新做行首分类
            state_ = EState::eLineStart;
            // 跳过空白后重新分类（注意：此时 c 可能为空白，需被行首逻辑跳过）
            if (c != ' ' && c != '\t')
            {
                if (c == '\n')
                {
                    // 空行 — 已在 eLineStart 逻辑中处理
                    return;
                }
                classifyFirstChar(c);
            }
        }
        else
        {
            state_ = EState::eLineStart;
        }
    }
}

// ============================================================================
// 块栈管理
// ============================================================================

void MarkdownBlockStateMachine::openBlock(EBlockType type, int level, bool ordered)
{
    BlockFrame f = { type, level, ordered };
    blockStack_.push_back(f);
}

void MarkdownBlockStateMachine::closeTop()
{
    if (blockStack_.empty()) return;

    BlockFrame& top = blockStack_.back();
    switch (top.type)
    {
    case EBlockType::Paragraph: sax_.endParagraph();   break;
    case EBlockType::Heading:   /* heading closed by handleHeadingChar */ break;
    case EBlockType::CodeBlock: /* code block closed by handleCodeBlockChar */ break;
    case EBlockType::Blockquote: sax_.endBlockquote();  break;
    case EBlockType::ListItem:   /* list item closed by newline handler */ break;
    case EBlockType::List:
        sax_.endList();
        inList_ = false;
        break;
    }
    blockStack_.pop_back();
}

void MarkdownBlockStateMachine::closeToType(EBlockType type)
{
    while (!blockStack_.empty() && blockStack_.back().type != type)
        closeTop();
    if (!blockStack_.empty() && blockStack_.back().type == type)
        closeTop();
}

void MarkdownBlockStateMachine::closeAllBlocks()
{
    while (!blockStack_.empty())
        closeTop();
}

bool MarkdownBlockStateMachine::hasBlock(EBlockType type) const
{
    for (size_t i = 0; i < blockStack_.size(); ++i)
        if (blockStack_[i].type == type) return true;
    return false;
}

// ============================================================================
// 段落 / 文档辅助
// ============================================================================

void MarkdownBlockStateMachine::ensureParagraph()
{
    if (!hasBlock(EBlockType::Paragraph))
    {
        sax_.startParagraph();
        openBlock(EBlockType::Paragraph);
    }
}

void MarkdownBlockStateMachine::endParagraph()
{
    flushInlineContent(true);
    if (hasBlock(EBlockType::Paragraph))
    {
        closeToType(EBlockType::Paragraph);
    }
    paraAfterNL_ = false;
    paraHadContent_ = false;
}

void MarkdownBlockStateMachine::ensureDocStarted()
{
    if (!docStarted_)
    {
        sax_.startDocument();
        docStarted_ = true;
    }
}

AST_NAMESPACE_END
