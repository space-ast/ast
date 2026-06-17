///
/// @file      testMarkdown.cpp
/// @brief     Markdown 渲染器单元测试
/// @details   每个测试用例同时输出原始 ANSI 到终端，便于目视验证渲染样式。
/// @author    axel
/// @date      2026-06-15
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

#include "AstGlobal.h"
#include "ast/Markdown.hpp"
#include "ast/MarkdownRenderer.hpp"
#include "ast/TestMarkdown.hpp"
#include "ast/IO.hpp"
#include "ast/Test.h"

#include <cstdio>

AST_USING_NAMESPACE

// ============================================================================
// 辅助工具
// ============================================================================

/// @brief 渲染并打印（方便目视确认样式），同时返回 ANSI 字符串供断言
static std::string show(const char* label, const std::string& markdown)
{
	Markdown md;
	std::string out = md(markdown.c_str());

	ast_printf("\n[%s]\n", label);
	ast_printf("[input]\n%s", markdown.c_str());
	if (!markdown.empty() && markdown.back() != '\n') ast_printf("\n");
	ast_printf("[output]\n%s", out.c_str());
	if (!out.empty() && out.back() != '\n') ast_printf("\n");

	return out;
}

/// @brief 检查子串是否存在
static bool has(const std::string& s, const std::string& sub)
{
	return s.find(sub) != std::string::npos;
}

// ============================================================================
// 普通文本
// ============================================================================

TEST(MarkdownTest, PlainText)
{
	std::string out = show("plain text", "Hello World\n");
	EXPECT_TRUE(has(out, "Hello World"));
}

TEST(MarkdownTest, PlainTextNoNewline)
{
	std::string out = show("no trailing newline", "No trailing newline");
	EXPECT_TRUE(has(out, "No trailing newline"));
}

// ============================================================================
// 标题
// ============================================================================

TEST(MarkdownTest, Heading1)
{
	std::string out = show("h1", "# 一级标题\n");
}

TEST(MarkdownTest, Heading2)
{
	std::string out = show("h2", "## 二级标题\n");
}

TEST(MarkdownTest, Heading6)
{
	std::string out = show("h6", "###### 六级标题\n");
}

TEST(MarkdownTest, NotHeading)
{
	std::string out = show("not heading", "#tag text\n");
	EXPECT_TRUE(has(out, "#tag text"));
	EXPECT_FALSE(has(out, "\033[1m\033[34m#"));
}

// ============================================================================
// 粗体
// ============================================================================

TEST(MarkdownTest, Bold)
{
	std::string out = show("bold", "Hello **world**!\n");
	EXPECT_TRUE(has(out, "\033[1m"));
	EXPECT_TRUE(has(out, "world"));
}

TEST(MarkdownTest, BoldOnly)
{
	std::string out = show("bold only", "**important**\n");
	EXPECT_TRUE(has(out, "\033[1m"));
	EXPECT_TRUE(has(out, "important"));
}

// ============================================================================
// 斜体
// ============================================================================

TEST(MarkdownTest, ItalicStar)
{
	std::string out = show("italic *", "This is *emphasized* text\n");
	EXPECT_TRUE(has(out, "\033[3m"));
	EXPECT_TRUE(has(out, "emphasized"));
}

TEST(MarkdownTest, ItalicUnderscore)
{
	std::string out = show("italic _", "This is _emphasized_ text\n");
	EXPECT_TRUE(has(out, "\033[3m"));
	EXPECT_TRUE(has(out, "emphasized"));
}

// ============================================================================
// 粗斜体嵌套
// ============================================================================

TEST(MarkdownTest, BoldItalicNested)
{
	std::string out = show("bold+italic nested", "**bold *and italic* text**\n");
	EXPECT_TRUE(has(out, "bold"));
	EXPECT_TRUE(has(out, "and italic"));
	EXPECT_TRUE(has(out, "\033[1m"));
	EXPECT_TRUE(has(out, "\033[3m"));
}

TEST(MarkdownTest, BoldItalicTriple)
{
	std::string out = show("triple ***", "***bold italic***\n");
	EXPECT_TRUE(has(out, "bold italic"));
}

// ============================================================================
// 行内代码
// ============================================================================

TEST(MarkdownTest, InlineCode)
{
	std::string out = show("inline code", "Use `printf()` function\n");
	EXPECT_TRUE(has(out, "\033[36m"));
	EXPECT_TRUE(has(out, "printf()"));
}

TEST(MarkdownTest, CodePreservesMarkers)
{
	std::string out = show("code preserves markers",
		"`*not italic*` and *italic*\n");
	EXPECT_TRUE(has(out, "*not italic*")) << "Expected literal *not italic* in code span";
	EXPECT_TRUE(has(out, "\033[3mitalic\033[0m"));
}

// ============================================================================
// 删除线
// ============================================================================

TEST(MarkdownTest, Strikethrough)
{
	std::string out = show("strikethrough",
		"This is ~~wrong~~ correct\n");
	EXPECT_TRUE(has(out, "\033[9m"));
	EXPECT_TRUE(has(out, "wrong"));
}

// ============================================================================
// 链接
// ============================================================================

