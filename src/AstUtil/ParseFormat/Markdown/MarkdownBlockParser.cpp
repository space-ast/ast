///
/// @file      MarkdownBlockParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "MarkdownBlockParser.hpp"
#include "MarkdownInlineParser.hpp"
#include "MarkdownTableParser.hpp"
#include "MarkdownSax.hpp"

AST_NAMESPACE_BEGIN


// ============================================================================
// MarkdownBlockParser — 公共接口
// ============================================================================

MarkdownBlockParser::MarkdownBlockParser(MarkdownSax& sax)
    : sax_(sax)
    , inlineSM_(sax)
    , tableSM_(new MarkdownTableParser(sax))
{
}

MarkdownBlockParser::~MarkdownBlockParser() = default;

void MarkdownBlockParser::feed(StringView data)
{
    for (size_t i = 0; i < data.size(); ++i)
        feedChar(data[i]);
    // 每个 chunk 末尾 flush 累积的行内内容（实现流式输出）
    flushInlineContent();
}

void MarkdownBlockParser::finish()
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

    // 关闭表格
    tableSM_->finish();

    // 关闭所有剩余块（自顶向下，确保内层 List 先于外层 ListItem 关闭）
    while (!blockStack_.empty())
    {
        BlockFrame& top = blockStack_.back();
        switch (top.type)
        {
        case EBlockType::ListItem: sax_.endListItem(); break;
        case EBlockType::List:     sax_.endList();     break;
        case EBlockType::Paragraph:  sax_.endParagraph();  break;
        case EBlockType::Blockquote: sax_.endBlockquote(); break;
        case EBlockType::CodeBlock:  sax_.endCodeBlock();  break;
        default: break;
        }
        blockStack_.pop_back();
    }
    inList_ = false;

    // ---- 结束行内状态机 ----
    inlineSM_.finish();

    // ---- 结束文档 ----
    if (docStarted_)
    {
        sax_.endDocument();
        docStarted_ = false;
    }
}

void MarkdownBlockParser::reset()
{
    blockStack_.clear();
    classBuf_.clear();
    contentBuf_.clear();
    codeLineBuf_.clear();

    lineFresh_ = true; state_ = EState::eLineStart;

    headingLevel_ = 0;

    codeFenceChar_ = 0;
    codeFenceCount_ = 0;
    codeFenceLang_.clear();

    inList_ = false;
    listOrdered_ = false;
    currentLineIndent_ = 0;
    listItemAfterNL_  = false;

    paraAfterNL_ = false;
    paraHadContent_ = false;

    docStarted_ = false;

    // 重置表格状态机
    tableSM_->reset();

    // 重置行内状态机（引用成员不可赋值，使用 placement new 重建）
    inlineSM_.~MarkdownInlineParser();
    new (&inlineSM_) MarkdownInlineParser(sax_);
}

// ============================================================================
// MarkdownBlockParser — 逐字符核心
// ============================================================================

