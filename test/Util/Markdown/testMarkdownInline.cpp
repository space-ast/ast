///
/// @file      testMarkdownInline.cpp
/// @brief     行内元素解析测试 — 覆盖 MarkdownInlineParser 与 MarkdownParser::parseInline
/// @details   验证斜体、粗体、斜粗体、删除线、行内代码、链接、图片、
///            转义、嵌套、混合分隔符、换行、中文字符等边界情况。
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

#include "ast/MarkdownParser.hpp"
#include "ast/MarkdownANSI.hpp"
#include "ast/MarkdownHTML.hpp"
#include "ast/StringView.hpp"
#include "ast/IO.hpp"
#include "ast/Test.hpp"

AST_USING_NAMESPACE

// ============================================================================
// 辅助函数
// ============================================================================

/// @brief 使用 MarkdownInlineParser 解析行内文本，对比 HTML 输出
void testInlineHTML(StringView chunk, const std::string& expect)
{
    MarkdownHTML sax;
    MarkdownInlineParser parser(sax);
    parser.feed(chunk);
    parser.finish();
    std::string output = sax.output();
    EXPECT_EQ(expect, output);
    ast_printf("chunk : %.*s\n", chunk.size(), chunk.data());
    ast_printf("output: %.*s\n", output.size(), output.data());
}

// ============================================================================
// 一、MarkdownInlineParser — 基础行内元素（下划线变体）
// ============================================================================

TEST(MarkdownInlineTest, Emphasis)
{
    testInlineHTML("*Emphasis*", "<em>Emphasis</em>");
}

TEST(MarkdownInlineTest, Strong)
{
    testInlineHTML("**Strong**", "<strong>Strong</strong>");
}

TEST(MarkdownInlineTest, StrongEmphasis)
{
    testInlineHTML("***StrongEmphasis***", "<strong><em>StrongEmphasis</em></strong>");
}

TEST(MarkdownInlineTest, Delete)
{
    testInlineHTML("~~Delete~~", "<del>Delete</del>");
}

// ---- 下划线变体 ----

TEST(MarkdownInlineTest, EmphasisUnderscore)
{
    testInlineHTML("_Emphasis_", "<em>Emphasis</em>");
}

TEST(MarkdownInlineTest, StrongUnderscore)
{
    testInlineHTML("__Strong__", "<strong>Strong</strong>");
}

TEST(MarkdownInlineTest, StrongEmphasisUnderscore)
{
    testInlineHTML("___StrongEmphasis___", "<strong><em>StrongEmphasis</em></strong>");
}

// ---- 混合分隔符（* 与 _ 等价） ----

TEST(MarkdownInlineTest, MixedDelimiterStarUnderscore)
{
    testInlineHTML("*Mixed_", "<em>Mixed</em>");
}

TEST(MarkdownInlineTest, MixedDelimiterUnderscoreStar)
{
    testInlineHTML("_Mixed*", "<em>Mixed</em>");
}

// ============================================================================
// 二、MarkdownInlineParser — 转义
// ============================================================================

TEST(MarkdownInlineTest, EscapeAsterisk)
{
    testInlineHTML("\\*escaped\\*", "*escaped*");
}

TEST(MarkdownInlineTest, EscapeUnderscore)
{
    testInlineHTML("\\_escaped\\_", "_escaped_");
}

TEST(MarkdownInlineTest, EscapeBackslash)
{
    testInlineHTML("\\\\backslash", "\\backslash");
}

TEST(MarkdownInlineTest, EscapeTilde)
{
    testInlineHTML("\\~\\~no delete\\~\\~", "~~no delete~~");
}

TEST(MarkdownInlineTest, EscapeInText)
{
    testInlineHTML("normal \\* text", "normal * text");
}

// ============================================================================
// 三、MarkdownInlineParser — 嵌套与组合
// ============================================================================

TEST(MarkdownInlineTest, StrongContainsEmphasis)
{
    // **bold *italic* end**
    testInlineHTML("**bold *italic* end**", "<strong>bold <em>italic</em> end</strong>");
}

TEST(MarkdownInlineTest, EmphasisContainsDelete)
{
    testInlineHTML("*italic ~~delete~~ end*", "<em>italic <del>delete</del> end</em>");
}

