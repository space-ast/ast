///
/// @file      testMarkdownBlock.cpp
/// @brief     MarkdownBlockStateMachine 块级流式解析单元测试
/// @details   使用 MarkdownParser + MarkdownHTML 验证块级元素的 HTML 输出，
///            覆盖段落、标题、代码块、引用、列表、分割线及跨 chunk 流式场景。
/// @author    axel
/// @date      2026-06-16
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "AstGlobal.h"
#include "ast/MarkdownParser.hpp"
#include "ast/MarkdownHTML.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"

#include <cstdio>

AST_USING_NAMESPACE

// ============================================================================
// 辅助工具
// ============================================================================

/// @brief 使用 MarkdownParser + MarkdownHTML 渲染 markdown 为 HTML
static std::string renderHtml(const std::string& markdown)
{
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed(markdown);
    parser.finish();
    return html.output();
}

/// @brief 检查子串是否存在
static bool has(const std::string& s, const std::string& sub)
{
    return s.find(sub) != std::string::npos;
}

/// @brief 渲染并打印，返回 HTML 字符串
static std::string show(const char* label, const std::string& markdown)
{
    std::string out = renderHtml(markdown);

    ast_printf("\n[%s]\n", label);
    ast_printf("[input]\n%s", markdown.c_str());
    if (!markdown.empty() && markdown.back() != '\n') ast_printf("\n");
    ast_printf("[output]\n%s", out.c_str());
    if (!out.empty() && out.back() != '\n') ast_printf("\n");

    return out;
}

// ============================================================================
// 一、段落
// ============================================================================

TEST(MarkdownBlockTest, PlainParagraph)
{
    std::string out = show("plain paragraph", "Hello World\n");
    EXPECT_TRUE(has(out, "<p>"));
    EXPECT_TRUE(has(out, "</p>"));
    EXPECT_TRUE(has(out, "Hello World"));
}

TEST(MarkdownBlockTest, MultiLineParagraph)
{
    // 软换行：同一段落的连续行以空格连接
    std::string out = show("multi-line paragraph",
        "Line one\nLine two\nLine three\n");
    EXPECT_TRUE(has(out, "<p>"));
    EXPECT_TRUE(has(out, "</p>"));
    EXPECT_TRUE(has(out, "Line one"));
    EXPECT_TRUE(has(out, "Line two"));
    EXPECT_TRUE(has(out, "Line three"));
}

TEST(MarkdownBlockTest, ParagraphBreak)
{
    // 空行分隔两个段落
    std::string out = show("paragraph break",
        "First paragraph.\n\nSecond paragraph.\n");
    EXPECT_TRUE(has(out, "<p>First paragraph.</p>"));
    EXPECT_TRUE(has(out, "<p>Second paragraph.</p>"));
}

TEST(MarkdownBlockTest, EmptyInput)
{
    std::string out = show("empty input", "");
    // 空输入应不生成文档标签
    EXPECT_FALSE(has(out, "<p>"));
}

TEST(MarkdownBlockTest, NoTrailingNewline)
{
    // 末行无换行符
    std::string out = show("no trailing newline", "No trailing newline");
    EXPECT_TRUE(has(out, "No trailing newline"));
    EXPECT_TRUE(has(out, "</p>"));
}

// ============================================================================
// 二、标题
// ============================================================================

TEST(MarkdownBlockTest, Heading1)
{
    std::string out = show("h1", "# Heading 1\n");
    EXPECT_TRUE(has(out, "<h1>Heading 1</h1>"));
}

TEST(MarkdownBlockTest, Heading2)
{
    std::string out = show("h2", "## Heading 2\n");
    EXPECT_TRUE(has(out, "<h2>Heading 2</h2>"));
}

TEST(MarkdownBlockTest, Heading3)
{
    std::string out = show("h3", "### Heading 3\n");
    EXPECT_TRUE(has(out, "<h3>Heading 3</h3>"));
}

