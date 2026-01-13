///
/// @file      String.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      2.12.2025
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
#include <string.h>

AST_NAMESPACE_BEGIN

typedef std::string String;



// 比较两个字符串是否相等（不区分大小写）
#ifdef _WIN32
using ::stricmp;
using ::strnicmp;
A_ALWAYS_INLINE int strcasecmp(const char* str1, const char* str2)
{
	return ::stricmp(str1, str2);
}
A_ALWAYS_INLINE int strncasecmp(const char* str1, const char* str2, size_t n)
{
	return ::strnicmp(str1, str2, n);
}
#else
using ::strcasecmp;
using ::strncasecmp;
A_ALWAYS_INLINE int stricmp(const char* str1, const char* str2)
{
	return ::strcasecmp(str1, str2);
}
A_ALWAYS_INLINE int strnicmp(const char* str1, const char* str2, size_t n)
{
	return ::strncasecmp(str1, str2, n);
}
#endif

AST_UTIL_CAPI bool aEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept;

 
AST_NAMESPACE_END
 