TEST(MarkdownInlineTest, DeleteContainsStrong)
{
    testInlineHTML("~~delete **bold** end~~", "<del>delete <strong>bold</strong> end</del>");
}

TEST(MarkdownInlineTest, MultiStyleSequence)
{
    testInlineHTML("*Italic* and **Bold** and ~~Delete~~",
                   "<em>Italic</em> and <strong>Bold</strong> and <del>Delete</del>");
}

TEST(MarkdownInlineTest, AdjacentBoldAndItalic)
{
    testInlineHTML("**Bold***Italic*", "<strong>Bold</strong><em>Italic</em>");
}

TEST(MarkdownInlineTest, AdjacentItalicAndBold)
{
    testInlineHTML("*Italic***Bold**", "<em>Italic</em><strong>Bold</strong>");
}

// ============================================================================
// 四、MarkdownInlineParser — 纯文本与边界
// ============================================================================

TEST(MarkdownInlineTest, PlainText)
{
    testInlineHTML("Hello World", "Hello World");
}

TEST(MarkdownInlineTest, EmptyString)
{
    testInlineHTML("", "");
}

TEST(MarkdownInlineTest, SingleAsterisk)
{
    testInlineHTML("*", "*");
}

TEST(MarkdownInlineTest, SingleTilde)
{
    // 孤立的 ~ 留在 pending 中，finish 时输出
    testInlineHTML("~", "~");
}

TEST(MarkdownInlineTest, FourAsterisks)
{
    // **** = 开 StrongEmphasis → 闭 Strong → 空内容
    testInlineHTML("****", "<hr>\n");
}

TEST(MarkdownInlineTest, AsteriskWithSpaces)
{
    testInlineHTML("* content with spaces *",
                   "<em> content with spaces </em>");
}

TEST(MarkdownInlineTest, TripleTilde)
{
    // ~~~ = ~~ + ~ — 删除线打开，孤立 ~ 留在 pending
    testInlineHTML("~~~", "<del>~</del>");
}

// ============================================================================
// 五、MarkdownInlineParser — 换行中断行内样式
// ============================================================================

TEST(MarkdownInlineTest, NewlineBreaksEmphasis)
{
    // *start\nend* — 换行中断斜体，后半段的 * 打开新的斜体但无闭合
    testInlineHTML("*start\nend*", "<em>start</em>\nend");
}

TEST(MarkdownInlineTest, NewlineBreaksStrong)
{
    testInlineHTML("**start\nend**", "<strong>start</strong>\nend");
}

TEST(MarkdownInlineTest, NewlineBreaksDelete)
{
    testInlineHTML("~~start\nend~~", "<del>start</del>\nend");
}

// ============================================================================
// 六、MarkdownInlineParser — Unicode / 中文字符
// ============================================================================

TEST(MarkdownInlineTest, ChineseEmphasis)
{
    testInlineHTML("*中文斜体测试*", "<em>中文斜体测试</em>");
}

TEST(MarkdownInlineTest, ChineseStrong)
{
    testInlineHTML("**中文粗体测试**", "<strong>中文粗体测试</strong>");
}

TEST(MarkdownInlineTest, ChineseDelete)
{
    testInlineHTML("~~中文删除线测试~~", "<del>中文删除线测试</del>");
}

TEST(MarkdownInlineTest, ChineseMixed)
{
    testInlineHTML("*斜体* 中文 **粗体** 混排",
                   "<em>斜体</em> 中文 <strong>粗体</strong> 混排");
}

TEST(MarkdownInlineTest, EmojiText)
{
    testInlineHTML("*🚀 rocket 🚀*", "<em>🚀 rocket 🚀</em>");
}

// ============================================================================
// 七、MarkdownInlineParser — 未闭合样式
// ============================================================================

TEST(MarkdownInlineTest, UnclosedEmphasis)
{
    // *unclosed — 打开了斜体但从未闭合
    testInlineHTML("*unclosed", "<em>unclosed</em>");
}

TEST(MarkdownInlineTest, UnclosedStrong)
{
    testInlineHTML("**unclosed", "<strong>unclosed</strong>");
}