TEST(MarkdownBlockTest, Heading6)
{
    std::string out = show("h6", "###### Heading 6\n");
    EXPECT_TRUE(has(out, "<h6>Heading 6</h6>"));
}

TEST(MarkdownBlockTest, NotHeadingHashTag)
{
    // #tag 没有空格 → 不是标题
    std::string out = show("not heading", "#tag text\n");
    EXPECT_TRUE(has(out, "#tag text"));
    EXPECT_FALSE(has(out, "<h1>"));
}

TEST(MarkdownBlockTest, HeadingWithInlineFormatting)
{
    std::string out = show("heading with bold", "# Hello **World**\n");
    EXPECT_TRUE(has(out, "<h1>Hello <strong>World</strong></h1>"));
}

// ============================================================================
// 三、代码块
// ============================================================================

TEST(MarkdownBlockTest, FencedCodeBlock)
{
    std::string out = show("code block ```",
        "```\nint main() {\n    return 0;\n}\n```\n");
    EXPECT_TRUE(has(out, "<pre><code>"));
    EXPECT_TRUE(has(out, "</code></pre>"));
    EXPECT_TRUE(has(out, "int main()"));
    EXPECT_TRUE(has(out, "return 0;"));
}

TEST(MarkdownBlockTest, FencedCodeBlockWithLang)
{
    std::string out = show("code block lang",
        "```cpp\nint x = 42;\n```\n");
    EXPECT_TRUE(has(out, "language-cpp"));
    EXPECT_TRUE(has(out, "int x = 42;"));
}

TEST(MarkdownBlockTest, TildeCodeBlock)
{
    std::string out = show("tilde code block",
        "~~~\ncode here\n~~~\n");
    EXPECT_TRUE(has(out, "<pre><code>"));
    EXPECT_TRUE(has(out, "code here"));
    EXPECT_TRUE(has(out, "</code></pre>"));
}

TEST(MarkdownBlockTest, CodeBlockPreservesMarkers)
{
    // 代码块内不应解析 Markdown 标记
    std::string out = show("code preserves markers",
        "```\n**not bold**\n*not italic*\n```\n");
    EXPECT_TRUE(has(out, "**not bold**"));
    EXPECT_TRUE(has(out, "*not italic*"));
    EXPECT_FALSE(has(out, "<strong>"));
    EXPECT_FALSE(has(out, "<em>"));
}

TEST(MarkdownBlockTest, UnclosedCodeBlock)
{
    // 未闭合的代码块：finish 时应自动关闭
    std::string out = show("unclosed code block",
        "```\nint x = 42;\n");
    EXPECT_TRUE(has(out, "<pre><code>"));
    EXPECT_TRUE(has(out, "int x = 42;"));
    // finish() 会关闭未闭合的块
}

// ============================================================================
// 四、引用
// ============================================================================

