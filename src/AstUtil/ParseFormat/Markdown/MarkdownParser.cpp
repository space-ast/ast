///
/// @file      MarkdownParser.cpp
/// @brief     Markdown SAX 解析器 — 流式状态机实现
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownParser.hpp"
#include "MarkdownSax.hpp"
#include <cctype>
#include <cstring>

AST_NAMESPACE_BEGIN

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

void MarkdownParser::flush()
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