TEST(MarkdownInlineTest, UnclosedDelete)
{
    testInlineHTML("~~unclosed", "<del>unclosed</del>");
}

// ============================================================================
// 八、MarkdownParser::parseInline — 行内代码
// ============================================================================

TEST(MarkdownParserInlineTest, CodeSpan)
{
    testInlineHTML("`inline code`", "<code>inline code</code>");
}

TEST(MarkdownParserInlineTest, CodeSpanEmpty)
{
    testInlineHTML("``", "<code></code>");
}

TEST(MarkdownParserInlineTest, CodeSpanWithSpecialChars)
{
    testInlineHTML("`<div> & </div>`", "<code>&lt;div&gt; &amp; &lt;/div&gt;</code>");
}

TEST(MarkdownParserInlineTest, CodeSpanWithAsterisks)
{
    // * 在代码中不应被解析为斜体
    testInlineHTML("`*not italic*`", "<code>*not italic*</code>");
}

// ============================================================================
// 九、MarkdownParser::parseInline — 链接
// ============================================================================

TEST(MarkdownParserInlineTest, Link)
{
    testInlineHTML("[GitHub](https://github.com)",
                         "<a href=\"https://github.com\">GitHub</a>");
}

TEST(MarkdownParserInlineTest, LinkWithEmptyText)
{
    testInlineHTML("[](url)", "<a href=\"url\"></a>");
}

TEST(MarkdownParserInlineTest, LinkWithEmptyUrl)
{
    testInlineHTML("[text]()", "<a href=\"\">text</a>");
}

TEST(MarkdownParserInlineTest, LinkWithStyledText)
{
    // 链接文本包含斜体
    testInlineHTML("[*italic link*](url)",
                         "<a href=\"url\"><em>italic link</em></a>");
}

TEST(MarkdownParserInlineTest, LinkInText)
{
    testInlineHTML("see [link](url) here",
                         "see <a href=\"url\">link</a> here");
}

// ============================================================================
// 十、MarkdownParser::parseInline — 图片
// ============================================================================

TEST(MarkdownParserInlineTest, Image)
{
    testInlineHTML("![alt text](image.png)",
                         "<img src=\"image.png\" alt=\"alt text\">");
}

TEST(MarkdownParserInlineTest, ImageWithEmptyAlt)
{
    testInlineHTML("![](img.png)",
                         "<img src=\"img.png\" alt=\"\">");
}

TEST(MarkdownParserInlineTest, ImageWithEmptyUrl)
{
    testInlineHTML("![alt]()",
                         "<img src=\"\" alt=\"alt\">");
}

TEST(MarkdownParserInlineTest, ImageBeforeLink)
{
    // 图片标记 ![...](...) 必须先于链接检测
    testInlineHTML("![img](img.png) and [link](url)",
                         "<img src=\"img.png\" alt=\"img\"> and <a href=\"url\">link</a>");
}

// ============================================================================
// 十一、MarkdownParser::parseInline — 组合行内元素
// ============================================================================

TEST(MarkdownParserInlineTest, BoldWithItalic)
{
    testInlineHTML("**bold *and italic***",
                         "<strong>bold <em>and italic</em></strong>");
}

TEST(MarkdownParserInlineTest, DeleteInParser)
{
    testInlineHTML("~~deleted~~",
                         "<del>deleted</del>");
}

TEST(MarkdownParserInlineTest, AllInlineMixed)
{
    testInlineHTML("**Bold** *Italic* `code` [link](url) ~~del~~",
                         "<strong>Bold</strong> <em>Italic</em> <code>code</code> "
                         "<a href=\"url\">link</a> <del>del</del>");
}

TEST(MarkdownParserInlineTest, EscapeInParser)
{
    testInlineHTML("\\*not bold\\*",
                         "*not bold*");
}

TEST(MarkdownParserInlineTest, ChineseCodeSpan)
{
    testInlineHTML("执行 `make install` 命令",
                         "执行 <code>make install</code> 命令");
}

TEST(MarkdownParserInlineTest, ChineseLink)
{
    testInlineHTML("[项目主页](https://space-ast.github.io)",
                         "<a href=\"https://space-ast.github.io\">项目主页</a>");
}