TEST(MarkdownBlockTest, SingleLineBlockquote)
{
    std::string out = show("single blockquote", "> quoted text\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "quoted text"));
}

TEST(MarkdownBlockTest, MultiLineBlockquote)
{
    std::string out = show("multi blockquote",
        "> line one\n> line two\n> line three\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    // 连续引用行应在一个 blockquote 内
    EXPECT_TRUE(has(out, "line one"));
    EXPECT_TRUE(has(out, "line two"));
    EXPECT_TRUE(has(out, "line three"));
    EXPECT_TRUE(has(out, "</blockquote>"));
}

TEST(MarkdownBlockTest, BlockquoteWithBold)
{
    std::string out = show("blockquote bold",
        "> quote with **bold** inside\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "<strong>bold</strong>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
}

TEST(MarkdownBlockTest, BlockquoteEndsAtBlankLine)
{
    std::string out = show("blockquote ends",
        "> quoted text\n\nnormal paragraph\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "normal paragraph"));
}

// ============================================================================
// 五、无序列表
// ============================================================================

TEST(MarkdownBlockTest, UnorderedListDash)
{
    std::string out = show("unordered list -",
        "- item A\n- item B\n- item C\n");
    EXPECT_TRUE(has(out, "<ul>"));
    EXPECT_TRUE(has(out, "</ul>"));
    EXPECT_TRUE(has(out, "<li>item A</li>"));
    EXPECT_TRUE(has(out, "<li>item B</li>"));
    EXPECT_TRUE(has(out, "<li>item C</li>"));
}

TEST(MarkdownBlockTest, UnorderedListStar)
{
    std::string out = show("unordered list *",
        "* star item 1\n* star item 2\n");
    EXPECT_TRUE(has(out, "<ul>"));
    EXPECT_TRUE(has(out, "<li>star item 1</li>"));
    EXPECT_TRUE(has(out, "<li>star item 2</li>"));
}

TEST(MarkdownBlockTest, UnorderedListPlus)
{
    std::string out = show("unordered list +",
        "+ plus item\n");
    EXPECT_TRUE(has(out, "<ul>"));
    EXPECT_TRUE(has(out, "<li>plus item</li>"));
}

TEST(MarkdownBlockTest, NotListNoSpace)
{
    // -text 不是列表（- 后无空格）
    std::string out = show("not list", "-not-a-list\n");
    EXPECT_FALSE(has(out, "<ul>"));
    EXPECT_FALSE(has(out, "<li>"));
    EXPECT_TRUE(has(out, "-not-a-list"));
}

// ============================================================================
// 六、有序列表
// ============================================================================

TEST(MarkdownBlockTest, OrderedList)
{
    std::string out = show("ordered list",
        "1. first\n2. second\n3. third\n");
    EXPECT_TRUE(has(out, "<ol>"));
    EXPECT_TRUE(has(out, "</ol>"));
    EXPECT_TRUE(has(out, "<li>first</li>"));
    EXPECT_TRUE(has(out, "<li>second</li>"));
    EXPECT_TRUE(has(out, "<li>third</li>"));
}

TEST(MarkdownBlockTest, OrderedListSingleDigit)
{
    std::string out = show("ordered list single", "1. one\n");
    EXPECT_TRUE(has(out, "<ol>"));
    EXPECT_TRUE(has(out, "<li>one</li>"));
}

TEST(MarkdownBlockTest, NotOrderedListNoSpace)
{
    // 1.text 不是列表（1. 后无空格）
    std::string out = show("not ordered list", "1.not a list\n");
    EXPECT_FALSE(has(out, "<ol>"));
    EXPECT_TRUE(has(out, "1.not a list"));
}

// ============================================================================
// 七、分割线
// ============================================================================

TEST(MarkdownBlockTest, HorizontalRuleDash)
{
    std::string out = show("HR ---",
        "before\n---\nafter\n");
    EXPECT_TRUE(has(out, "<hr>"));
    EXPECT_TRUE(has(out, "before"));
    EXPECT_TRUE(has(out, "after"));
}

TEST(MarkdownBlockTest, HorizontalRuleStar)
{
    std::string out = show("HR ***",
        "before\n***\nafter\n");
    EXPECT_TRUE(has(out, "<hr>"));
}

TEST(MarkdownBlockTest, HorizontalRuleUnderscore)
{
    std::string out = show("HR ___",
        "before\n___\nafter\n");
    EXPECT_TRUE(has(out, "<hr>"));
}

TEST(MarkdownBlockTest, NotHorizontalRule)
{
    // -- 不是分割线（只有 2 个）
    std::string out = show("not HR", "--\n");
    EXPECT_FALSE(has(out, "<hr>"));
    EXPECT_TRUE(has(out, "--"));
}

// ============================================================================
// 八、组合元素
// ============================================================================

TEST(MarkdownBlockTest, HeadingThenParagraph)
{
    std::string out = show("heading then para",
        "## Title\n\nBody text here.\n");
    EXPECT_TRUE(has(out, "<h2>Title</h2>"));
    EXPECT_TRUE(has(out, "<p>Body text here.</p>"));
}

TEST(MarkdownBlockTest, ParagraphThenList)
{
    std::string out = show("para then list",
        "Some text.\n\n- item 1\n- item 2\n");
    EXPECT_TRUE(has(out, "<p>Some text.</p>"));
    EXPECT_TRUE(has(out, "<li>item 1</li>"));
    EXPECT_TRUE(has(out, "<li>item 2</li>"));
}

TEST(MarkdownBlockTest, QuoteThenParagraph)
{
    std::string out = show("quote then para",
        "> quoted\n\nnormal text\n");
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "</blockquote>"));
    EXPECT_TRUE(has(out, "normal text"));
}

TEST(MarkdownBlockTest, ListWithInlineFormatting)
{
    std::string out = show("list with formatting",
        "- **bold item**\n- *italic item*\n- `code item`\n");
    EXPECT_TRUE(has(out, "<strong>bold item</strong>"));
    EXPECT_TRUE(has(out, "<em>italic item</em>"));
    EXPECT_TRUE(has(out, "<code>code item</code>"));
}

TEST(MarkdownBlockTest, AllBlockElements)
{
    std::string out = show("all blocks",
        "# Heading\n\n"
        "Paragraph text.\n\n"
        "- list item\n\n"
        "> blockquote\n\n"
        "---\n\n"
        "```\ncode block\n```\n");
    EXPECT_TRUE(has(out, "<h1>Heading</h1>"));
    EXPECT_TRUE(has(out, "<p>Paragraph text.</p>"));
    EXPECT_TRUE(has(out, "<li>list item</li>"));
    EXPECT_TRUE(has(out, "<blockquote>"));
    EXPECT_TRUE(has(out, "<hr>"));
    EXPECT_TRUE(has(out, "<pre><code>"));
}

// ============================================================================
// 九、流式跨 chunk 解析
// ============================================================================

TEST(MarkdownBlockTest, StreamHeadingSplit)
{
    // 标题跨 chunk 分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("## Ti");
    parser.feed("tle\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream heading split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<h2>Title</h2>"));
}

TEST(MarkdownBlockTest, StreamParagraphSplit)
{
    // 段落跨 chunk 分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("Hello ");
    parser.feed("World\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream paragraph split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<p>Hello World</p>"));
}

