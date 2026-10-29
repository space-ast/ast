///
/// @file      testEscape.cpp
/// @brief     C 风格转义 / 反转义与 XML 实体解析测试
/// @details   覆盖 aCEscape、aCUnescape、aQuote/aUnquote 的边界分支，
///            以及 aUnescapeXML 的命名实体、十进制/十六进制数字实体和非法实体。
/// @author    AistSpace
/// @date      2026-10-29
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

#include "ast/Test.hpp"
#include "ast/Escape.hpp"

#include <string>

AST_USING_NAMESPACE

// ============================ aCEscape ============================

TEST(CEscapeTest, PrintablePassThrough)
{
    EXPECT_EQ(aCEscape("abc123"), "abc123");
    EXPECT_EQ(aCEscape("!@#$%^&*()"), "!@#$%^&*()");
    EXPECT_EQ(aCEscape(""), "");
}

TEST(CEscapeTest, SimpleEscapes)
{
    EXPECT_EQ(aCEscape("\\"), "\\\\");
    EXPECT_EQ(aCEscape("\""), "\\\"");
    EXPECT_EQ(aCEscape("'"), "\\'");
    EXPECT_EQ(aCEscape("\a"), "\\a");
    EXPECT_EQ(aCEscape("\b"), "\\b");
    EXPECT_EQ(aCEscape("\f"), "\\f");
    EXPECT_EQ(aCEscape("\n"), "\\n");
    EXPECT_EQ(aCEscape("\r"), "\\r");
    EXPECT_EQ(aCEscape("\t"), "\\t");
    EXPECT_EQ(aCEscape("\v"), "\\v");
}

TEST(CEscapeTest, HeaderDocExample)
{
    // 头文件示例中的字符串，转义结果以实际实现为准（\b/\v/\f 用单字母形式）
    std::string s = "foo\rbar\tbaz\010\011\012\013\014\x0d\n";
    EXPECT_EQ(aCEscape(s), "foo\\rbar\\tbaz\\b\\t\\n\\v\\f\\r\\n");
}

TEST(CEscapeTest, WhitespaceIsEscapedAsOctal)
{
    // 空格属于 ASCII 空白，虽然可打印，但仍按八进制转义
    EXPECT_EQ(aCEscape("a b"), "a\\040b");
    EXPECT_EQ(aCEscape("a\tb"), "a\\tb");
}

TEST(CEscapeTest, NonPrintableBytesUseOctal)
{
    EXPECT_EQ(aCEscape(std::string("\x7F", 1)), "\\177");
    EXPECT_EQ(aCEscape(std::string("\xFF", 1)), "\\377");
    EXPECT_EQ(aCEscape(std::string("\x01", 1)), "\\001");
}

// ============================ aCUnescape ============================

TEST(CUnescapeTest, SimpleEscapes)
{
    EXPECT_EQ(aCUnescape("\\\\"), "\\");
    EXPECT_EQ(aCUnescape("\\\""), "\"");
    EXPECT_EQ(aCUnescape("\\'"), "'");
    EXPECT_EQ(aCUnescape("\\a"), "\a");
    EXPECT_EQ(aCUnescape("\\b"), "\b");
    EXPECT_EQ(aCUnescape("\\f"), "\f");
    EXPECT_EQ(aCUnescape("\\n"), "\n");
    EXPECT_EQ(aCUnescape("\\r"), "\r");
    EXPECT_EQ(aCUnescape("\\t"), "\t");
    EXPECT_EQ(aCUnescape("\\v"), "\v");
}

TEST(CUnescapeTest, SimpleQuestionMark)
{
    // C 标准里 \? 是合法的简单转义（用于三元运算符的历史兼容）
    EXPECT_EQ(aCUnescape("a\\?b"), "a?b");
}

TEST(CUnescapeTest, HexEscapes)
{
    EXPECT_EQ(aCUnescape("\\x41"), "A");
    EXPECT_EQ(aCUnescape("\\x4"), std::string("\x04", 1));
    EXPECT_EQ(aCUnescape("\\xFF"), std::string("\xFF", 1));
    // 十六进制转义位数不限，值取低 8 位（与 GCC/Clang 截断行为一致）
    EXPECT_EQ(aCUnescape("\\x4142"), "B");
    EXPECT_EQ(aCUnescape("\\x000041"), "A");
    EXPECT_EQ(aCUnescape("\\x1FF"), std::string("\xFF", 1));
    EXPECT_EQ(aCUnescape("\\x41bc"), std::string("\xBC", 1));
    // 遇到非十六进制字符即停止
    EXPECT_EQ(aCUnescape("\\xAG"), std::string("\nG", 2));
}

TEST(CUnescapeTest, InvalidHexKeepsLiteralBackslashX)
{
    EXPECT_EQ(aCUnescape("\\x"), "\\x");
    EXPECT_EQ(aCUnescape("\\xZZ"), "\\xZZ");
}

TEST(CUnescapeTest, OctalEscapes)
{
    EXPECT_EQ(aCUnescape("\\101"), "A");
    EXPECT_EQ(aCUnescape("\\0"), std::string("\0", 1));
    EXPECT_EQ(aCUnescape("a\\0b"), std::string("a\0b", 3));
    EXPECT_EQ(aCUnescape("\\12"), "\n");
    EXPECT_EQ(aCUnescape("\\377"), std::string("\xFF", 1));
    // 最多三位八进制数字
    EXPECT_EQ(aCUnescape("\\1011"), "A1");
}

