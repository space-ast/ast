///
/// @file      StringPosix.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-10
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
#include <string>
#include <cstring>
#include <cctype>   // for std::isspace

AST_NAMESPACE_BEGIN

typedef std::string String;


namespace posix{

#ifdef _WIN32
/// @brief 比较两个字符串是否相等（不区分大小写）
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @return 若相等则返回0，否则返回非0值
A_ALWAYS_INLINE int strcasecmp(const char* str1, const char* str2)
{
	return ::_stricmp(str1, str2);
}

/// @brief 比较两个字符串是否相等（不区分大小写）
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @param n 比较的字符数
/// @return 若相等则返回0，否则返回非0值
A_ALWAYS_INLINE int strncasecmp(const char* str1, const char* str2, size_t n)
{
	return ::_strnicmp(str1, str2, n);
}

#else
using ::strcasecmp;
using ::strncasecmp;
#endif

}

// 比较两个字符串是否相等（不区分大小写）
#ifdef _WIN32
using ::stricmp;
using ::strnicmp;
#else

/// @brief 比较两个字符串是否相等（不区分大小写）
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @return 若相等则返回0，否则返回非0值
A_ALWAYS_INLINE int stricmp(const char* str1, const char* str2)
{
	return ::strcasecmp(str1, str2);
}

/// @brief 比较两个字符串是否相等（不区分大小写）
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @param n 比较的字符数
/// @return 若相等则返回0，否则返回非0值
A_ALWAYS_INLINE int strnicmp(const char* str1, const char* str2, size_t n)
{
	return ::strncasecmp(str1, str2, n);
}

#endif

using posix::strcasecmp;
using posix::strncasecmp;

AST_NAMESPACE_END
