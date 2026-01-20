///
/// @file      IdentifierAPI.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
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
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN

class Identifier;


/// @brief 创建一个标识符
/// @param str 标识符字符串
/// @return 标识符指针
AST_UTIL_API Identifier* aIdentifier(const std::string& str);

/// @brief 创建一个标识符
/// @param str 标识符字符串
/// @return 标识符指针
AST_UTIL_API Identifier* aIdentifier(const StringView& str);


/// @brief 创建一个标识符
/// @param str 标识符字符串
/// @param len 标识符字符串长度
/// @return 标识符指针
AST_UTIL_API Identifier* aIdentifier(const char* str, size_t len);


/// @brief 创建一个标识符
/// @param str 标识符字符串
/// @return 标识符指针
A_ALWAYS_INLINE Identifier* aIdentifier(const char* str)
{
    return aIdentifier(str,  std::char_traits<char>::length(str));
}


AST_NAMESPACE_END
