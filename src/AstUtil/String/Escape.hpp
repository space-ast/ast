///
/// @file      Escape.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
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

#pragma once

#include "AstGlobal.h"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief ANSI 转义序列
/// @details 用于控制终端（命令行、控制台）的显示效果，例如改变文字颜色、背景色、移动光标、清除屏幕等
namespace escape
{
    constexpr char kReset[]       = "\033[0m";        ///< 重置所有属性
    constexpr char kBold[]        = "\033[1m";        ///< 加粗
    constexpr char kItalic[]      = "\033[3m";        ///< 斜体
    constexpr char kUnderline[]   = "\033[4m";        ///< 下划线
    constexpr char kStrike[]      = "\033[9m";        ///< 删除线
    
    constexpr char kBlue[]        = "\033[34m";       ///< 蓝色
    constexpr char kCyan[]        = "\033[36m";       ///< 青色
    constexpr char kDim[]         = "\033[2m";        ///< 暗淡
    constexpr char kYellow[]      = "\033[33m";       ///< 黄色
    constexpr char kBrightWhite[] = "\033[97m";       ///< 亮白色
    constexpr char kBrightCyan[]  = "\033[96m";       ///< 亮青色
}




/// @brief 解析C风格转义序列
/// @details 解析'source'字符串中的C风格转义序列，将它们替换为对应的字符
/// @param source 要解析的源字符串
/// @return 解析后的字符串
AST_UTIL_API std::string aCUnescape(StringView source);



/// @brief     使用C风格转义序列转义字符串
/// @details   转义'src'字符串，使用C风格转义序列(https://en.cppreference.com/w/cpp/language/escape)，
///            其中中常用转义符（如 '\n', '\r', '\t', '\b', '\f', '\v', '\"', '\\'）使用单字母形式
///            将其他不可打印/非空白字节转义为八进制序列(例如"\377")
/// @param      src 要转义的源字符串
/// @return     std::string 转义后的字符串
/// @example
///   std::string s = "foo\rbar\tbaz\010\011\012\013\014\x0d\n";
///   std::string escaped_s = aCEscape(s);
///   EXPECT_EQ(escaped_s, "foo\\rbar\\tbaz\\010\\t\\n\\013\\014\\r\\n");
AST_UTIL_API std::string aCEscape(StringView src);


/// @brief 使用引号引用字符串，并进行 C 风格转义序列转义
/// @details 将源字符串 `src` 封装在双引号中，同时对其中的特殊字符（如双引号、反斜杠、
///          换行符、回车符、制表符等）进行 C 语言风格的反斜杠转义。转义规则与 C/C++
///          字符串字面量一致。
/// @param src 要引用的源字符串（使用 `StringView` 避免拷贝）
/// @return 返回引用并转义后的字符串，首尾带有双引号
/// @exception 不抛出异常
/// @example
///   std::string s = "foo\" bar";
///   std::string quoted = aQuote(s);
///   EXPECT_EQ(quoted, "\"foo\\\" bar\"");
///
///   std::string s2 = "line1\nline2";
///   std::string quoted2 = aQuote(s2);
///   // 输出: "line1\nline2" -> 转义后为 "\"line1\\nline2\""
///   EXPECT_EQ(quoted2, "\"line1\\nline2\"");
AST_UTIL_API std::string aQuote(StringView src);


/// @brief 去除字符串首尾的双引号，并对 C 风格转义序列进行反转义
/// @details 假定输入字符串 `src` 以双引号开头和结尾（即由 aQuote 生成），函数会去掉
///          这对双引号，然后将其内部的转义序列（如 `\"`、`\\`、`\n`、`\r`、`\t`、
///          `\b`、`\f`、`\v`、八进制 `\ooo`、十六进制 `\xHH`）还原为原始字符。
///          如果输入字符串缺少首尾双引号，函数将返回原始字符串
/// @param src 需要反转义的字符串（必须以双引号开头和结尾）
/// @return 反转义后的原始字符串。若引号不存在，则返回原字符串
/// @example
///   std::string quoted = "\"foo\\\" bar\"";
///   std::string unquoted = aUnquote(quoted);
///   // 输出: foo" bar
///   EXPECT_EQ(unquoted, "foo\" bar");
///
///   std::string quoted2 = "\"line1\\nline2\"";
///   std::string unquoted2 = aUnquote(quoted2);
///   EXPECT_EQ(unquoted2, "line1\nline2");
///
///   // 错误格式示例：
///   std::string bad = "foo\" bar";   // 缺少开头引号
///   EXPECT_EQ(aUnquote(bad), "");    // 返回空字符串
AST_UTIL_API std::string aUnquote(StringView src);



/// @brief 解析XML转义序列
/// @details 解析'str'字符串中的XML转义序列，将它们替换为对应的字符
/// @param str 要解析的源字符串
/// @param result 在字符串后添加解析结果
/// @exception 不抛出异常
AST_UTIL_API void aUnescapeXML(StringView& str, std::string& result);


/// @brief 解析XML转义序列
/// @details 解析'src'字符串中的XML转义序列，将它们替换为对应的字符
/// @param src 要解析的源字符串
/// @return 解析后的字符串
/// @exception 不抛出异常
AST_UTIL_API std::string aUnescapeXML(StringView src);

/*! @} */

AST_NAMESPACE_END