TEST(MarkdownTest, Link)
{
	std::string out = show("link",
		"See [GitHub](https://github.com) for more\n");
	EXPECT_TRUE(has(out, "GitHub"));
	EXPECT_TRUE(has(out, "github.com"));
	EXPECT_TRUE(has(out, "\033[4m"));
	EXPECT_TRUE(has(out, "\033[34m"));
}

TEST(MarkdownTest, LinkWithBoldText)
{
	std::string out = show("link bold",
		"[**bold link**](https://example.com)\n");
	EXPECT_TRUE(has(out, "bold link"));
	EXPECT_TRUE(has(out, "example.com"));
}

TEST(MarkdownTest, NotLinkMissingParen)
{
	std::string out = show("not link",
		"[not a link] text\n");
	EXPECT_TRUE(has(out, "[not a link]"));
}

// ============================================================================
// 无序列表
// ============================================================================

TEST(MarkdownTest, UnorderedListDash)
{
	std::string out = show("list -",
		"- item A\n- item B\n- item C\n");
	EXPECT_TRUE(has(out, "\xe2\x80\xa2")); // "•"
	EXPECT_TRUE(has(out, "item A"));
	EXPECT_TRUE(has(out, "item B"));
}

TEST(MarkdownTest, UnorderedListStar)
{
	std::string out = show("list *",
		"* star item 1\n* star item 2\n");
	EXPECT_TRUE(has(out, "\xe2\x80\xa2"));
}

TEST(MarkdownTest, UnorderedListPlus)
{
	std::string out = show("list +",
		"+ plus item\n");
	EXPECT_TRUE(has(out, "\xe2\x80\xa2"));
}

TEST(MarkdownTest, NotListNoSpace)
{
	std::string out = show("not list",
		"-not-a-list\n");
	EXPECT_FALSE(has(out, "\xe2\x80\xa2"));
	EXPECT_TRUE(has(out, "-not-a-list"));
}

// ============================================================================
// 有序列表
// ============================================================================

TEST(MarkdownTest, OrderedList)
{
	std::string out = show("ordered list",
		"1. first\n2. second\n3. third\n");
	EXPECT_TRUE(has(out, "1. "));
	EXPECT_TRUE(has(out, "2. "));
	EXPECT_TRUE(has(out, "first"));
}

TEST(MarkdownTest, NotOrderedListNoSpace)
{
	std::string out = show("not ordered",
		"1.not a list\n");
	EXPECT_FALSE(has(out, "\033[33m1."));
}

// ============================================================================
// 引用
// ============================================================================

TEST(MarkdownTest, Blockquote)
{
	std::string out = show("blockquote",
		"> quoted text here\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x82")); // "│"
	EXPECT_TRUE(has(out, "quoted text here"));
}

TEST(MarkdownTest, QuoteWithBoldInline)
{
	std::string out = show("quote bold",
		"> quote with **bold** inside\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x82"));
	EXPECT_TRUE(has(out, "\033[1mbold\033[0m"));
}

TEST(MarkdownTest, QuoteWithList)
{
	std::string out = show("quote + list",
		"> - item in quote\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x82"));
}

// ============================================================================
// 分割线
// ============================================================================

TEST(MarkdownTest, HorizontalRuleDash)
{
	std::string out = show("HR ---",
		"before\n---\nafter\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x80")); // "─"
}

TEST(MarkdownTest, HorizontalRuleStar)
{
	std::string out = show("HR ***",
		"before\n***\nafter\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x80"));
}

TEST(MarkdownTest, HorizontalRuleUnderscore)
{
	std::string out = show("HR ___",
		"before\n___\nafter\n");
	EXPECT_TRUE(has(out, "\xe2\x94\x80"));
}

TEST(MarkdownTest, NotHorizontalRule)
{
	std::string out = show("not HR",
		"--\n");
	EXPECT_FALSE(has(out, "\xe2\x94\x80"));
	EXPECT_TRUE(has(out, "--"));
}

// ============================================================================
// 代码块
// ============================================================================

TEST(MarkdownTest, FencedCodeBlock)
{
	std::string out = show("code block ```",
		"```\nint main() {\n    return 0;\n}\n```\n");
	EXPECT_TRUE(has(out, "int main()"));
	EXPECT_TRUE(has(out, "return 0;"));
}

TEST(MarkdownTest, FencedCodeBlockLang)
{
	std::string out = show("code block lang",
		"```cpp\nint x = 42;\n```\n");
	EXPECT_TRUE(has(out, "cpp"));
	EXPECT_TRUE(has(out, "int x = 42;"));
}

TEST(MarkdownTest, CodeBlockPreservesMarkers)
{
	std::string out = show("code preserves markers",
		"```\n**not bold**\n*not italic*\n```\n");
	EXPECT_TRUE(has(out, "**not bold**"));
	EXPECT_TRUE(has(out, "*not italic*"));
}

TEST(MarkdownTest, TildeCodeBlock)
{
	std::string out = show("tilde code block",
		"~~~\ncode here\n~~~\n");
	EXPECT_TRUE(has(out, "code here"));
}

// ============================================================================
// 转义
// ============================================================================