// ============================================================================
// 十二、强调与粗体进阶 — 词内分隔符、交错嵌套、混合定界符
// ============================================================================

TEST(MarkdownInlineTest, IntrawordUnderscore)
{
    // CommonMark: 词内下划线不触发斜体
    testInlineHTML("foo_bar_baz", "foo_bar_baz");
}

TEST(MarkdownInlineTest, IntrawordAsterisk)
{
    // CommonMark: 词内星号可以触发斜体
    testInlineHTML("foo*bar*baz", "foo<em>bar</em>baz");
}

TEST(MarkdownInlineTest, EmphasisContainsStrong)
{
    // 斜体包含粗体（与 StrongContainsEmphasis 相反）
    testInlineHTML("*italic **bold** more*",
                   "<em>italic <strong>bold</strong> more</em>");
}

TEST(MarkdownInlineTest, StrongEmphasisMixedStarUnderscore)
{
    // ** 开粗体，_ 开斜体，_ 关斜体，** 关粗体
    testInlineHTML("**_bold italic_**",
                   "<strong><em>bold italic</em></strong>");
}

TEST(MarkdownInlineTest, StrongEmphasisMixedUnderscoreStar)
{
    // __ 开粗体，* 开斜体，* 关斜体，__ 关粗体
    testInlineHTML("__*bold italic*__",
                   "<strong><em>bold italic</em></strong>");
}

TEST(MarkdownInlineTest, TripleDelimiterStaggeredClose)
{
    // *** 同时开斜体+粗体，* 先关斜体，** 再关粗体
    testInlineHTML("***bold* and**",
                   "<strong><em>bold</em> and</strong>");
}

TEST(MarkdownInlineTest, EmphasisUnderscoreWithLiteralAsterisk)
{
    // 下划线斜体中，星号为普通文本（非混合定界符）
    testInlineHTML("_emphasis * literal_",
                   "<em>emphasis * literal</em>");
}

TEST(MarkdownInlineTest, StrongUnderscoreContainsEmphasisStar)
{
    // 下划线粗体包含星号斜体
    testInlineHTML("__bold *italic* end__",
                   "<strong>bold <em>italic</em> end</strong>");
}

// ============================================================================
// 十三、行内代码进阶 — 反引号内嵌、连续代码、空格剥离
// ============================================================================

TEST(MarkdownParserInlineTest, CodeSpanWithBacktick)
{
    // 用双反引号定界包含单反引号的内容
    testInlineHTML("`` `foo` ``", "<code>`foo`</code>");
}

TEST(MarkdownParserInlineTest, ConsecutiveCodeSpans)
{
    // 同一行中两个相邻的行内代码
    testInlineHTML("`a` `b`", "<code>a</code> <code>b</code>");
}

TEST(MarkdownParserInlineTest, CodeSpanStripLeadingTrailingSpace)
{
    // 首尾各有一个空格时剥离一个
    testInlineHTML("` text `", "<code>text</code>");
}

TEST(MarkdownParserInlineTest, CodeSpanOnlySpaces)
{
    // 只有空格的内容不剥离
    testInlineHTML("`  `", "<code>  </code>");
}

TEST(MarkdownParserInlineTest, CodeSpanEmbeddedInText)
{
    // 行内代码与普通文本混合
    testInlineHTML("press `Ctrl+C` to cancel",
                   "press <code>Ctrl+C</code> to cancel");
}

// ============================================================================
// 十四、链接进阶 — 多链接、链接内含样式、相邻链接
// ============================================================================

TEST(MarkdownParserInlineTest, MultipleLinks)
{
    testInlineHTML("[a](url1) and [b](url2)",
                   "<a href=\"url1\">a</a> and <a href=\"url2\">b</a>");
}

TEST(MarkdownParserInlineTest, LinkWithCodeInText)
{
    // 链接文本中包含行内代码
    testInlineHTML("[`code` link](url)",
                   "<a href=\"url\"><code>code</code> link</a>");
}