void MarkdownBlockParser::feedChar(char c)
{
    switch (state_)
    {
    case EState::eLineStart:
        // ---- 每行首次进入时重置缩进计数 ----
        if (lineFresh_)
        {
            currentLineIndent_ = 0;
            lineFresh_ = false;
        }

        // ---- 计数行首空白（用于缩进比较） ----
        if (c == ' ' || c == '\t')
        {
            currentLineIndent_++;
            return;
        }
        // 非空白字符：currentLineIndent_ 已累积完毕，后续 classifyFirstChar 使用

        // ---- 空行（\n 紧跟 \n） ----
        if (c == '\n')
        {
            // 处理延迟的列表项关闭
            closePendingListItem();
            currentLineIndent_ = 0;
            listItemAfterNL_ = false;
            // 结束当前段落
            if (hasBlock(EBlockType::Paragraph))
                endParagraph();
            // 结束所有列表
            while (hasBlock(EBlockType::List))
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

        // ---- 延迟的列表项关闭：根据缩进决定是否保持打开 ----
        if (listItemAfterNL_)
        {
            listItemAfterNL_ = false;
            int listIndent = currentListIndent();

            if (currentLineIndent_ <= listIndent)
            {
                // 缩进相同 → 关闭前一个列表项
                closePendingListItem();
                // 缩进更浅 → 逐层关闭内嵌列表及包裹它们的列表项
                while (hasBlock(EBlockType::List) && currentListIndent() > currentLineIndent_)
                {
                    closeToType(EBlockType::List);  // 关闭内层 List
                    closePendingListItem();          // 关闭刚成为栈顶的父列表项
                    inList_ = hasBlock(EBlockType::List);
                }
            }
            // else: 缩进更深 → 保持列表项打开（可能包含嵌套列表或续行内容）
        }

        // ---- 段落空行检查：若 paraAfterNL_ 且行首字符并非块标记，继续段落 ----
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
            // 不在此处关闭列表项 —— 延迟到下一行根据缩进决定
            listItemAfterNL_ = true;
            currentLineIndent_ = 0;
            lineFresh_ = true; state_ = EState::eLineStart;
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
                lineFresh_ = true; state_ = EState::eLineStart;
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

void MarkdownBlockParser::flushInlineContent()
{
    flushInlineContent(false);
}

void MarkdownBlockParser::flushInlineContent(bool force)
{
    if (contentBuf_.empty()) return;
    // 反引号批处理：非强制模式下，末尾为反引号时不 flush，
    // 等待后续字符以正确计数多反引号序列
    if (!force && contentBuf_.back() == '`') return;
    StringView chunk(contentBuf_.data(), contentBuf_.size());
    inlineSM_.feed(chunk);
    contentBuf_.clear();
}

void MarkdownBlockParser::closeParagraph()
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

void MarkdownBlockParser::ensureListBlock(bool ordered, int indent)
{
    // 没有已打开的列表 → 直接开始
    if (!inList_)
    {
        sax_.startList(ordered);
        openBlock(EBlockType::List, indent, ordered);
        inList_ = true;
        listOrdered_ = ordered;
        return;
    }

    int topIndent = currentListIndent();

    // 缩进更深 → 开始嵌套列表（不关闭父列表项）
    if (indent > topIndent)
    {
        sax_.startList(ordered);
        openBlock(EBlockType::List, indent, ordered);
        listOrdered_ = ordered;
        return;
    }

    // 缩进相同 → 同层新列表（若类型不同则替换，否则复用）
    if (indent == topIndent)
    {
        if (listOrdered_ != ordered)
        {
            closeToType(EBlockType::List);
            sax_.startList(ordered);
            openBlock(EBlockType::List, indent, ordered);
            inList_ = true;
            listOrdered_ = ordered;
        }
        // 同类型 → 复用现有列表块，不发出新 startList
        return;
    }

    // 缩进更浅 → 关闭内层列表直到找到匹配级别
    while (inList_ && currentListIndent() > indent)
    {
        // 安全关闭嵌套列表及其最后一项
        closePendingListItem();
        closeToType(EBlockType::List);
        inList_ = hasBlock(EBlockType::List);
    }

    // 到达匹配缩进层
    if (!inList_)
    {
        sax_.startList(ordered);
        openBlock(EBlockType::List, indent, ordered);
        inList_ = true;
        listOrdered_ = ordered;
    }
    else if (listOrdered_ != ordered)
    {
        closeToType(EBlockType::List);
        sax_.startList(ordered);
        openBlock(EBlockType::List, indent, ordered);
        inList_ = true;
        listOrdered_ = ordered;
    }
}

int MarkdownBlockParser::currentListIndent() const
{
    for (int i = static_cast<int>(blockStack_.size()) - 1; i >= 0; --i)
    {
        if (blockStack_[i].type == EBlockType::List)
            return blockStack_[i].level;
    }
    return -1;
}

void MarkdownBlockParser::closePendingListItem()
{
    // 只关闭最内层的一个延迟列表项（嵌套项由调用方逐层关闭）
    if (!hasBlock(EBlockType::ListItem)) return;
    sax_.endListItem();
    closeToType(EBlockType::ListItem);
    listItemAfterNL_ = false;
}

// ============================================================================
// 行首分类
// ============================================================================

void MarkdownBlockParser::classifyFirstChar(char c)
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
        ensureParagraph();  // 为引用内容打开段落，触发 emitBlockPrefix 输出 │ 前缀
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
        // 表格：关闭段落，委托给 TableParser
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

void MarkdownBlockParser::handleHeadingHashes(char c)
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

void MarkdownBlockParser::handleHeadingChar(char c)
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
        lineFresh_ = true; state_ = EState::eLineStart;
    }
    else
    {
        contentBuf_ += c;
    }
}

// ============================================================================
// 代码围栏处理
// ============================================================================

void MarkdownBlockParser::handleCodeFenceOpen(char c)
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
            lineFresh_ = true; state_ = EState::eLineStart;
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

void MarkdownBlockParser::handleCodeBlockChar(char c)
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
                lineFresh_ = true; state_ = EState::eLineStart;
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

void MarkdownBlockParser::handleBlockquoteChar(char c)
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
        lineFresh_ = true; state_ = EState::eLineStart;
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

void MarkdownBlockParser::handleListMarker(char c)
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
                lineFresh_ = true; state_ = EState::eLineStart;
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
        lineFresh_ = true; state_ = EState::eLineStart;
        return;
    }

    if (first == '-' || first == '*' || first == '+')
    {
        // 无序列表：检查 "- "/"* "/"+ " 格式
        if (c == ' ')
        {
            // 确认无序列表
            closeParagraph();
            ensureListBlock(false, currentLineIndent_);
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
            ensureListBlock(true, currentLineIndent_);
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

void MarkdownBlockParser::handleParagraphChar(char c)
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

void MarkdownBlockParser::handleTableChar(char c)
{
    tableSM_->feedChar(c);

    if (tableSM_->isIdle())
    {
        // 表格结束，检查是否需要重新处理未消费字符
        if (!tableSM_->isCharConsumed())
        {
            // 当前字符未被表格消费 → 重新做行首分类
            lineFresh_ = true; state_ = EState::eLineStart;
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
            lineFresh_ = true; state_ = EState::eLineStart;
        }
    }
}

// ============================================================================
// 块栈管理
// ============================================================================

void MarkdownBlockParser::openBlock(EBlockType type, int level, bool ordered)
{
    BlockFrame f = { type, level, ordered };
    blockStack_.push_back(f);
}

void MarkdownBlockParser::closeTop()
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
        // 检查是否还有其他 List 块（支持嵌套），
        // 遍历除当前 top 外的栈帧：
        // pop_back 尚未执行，所以 blockStack_.size()-1 即为不包括 top 的大小
        inList_ = false;
        for (size_t i = 0; i + 1 < blockStack_.size(); ++i)
        {
            if (blockStack_[i].type == EBlockType::List)
            {
                inList_ = true;
                listOrdered_ = blockStack_[i].ordered;
                break;
            }
        }
        break;
    }
    blockStack_.pop_back();
}

