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
/// @todo 实现Linux平台上的ANSI到宽字符编码转换
AST_UTIL_CAPI err_t aAnsiToWide(const char* ansi, std::wstring& wide);


/// @brief 将UTF-8编码的字符串转换为宽字符编码
/// @param utf8 输入UTF-8编码的字符串
/// @param wide 输出宽字符编码的字符串
/// @return 错误码
/// @warning 仅在Windows平台上实现
/// @todo 实现Linux平台上的UTF-8到宽字符编码转换
AST_UTIL_CAPI err_t aUtf8ToWide(const char* utf8, std::wstring& wide);

/// @brief 将宽字符编码的字符串转换为ANSI编码
/// @param wide 输入宽字符编码的字符串
/// @param ansi 输出ANSI编码的字符串
/// @return 错误码
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI err_t aWideToAnsi(const wchar_t* wide, std::string& ansi);

/// @brief 将宽字符编码的字符串转换为UTF-8编码
/// @param wide 输入宽字符编码的字符串
/// @param utf8 输出UTF-8编码的字符串
/// @return 错误码
/// @warning 仅在Windows平台上实现
AST_UTIL_CAPI err_t aWideToUtf8(const wchar_t* wide, std::string& utf8);



/*! @} */

AST_NAMESPACE_END
 