TEST(MarkdownTest, EscapeAsterisk)
{
	std::string out = show("escape *",
		"\\*not italic\\*\n");
	EXPECT_TRUE(has(out, "*not italic*"));
	EXPECT_FALSE(has(out, "\033[3m"));
}

TEST(MarkdownTest, EscapeBacktick)
{
	std::string out = show("escape `",
		"\\`not code\\`\n");
	EXPECT_TRUE(has(out, "`not code`"));
}

TEST(MarkdownTest, EscapeHash)
{
	std::string out = show("escape #",
		"\\# not heading\n");
	EXPECT_TRUE(has(out, "# not heading"));
}

TEST(MarkdownTest, EscapeBackslash)
{
	std::string out = show("escape \\",
		"\\\\ backslash\n");
	EXPECT_TRUE(has(out, "\\ backslash"));
}

// ============================================================================
// 跨 chunk 流式渲染
// ============================================================================

TEST(MarkdownTest, StreamBoldSplitMidWord)
{
	Markdown md;
	std::string o1 = md("Hello **wor");
	std::string o2 = md("ld**!\n");

	ast_printf("\n  [stream bold split]\n");
	ast_printf("  chunk1: %s\n", o1.c_str());
	ast_printf("  chunk2: %s\n", o2.c_str());

	EXPECT_TRUE(has(o1, "\033[1m"));
	EXPECT_TRUE(has(o2, "\033[0m"));
}

TEST(MarkdownTest, StreamMarkerStarSplit)
{
	Markdown md;
	std::string o1 = md("Hello *");
	std::string o2 = md("*bold**\n");

	ast_printf("\n  [stream ** split]\n");
	ast_printf("  chunk1: %s\n", o1.c_str());
	ast_printf("  chunk2: %s\n", o2.c_str());

	// * 在 chunk1 中待定，不应输出字面 *
	// ** 在 chunk2 中确认为粗体开始
	EXPECT_TRUE(has(o1 + o2, "\033[1m"));
}

TEST(MarkdownTest, StreamHeadingSplit)
{
	Markdown md;
	std::string o1 = md("## T");
	std::string o2 = md("itle\n");

	ast_printf("\n  [stream heading split]\n");
	ast_printf("  chunk1: %s\n", o1.c_str());
	ast_printf("  chunk2: %s\n", o2.c_str());

}

// ============================================================================
// Reset — 状态清除
// ============================================================================

TEST(MarkdownTest, ResetClearsIncompleteBold)
{
	Markdown md;
	md("**incomplete bold...");

	ast_printf("\n  [reset clears bold]\n");
	ast_printf("  before reset, output: %s\n",
	       std::string(md("")).c_str());

	md.reset();
	std::string out = md("clean text\n");

	ast_printf("  after reset: %s", out.c_str());

	EXPECT_TRUE(has(out, "clean text"));
	EXPECT_FALSE(has(out, "\033[1m"));
}

TEST(MarkdownTest, ResetClearsHeadingState)
{
	Markdown md;
	md("# Title\n");
	md.reset();

	std::string out = show("reset heading", "normal\n");
	EXPECT_FALSE(has(out, "\033[1m\033[34m"));
}

// ============================================================================
// 边界情况
// ============================================================================

TEST(MarkdownTest, SingleAsteriskLiteral)
{
	// "* " 在行首是合法的无序列表标记，渲染为 bullet
	std::string out = show("single *", "* \n");
	EXPECT_TRUE(has(out, "\xe2\x80\xa2")); // bullet "•"
}

TEST(MarkdownTest, MixedBlockAndInline)
{
	std::string out = show("mixed",
		"## 标题\n"
		"Hello **world** and *italic*.\n"
		"- list with `code`\n"
		"> quote **bold**\n");
	EXPECT_TRUE(has(out, "\033[1mworld"));
	EXPECT_TRUE(has(out, "\033[3mitalic"));
	EXPECT_TRUE(has(out, "\xe2\x80\xa2"));
	EXPECT_TRUE(has(out, "\xe2\x94\x82"));
}

TEST(MarkdownTest, ConsecutiveNewlines)
{
	std::string out = show("consecutive nl",
		"para1\n\npara2\n\npara3\n");
	EXPECT_TRUE(has(out, "para1"));
	EXPECT_TRUE(has(out, "para2"));
	EXPECT_TRUE(has(out, "para3"));
}

// ============================================================================
// 综合渲染示例 — 打印所有 markdown 元素，便于目视检查样式
// ============================================================================

TEST(MarkdownTest, RenderAllElements)
{
	const char* sample = aMarkdownString();

	// 去掉开头的换行
	const char* markdown = sample;

	MarkdownRenderer md;
	//Markdown md;
	std::string out = md(markdown);

	ast_printf("\n");
	ast_printf("================================================================\n");
	ast_printf("  综合 Markdown 渲染示例\n");
	ast_printf("================================================================\n");
	ast_printf("\n[ANSI OUTPUT]\n%s", out.c_str());
	if (!out.empty() && out.back() != '\n')
		ast_printf("\n");
	ast_printf("================================================================\n");
}

GTEST_MAIN()