TEST(MarkdownBlockTest, StreamBoldSplit)
{
    // 粗体标记跨 chunk 分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("Hello **wor");
    parser.feed("ld**!\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream bold split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<strong>"));
    EXPECT_TRUE(has(out, "world"));
    EXPECT_TRUE(has(out, "</strong>"));
}

TEST(MarkdownBlockTest, StreamCodeFenceSplit)
{
    // 代码围栏跨 chunk 分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("``");
    parser.feed("`\ncode\n```\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream code fence split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<pre><code>"));
    EXPECT_TRUE(has(out, "code"));
    EXPECT_TRUE(has(out, "</code></pre>"));
}

TEST(MarkdownBlockTest, StreamListMarkerSplit)
{
    // 列表标记跨 chunk 分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("- ");
    parser.feed("item\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream list marker split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<li>item</li>"));
}

TEST(MarkdownBlockTest, StreamMultiParagraphSplit)
{
    // 多段落在 chunk 边界分割
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("Para one.\n\nPa");
    parser.feed("ra two.\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[stream multi paragraph split]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_TRUE(has(out, "<p>Para one.</p>"));
    EXPECT_TRUE(has(out, "<p>Para two.</p>"));
}

// ============================================================================
// 十、重置测试
// ============================================================================

TEST(MarkdownBlockTest, ResetClearsState)
{
    MarkdownHTML html;
    MarkdownParser parser(html);
    parser.feed("# Incomplete heading");
    parser.reset();
    parser.feed("Clean text\n");
    parser.finish();
    std::string out = html.output();

    ast_printf("\n[reset clears state]\n");
    ast_printf("[output] %s", out.c_str());
    EXPECT_FALSE(has(out, "<h1>"));
    EXPECT_TRUE(has(out, "Clean text"));
}

GTEST_MAIN()
