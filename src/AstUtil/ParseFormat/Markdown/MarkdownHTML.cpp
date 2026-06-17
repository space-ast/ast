///
/// @file      MarkdownHTML.cpp
/// @brief     Markdown HTML 渲染器 — SAX 事件驱动，输出 HTML 标签
/// @author    axel
/// @date      2026-06-16
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

#include "MarkdownHTML.hpp"

AST_NAMESPACE_BEGIN

// ============================================================================
// HTML 转义辅助
// ============================================================================

void MarkdownHTML::appendEscaped(std::string& out, StringView text)
{
    for (size_t i = 0; i < text.size(); ++i)
    {
        switch (text[i])
        {
        case '&': out += "&amp;";  break;
        case '<': out += "&lt;";   break;
        case '>': out += "&gt;";   break;
        case '"': out += "&quot;"; break;
        default:  out += text[i];  break;
        }
    }
}

// ============================================================================
// 文档生命周期
// ============================================================================

void MarkdownHTML::startDocument()
{
    output_.clear();
    listStack_.clear();
    blockquoteDepth_ = 0;
    inTableHead_ = false;
    linkUrl_.clear();
}

void MarkdownHTML::endDocument()
{
    // 确保文档以换行结尾
    if (!output_.empty() && output_.back() != '\n')
        output_ += '\n';
}

// ============================================================================
// 块级元素 — 标题
// ============================================================================

void MarkdownHTML::startHeading(int level)
{
    output_ += "<h";
    output_ += ('0' + static_cast<char>(level));
    output_ += ">";
}

void MarkdownHTML::endHeading(int level)
{
    output_ += "</h";
    output_ += ('0' + static_cast<char>(level));
    output_ += ">\n";
}

// ============================================================================
// 块级元素 — 段落
// ============================================================================

void MarkdownHTML::startParagraph()
{
    output_ += "<p>";
}

void MarkdownHTML::endParagraph()
{
    output_ += "</p>\n";
}

// ============================================================================
// 块级元素 — 代码块
// ============================================================================

void MarkdownHTML::startCodeBlock(StringView language)
{
    output_ += "<pre><code";
    if (!language.empty())
    {
        output_ += " class=\"language-";
        output_.append(language.data(), language.size());
        output_ += "\"";
    }
    output_ += ">";
}

void MarkdownHTML::codeLine(StringView line)
{
    appendEscaped(output_, line);
    output_ += '\n';
}

void MarkdownHTML::endCodeBlock()
{
    output_ += "</code></pre>\n";
}

// ============================================================================
// 块级元素 — 列表
// ============================================================================

void MarkdownHTML::startList(bool ordered)
{
    listStack_.push_back(ordered);
    output_ += ordered ? "<ol>\n" : "<ul>\n";
}

void MarkdownHTML::startListItem()
{
    output_ += "<li>";
}

void MarkdownHTML::endListItem()
{
    output_ += "</li>\n";
}

void MarkdownHTML::endList()
{
    if (listStack_.empty()) return;

    bool ordered = listStack_.back();
    listStack_.pop_back();

    output_ += ordered ? "</ol>\n" : "</ul>\n";
}

// ============================================================================
// 块级元素 — 引用
// ============================================================================

void MarkdownHTML::startBlockquote()
{
    blockquoteDepth_++;
    output_ += "<blockquote>\n";
}

void MarkdownHTML::endBlockquote()
{
    if (blockquoteDepth_ > 0)
    {
        blockquoteDepth_--;
        output_ += "</blockquote>\n";
    }
}

// ============================================================================
// 块级元素 — 分割线
// ============================================================================

void MarkdownHTML::horizontalRule()
{
    output_ += "<hr>\n";
}

// ============================================================================
// 块级元素 — 表格
// ============================================================================

void MarkdownHTML::startTable()
{
    output_ += "<table>\n";
}

void MarkdownHTML::startTableHead()
{
    inTableHead_ = true;
    output_ += "<thead>\n";
}

void MarkdownHTML::endTableHead()
{
    output_ += "</thead>\n";
    inTableHead_ = false;
}

void MarkdownHTML::startTableBody()
{
    output_ += "<tbody>\n";
}

void MarkdownHTML::endTableBody()
{
    output_ += "</tbody>\n";
}

void MarkdownHTML::startTableRow()
{
    output_ += "<tr>";
}

void MarkdownHTML::endTableRow()
{
    output_ += "</tr>\n";
}

void MarkdownHTML::startTableCell(ETableAlign align)
{
    const char* tag = inTableHead_ ? "th" : "td";

    switch (align)
    {
    case ETableAlign::eLeft:
        output_ += "<";
        output_ += tag;
        output_ += " align=\"left\">";
        break;
    case ETableAlign::eCenter:
        output_ += "<";
        output_ += tag;
        output_ += " align=\"center\">";
        break;
    case ETableAlign::eRight:
        output_ += "<";
        output_ += tag;
        output_ += " align=\"right\">";
        break;
    case ETableAlign::eDefault:
    default:
        output_ += "<";
        output_ += tag;
        output_ += ">";
        break;
    }
}

void MarkdownHTML::endTableCell()
{
    output_ += inTableHead_ ? "</th>" : "</td>";
}

void MarkdownHTML::endTable()
{
    output_ += "</table>\n";
}

// ============================================================================
// 行内元素 — 文本
// ============================================================================

void MarkdownHTML::text(StringView txt)
{
    appendEscaped(output_, txt);
}

// ============================================================================
// 行内元素 — 斜体
// ============================================================================

void MarkdownHTML::startEmphasis()
{
    output_ += "<em>";
}

void MarkdownHTML::endEmphasis()
{
    output_ += "</em>";
}

// ============================================================================
// 行内元素 — 粗体
// ============================================================================

void MarkdownHTML::startStrong()
{
    output_ += "<strong>";
}

void MarkdownHTML::endStrong()
{
    output_ += "</strong>";
}

// ============================================================================
// 行内元素 — 删除线
// ============================================================================

void MarkdownHTML::startDelete()
{
    output_ += "<del>";
}

void MarkdownHTML::endDelete()
{
    output_ += "</del>";
}

// ============================================================================
// 行内元素 — 行内代码
// ============================================================================

void MarkdownHTML::codeSpan(StringView code)
{
    output_ += "<code>";
    appendEscaped(output_, code);
    output_ += "</code>";
}

// ============================================================================
// 行内元素 — 链接
// ============================================================================

void MarkdownHTML::startLink(StringView url)
{
    linkUrl_.assign(url.data(), url.size());

    output_ += "<a href=\"";
    // URL 属性值：仅转义 " 和 <（& 在 URL 中保留原样）
    for (size_t i = 0; i < url.size(); ++i)
    {
        switch (url[i])
        {
        case '"': output_ += "&quot;"; break;
        case '<': output_ += "&lt;";   break;
        default:  output_ += url[i];   break;
        }
    }
    output_ += "\">";
}

void MarkdownHTML::endLink()
{
    output_ += "</a>";
    linkUrl_.clear();
}

// ============================================================================
// 行内元素 — 图片
// ============================================================================

void MarkdownHTML::image(StringView alt, StringView url)
{
    output_ += "<img src=\"";
    // URL: 仅转义 " 和 <
    for (size_t i = 0; i < url.size(); ++i)
    {
        switch (url[i])
        {
        case '"': output_ += "&quot;"; break;
        case '<': output_ += "&lt;";   break;
        default:  output_ += url[i];   break;
        }
    }
    output_ += "\" alt=\"";
    appendEscaped(output_, alt);
    output_ += "\">";
}

AST_NAMESPACE_END
