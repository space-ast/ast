///
/// @file      MarkdownParser.cpp
/// @brief     Markdown SAX 解析器 — 流式状态机实现
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "MarkdownParser.hpp"
#include "MarkdownSax.hpp"
#include "MarkdownANSI.hpp"
#include "MarkdownHTML.hpp"
#include "AstUtil/StringView.hpp"
#include <cctype>
#include <cstring>

AST_NAMESPACE_BEGIN


std::string aMarkdownANSI(StringView markdown)
{
    MarkdownANSI ansi;
    MarkdownParser md(ansi);
    md.feed(markdown);
    md.finish();
    return ansi.output();
}


std::string aMarkdownHTML(StringView markdown, bool compact)
{
    MarkdownHTML html(compact);
    MarkdownParser md(html);
    md.feed(markdown);
    md.finish();
    return html.output();
}

AST_NAMESPACE_END