TEST(MarkdownParserInlineTest, LinkWithDeleteInText)
{
    // 链接文本中包含删除线
    testInlineHTML("[~~del~~ link](url)",
                   "<a href=\"url\"><del>del</del> link</a>");
}

TEST(MarkdownParserInlineTest, AdjacentLinks)
{
    // 两个链接之间没有空格
    testInlineHTML("[a](url1)[b](url2)",
                   "<a href=\"url1\">a</a><a href=\"url2\">b</a>");
}

TEST(MarkdownParserInlineTest, ThreeConsecutiveLinks)
{
    // 三个链接连续排列
    testInlineHTML("[1](a)[2](b)[3](c)",
                   "<a href=\"a\">1</a><a href=\"b\">2</a><a href=\"c\">3</a>");
}

// ============================================================================
// 十五、图片进阶
// ============================================================================

TEST(MarkdownParserInlineTest, ImageInSentence)
{
    testInlineHTML("see ![img](pic.png) now",
                   "see <img src=\"pic.png\" alt=\"img\"> now");
}

TEST(MarkdownParserInlineTest, ImageWithStyledAlt)
{
    // 图片 Alt 文本包含斜体（Alt 为纯文本，不应含 HTML）
    testInlineHTML("![*alt*](img.png)",
                   "<img src=\"img.png\" alt=\"*alt*\">");
}

TEST(MarkdownParserInlineTest, ImageThenImage)
{
    // 两张图片相邻
    testInlineHTML("![a](a.png)![b](b.png)",
                   "<img src=\"a.png\" alt=\"a\"><img src=\"b.png\" alt=\"b\">");
}

// ============================================================================
// 十六、转义进阶
// ============================================================================

TEST(MarkdownParserInlineTest, EscapeLeftBracket)
{
    // 转义 [ 阻止链接解析
    testInlineHTML("\\[not link](url)", "[not link](url)");
}

TEST(MarkdownInlineTest, EscapeBang)
{
    // 转义 ! 阻止图片解析
    testInlineHTML("\\!\\[text](url)", "![text](url)");
}

TEST(MarkdownInlineTest, DoubleEscape)
{
    // 两个连续的转义：\\ 产生一个 \，后面的字符不被转义
    testInlineHTML("\\\\*text*", "\\<em>text</em>");
}

TEST(MarkdownParserInlineTest, EscapeInCodeSpan)
{
    // 行内代码中反斜杠为字面量
    testInlineHTML("`\\* \\_ \\~`", "<code>\\* \\_ \\~</code>");
}

// ============================================================================
// 十七、边界组合 — 空分隔符、连续内联、纯符号
// ============================================================================

TEST(MarkdownInlineTest, SingleUnderscore)
{
    testInlineHTML("_", "_");
}

TEST(MarkdownInlineTest, TwoUnderscores)
{
    testInlineHTML("__", "<strong></strong>");
}

TEST(MarkdownInlineTest, ThreeUnderscores)
{
    testInlineHTML("___", "<strong><em></em></strong>");
}

TEST(MarkdownInlineTest, FourUnderscores)
{
    testInlineHTML("____", "<hr>\n");
}

TEST(MarkdownParserInlineTest, AllInlineChained)
{
    // 所有行内元素连续排列，无空格
    testInlineHTML("`code`*em***strong**~~del~~[link](url)",
                   "<code>code</code><em>em</em><strong>strong</strong>"
                   "<del>del</del><a href=\"url\">link</a>");
}

TEST(MarkdownParserInlineTest, AllInlineWithSpaces)
{
    // 所有行内元素用空格分隔
    testInlineHTML("`code` *em* **strong** ~~del~~ [link](url)",
                   "<code>code</code> <em>em</em> <strong>strong</strong> "
                   "<del>del</del> <a href=\"url\">link</a>");
}

TEST(MarkdownInlineTest, TildeAfterEmphasis)
{
    // 斜体结束后紧跟删除线
    testInlineHTML("*em*~~del~~", "<em>em</em><del>del</del>");
}

TEST(MarkdownInlineTest, EmphasisAfterDelete)
{
    // 删除线结束后紧跟斜体
    testInlineHTML("~~del~~*em*", "<del>del</del><em>em</em>");
}

