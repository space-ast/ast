///
/// @file      Encode.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      28.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
#include <string>
#include <clocale>
 
 
AST_NAMESPACE_BEGIN



/*!
    @addtogroup String
    @{
*/


/// @brief 将ANSI编码的字符串转换为宽字符编码
/// @param ansi 输入ANSI编码的字符串
/// @param wide 输出宽字符编码的字符串
/// @return 错误码
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI errc_t aAnsiToWide(const char* ansi, std::wstring& wide);

/// @brief 将宽字符编码的字符串转换为ANSI编码
/// @param wide 输入宽字符编码的字符串
/// @param ansi 输出ANSI编码的字符串
/// @return 错误码
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI errc_t aWideToAnsi(const wchar_t* wide, std::string& ansi);


/// @brief 将UTF-8编码的字符串转换为宽字符编码
/// @param utf8 输入UTF-8编码的字符串
/// @param wide 输出宽字符编码的字符串
/// @return 错误码
AST_UTIL_API errc_t aUtf8ToWide(const char* utf8, std::wstring& wide);
AST_UTIL_API std::wstring aUtf8ToWide(StringView utf8);

/// @brief 将宽字符编码的字符串转换为UTF-8编码
/// @param wide 输入宽字符编码的字符串
/// @param utf8 输出UTF-8编码的字符串
/// @return 错误码
AST_UTIL_CAPI errc_t aWideToUtf8(const wchar_t* wide, std::string& utf8);




/// @brief 将宽字符编码的字符串转换为C语言运行时LC_CTYPE对应的编码
/// @param wide 输入宽字符编码的字符串
/// @param crt 输出C语言运行时LC_CTYPE对应的编码的字符串
/// @return 错误码
AST_UTIL_CAPI errc_t aWideToCRT(const wchar_t* wide, std::string& crt);

/// @brief 将C语言运行时LC_CTYPE对应的编码的字符串转换为宽字符编码
/// @param crt 输入C语言运行时LC_CTYPE对应的编码的字符串
/// @param wide 输出宽字符编码的字符串
/// @return 错误码
AST_UTIL_CAPI errc_t aCRTToWide(const char* crt, std::wstring& wide);

/// @brief 检查C语言运行时LC_CTYPE对应的编码是否为UTF-8编码
/// @return 是否为UTF-8编码
AST_UTIL_CAPI bool aCRTIsUTF8();


/// @brief 将UTF-8编码的字符串转换为C语言运行时LC_CTYPE对应的编码
/// @param utf8 输入UTF-8编码的字符串
/// @param crt 输出C语言运行时LC_CTYPE对应的编码的字符串
/// @return 错误码
AST_UTIL_CAPI errc_t aUTF8ToCRT(const char* utf8, std::string& crt);

#ifdef _WIN32

/// @brief 获取UTF-8编码的locale
/// @return UTF-8编码的locale
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI _locale_t aUTF8Locale();

/// @brief 获取C语言运行时LC_CTYPE对应的编码的locale
/// @return C语言运行时LC_CTYPE对应的编码的locale
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI _locale_t aAnsiLocale();


#endif

/*! @} */

AST_NAMESPACE_END
 