void MarkdownBlockParser::closeToType(EBlockType type)
{
    while (!blockStack_.empty() && blockStack_.back().type != type)
        closeTop();
    if (!blockStack_.empty() && blockStack_.back().type == type)
        closeTop();
}

void MarkdownBlockParser::closeAllBlocks()
{
    while (!blockStack_.empty())
        closeTop();
}

bool MarkdownBlockParser::hasBlock(EBlockType type) const
{
    for (size_t i = 0; i < blockStack_.size(); ++i)
        if (blockStack_[i].type == type) return true;
    return false;
}

// ============================================================================
// 段落 / 文档辅助
// ============================================================================

void MarkdownBlockParser::ensureParagraph()
{
    if (!hasBlock(EBlockType::Paragraph))
    {
        sax_.startParagraph();
        openBlock(EBlockType::Paragraph);
    }
}

void MarkdownBlockParser::endParagraph()
{
    flushInlineContent(true);
    if (hasBlock(EBlockType::Paragraph))
    {
        closeToType(EBlockType::Paragraph);
    }
    paraAfterNL_ = false;
    paraHadContent_ = false;
}

void MarkdownBlockParser::ensureDocStarted()
{
    if (!docStarted_)
    {
        sax_.startDocument();
        docStarted_ = true;
    }
}

AST_NAMESPACE_END