TEST(MarkdownInlineTest, BoldItalicBoldSequence)
{
    // 粗体、斜体、粗体交替
    testInlineHTML("**a***b***c**", "<strong>a</strong><em>b</em><strong>c</strong>");
}

TEST(MarkdownInlineTest, OnlySpacesBetweenDelimiters)
{
    // 分隔符之间只有空格
    testInlineHTML("** **", "<strong> </strong>");
}

// ============================================================================
// 十八、Unicode / 特殊字符进阶
// ============================================================================

TEST(MarkdownInlineTest, JapaneseEmphasis)
{
    testInlineHTML("*日本語の強調*", "<em>日本語の強調</em>");
}

TEST(MarkdownInlineTest, KoreanStrong)
{
    testInlineHTML("**한국어 굵게**", "<strong>한국어 굵게</strong>");
}

TEST(MarkdownParserInlineTest, UnicodeLink)
{
    testInlineHTML("[サイト](https://例.com)",
                   "<a href=\"https://例.com\">サイト</a>");
}

TEST(MarkdownParserInlineTest, EmojiInLink)
{
    testInlineHTML("[🚀 launch](url)",
                   "<a href=\"url\">🚀 launch</a>");
}

TEST(MarkdownInlineTest, MathLikeText)
{
    // 包含下划线但不应被解析为斜体的类数学文本
    testInlineHTML("a_b = c_d + e_f", "a_b = c_d + e_f");
}

TEST(MarkdownInlineTest, SnakeCaseVariable)
{
    // 词内下划线不触发样式（常见于变量名）
    testInlineHTML("my_variable_name", "my_variable_name");
}

// ============================================================================
// 十九、嵌套组合补全 — 样式中包含链接、同类型嵌套
// ============================================================================

TEST(MarkdownInlineTest, DeleteContainsEmphasis)
{
    // 删除线包含斜体（与 EmphasisContainsDelete 相反）
    testInlineHTML("~~delete *em* end~~",
                   "<del>delete <em>em</em> end</del>");
}

TEST(MarkdownInlineTest, StrongContainsDelete)
{
    // 粗体包含删除线
    testInlineHTML("**bold ~~del~~ end**",
                   "<strong>bold <del>del</del> end</strong>");
}

TEST(MarkdownParserInlineTest, EmphasisContainsLink)
{
    // 斜体包含链接
    testInlineHTML("*[link](url)*",
                   "<em><a href=\"url\">link</a></em>");
}

TEST(MarkdownParserInlineTest, StrongContainsLink)
{
    // 粗体包含链接
    testInlineHTML("**[link](url)**",
                   "<strong><a href=\"url\">link</a></strong>");
}

TEST(MarkdownParserInlineTest, DeleteContainsLink)
{
    // 删除线包含链接
    testInlineHTML("~~[link](url)~~",
                   "<del><a href=\"url\">link</a></del>");
}

TEST(MarkdownParserInlineTest, LinkWithStrongInText)
{
    // 链接文本包含粗体
    testInlineHTML("[**bold** link](url)",
                   "<a href=\"url\"><strong>bold</strong> link</a>");
}

TEST(MarkdownParserInlineTest, LinkWithEmAndDelInText)
{
    // 链接文本包含斜体和删除线
    testInlineHTML("[*em* ~~del~~ text](url)",
                   "<a href=\"url\"><em>em</em> <del>del</del> text</a>");
}

TEST(MarkdownInlineTest, NestedSameTypeEmphasis)
{
    // 同类型斜体嵌套：内层先匹配
    testInlineHTML("*foo *bar* baz*",
                   "<em>foo <em>bar</em> baz</em>");
}

// ============================================================================
// 二十、分隔符与标点相邻 — CommonMark 左右侧翼规则
// ============================================================================

TEST(MarkdownInlineTest, PunctuationAdjacentEmphasis)
{
    // 分隔符紧邻圆括号
    testInlineHTML("*(foo)*", "<em>(foo)</em>");
}

TEST(MarkdownInlineTest, EmphasisWithTrailingComma)
{
    // 斜体后紧跟逗号
    testInlineHTML("*em*, and more", "<em>em</em>, and more");
}