TEST(CUnescapeTest, UnknownEscapeIsKept)
{
    // C 标准对未知转义未定义；本函数选择原样保留，避免静默改写路径/配置
    EXPECT_EQ(aCUnescape("\\q"), "\\q");
    EXPECT_EQ(aCUnescape("\\8"), "\\8");
    EXPECT_EQ(aCUnescape("a\\zb"), "a\\zb");
}

TEST(CUnescapeTest, TrailingBackslashAndPlainText)
{
    EXPECT_EQ(aCUnescape("abc\\"), "abc\\");
    EXPECT_EQ(aCUnescape("plain text"), "plain text");
    EXPECT_EQ(aCUnescape(""), "");
}

TEST(CUnescapeTest, CombinedSequences)
{
    EXPECT_EQ(aCUnescape("a\\tb\\nc"), "a\tb\nc");
    EXPECT_EQ(aCUnescape("\\x41\\102\\103"), "ABC");
}

// ============================ aQuote / aUnquote ============================

TEST(QuoteTest, QuoteWrapsAndEscapes)
{
    EXPECT_EQ(aQuote("abc"), "\"abc\"");
    EXPECT_EQ(aQuote(""), "\"\"");
    EXPECT_EQ(aQuote("a\"b"), "\"a\\\"b\"");
    EXPECT_EQ(aQuote("a\\b"), "\"a\\\\b\"");
    EXPECT_EQ(aQuote("line1\nline2"), "\"line1\\nline2\"");
    EXPECT_EQ(aQuote("a\tb\rc\bd\fe\vf"), "\"a\\tb\\rc\\bd\\fe\\vf\"");
}

TEST(QuoteTest, RoundTrip)
{
    const char* samples[] = {
        "",
        "plain",
        "a \"quoted\" word",
        "back\\slash",
        "multi\nline\ttext\vwith\fcontrols\b",
        "中文 UTF-8",
    };
    for (const char* s : samples)
    {
        EXPECT_EQ(aUnquote(aQuote(s)), s);
    }
}

TEST(QuoteTest, UnquoteInvalidInputReturnsOriginal)
{
    EXPECT_EQ(aUnquote("foo"), "foo");
    EXPECT_EQ(aUnquote("\""), "\"");
    EXPECT_EQ(aUnquote(""), "");
    // 只有一端有引号时不处理
    EXPECT_EQ(aUnquote("\"abc"), "\"abc");
    EXPECT_EQ(aUnquote("abc\""), "abc\"");
}

TEST(QuoteTest, UnquoteEmptyQuoted)
{
    EXPECT_EQ(aUnquote("\"\""), "");
}

// ============================ aUnescapeXML ============================

TEST(XMLUnescapeTest, NamedEntities)
{
    EXPECT_EQ(aUnescapeXML("&lt;tag&gt;"), "<tag>");
    EXPECT_EQ(aUnescapeXML("a &amp; b"), "a & b");
    EXPECT_EQ(aUnescapeXML("&apos;&quot;"), "'\"");
}

TEST(XMLUnescapeTest, DecimalAndHexEntities)
{
    EXPECT_EQ(aUnescapeXML("&#65;&#x42;"), "AB");
    EXPECT_EQ(aUnescapeXML("&#X41;"), "A");
    // U+4E2D（中）的 UTF-8 编码为 E4 B8 AD
    EXPECT_EQ(aUnescapeXML("&#x4E2D;"), std::string("\xE4\xB8\xAD", 3));
    // U+00E9（é）的 UTF-8 编码为 C3 A9
    EXPECT_EQ(aUnescapeXML("&#233;"), std::string("\xC3\xA9", 2));
}

TEST(XMLUnescapeTest, InvalidEntitiesAreKept)
{
    // 超出 Unicode 范围
    EXPECT_EQ(aUnescapeXML("&#1114112;"), "&#1114112;");
    EXPECT_EQ(aUnescapeXML("&#x110000;"), "&#x110000;");
    // 非字符码点
    EXPECT_EQ(aUnescapeXML("&#xFFFE;"), "&#xFFFE;");
    // 未识别的命名实体
    EXPECT_EQ(aUnescapeXML("&unknown;"), "&unknown;");
    // 缺少分号
    EXPECT_EQ(aUnescapeXML("&amp"), "&amp");
    // 空的实体
    EXPECT_EQ(aUnescapeXML("&;"), "&;");
}

TEST(XMLUnescapeTest, PlainTextAndMixedContent)
{
    EXPECT_EQ(aUnescapeXML("plain text"), "plain text");
    EXPECT_EQ(aUnescapeXML(""), "");
    EXPECT_EQ(aUnescapeXML("a &lt; b &amp;&amp; c &gt; d"), "a < b && c > d");
}

TEST(XMLUnescapeTest, AppendsToExistingResult)
{
    std::string result = "prefix:";
    StringView sv = "&lt;x&gt;";
    aUnescapeXML(sv, result);
    EXPECT_EQ(result, "prefix:<x>");
}

GTEST_MAIN()