TEST(MarkdownInlineTest, EmphasisWithLeadingColon)
{
    // 冒号后紧跟斜体
    testInlineHTML("see: *em* here", "see: <em>em</em> here");
}

TEST(MarkdownInlineTest, StarsSurroundedBySpaces)
{
    // ** 两侧都是空格 → 不是分隔符，原样输出
    testInlineHTML("a ** b", "a ** b");
}

TEST(MarkdownInlineTest, UnderscoresSurroundedBySpaces)
{
    // __ 两侧都是空格 → 不是分隔符，原样输出
    testInlineHTML("a __ b", "a __ b");
}

// ============================================================================
// 二十一、链接 / 图片进阶 — URL 特殊字符、转义括号
// ============================================================================

TEST(MarkdownParserInlineTest, LinkURLWithQueryAndFragment)
{
    testInlineHTML("[text](https://example.com/path?a=1&b=2#frag)",
                   "<a href=\"https://example.com/path?a=1&b=2#frag\">text</a>");
}

TEST(MarkdownParserInlineTest, LinkWithEscapedBracketInText)
{
    // 链接文本中 \\] 表示转义的 ]，不结束链接文本
    testInlineHTML("[text\\]more](url)",
                   "<a href=\"url\">text]more</a>");
}

TEST(MarkdownParserInlineTest, BangWithoutBracket)
{
    // ! 后没有 [ 不触发图片
    testInlineHTML("!text", "!text");
}

TEST(MarkdownParserInlineTest, ImageAltWithSpaces)
{
    // Alt 文本只有一个空格
    testInlineHTML("![ ](img.png)",
                   "<img src=\"img.png\" alt=\" \">");
}

// ============================================================================
// 二十二、空白符控制 — 制表符、连续换行
// ============================================================================

TEST(MarkdownInlineTest, TabInEmphasis)
{
    // 斜体中的制表符应保留
    testInlineHTML("*\ta\t*", "<em>\ta\t</em>");
}

TEST(MarkdownInlineTest, TabInDelete)
{
    // 删除线中的制表符应保留
    testInlineHTML("~~\tdel\t~~", "<del>\tdel\t</del>");
}

TEST(MarkdownInlineTest, MultipleNewlinesBetweenElements)
{
    // 两个内联元素之间有两个换行（空行）
    testInlineHTML("*a*\n\n*b*", "<em>a</em>\n\n<em>b</em>");
}

// ============================================================================
// 二十三、更多边界符号组合
// ============================================================================

TEST(MarkdownInlineTest, FourTildes)
{
    // ~~~~ = ~~ (开删除线) + ~~ (关删除线) → 空内容
    testInlineHTML("~~~~", "<del></del>");
}

TEST(MarkdownInlineTest, FiveTildes)
{
    // ~~~~~ = ~~ (开) + ~~ (关, 空内容) + ~ (孤立)
    testInlineHTML("~~~~~", "<del></del>~");
}

TEST(MarkdownInlineTest, SixAsterisks)
{
    // ****** — 六个星号
    testInlineHTML("******", "<hr>\n");
}

TEST(MarkdownInlineTest, EmphasisUnderscoreWithAsteriskContent)
{
    // _ 斜体中 * 开头但无匹配 _ 的 * 视为普通文本
    testInlineHTML("_foo *bar_", "<em>foo *bar</em>");
}

TEST(MarkdownInlineTest, AsteriskEmphasisWithUnderscoreContent)
{
    // * 斜体中 _ 开头但无匹配 * 的 _ 视为普通文本
    testInlineHTML("*foo _bar*", "<em>foo _bar</em>");
}

TEST(MarkdownParserInlineTest, CodeSpanFollowedByLink)
{
    // 行内代码紧跟链接
    testInlineHTML("`code`[link](url)",
                   "<code>code</code><a href=\"url\">link</a>");
}

TEST(MarkdownParserInlineTest, ImageFollowedByCodeSpan)
{
    // 图片紧跟前导空格和行内代码
    testInlineHTML("![a](a.png) `code`",
                   "<img src=\"a.png\" alt=\"a\"> <code>code</code>");
}


GTEST_MAIN()
