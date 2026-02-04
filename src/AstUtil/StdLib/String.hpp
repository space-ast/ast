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
#include "StringView.hpp"
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

/// @brief 比较两个字符串是否相等（不区分大小写）
/// @param str1 第一个字符串
/// @param str2 第二个字符串
/// @return 若相等则返回true，否则返回false
bool aEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept;


/// @brief 移除字符串首尾的ASCII空白字符
/// @param str 输入字符串
/// @return 移除首尾空白字符后的字符串视图
StringView aStripAsciiWhitespace(StringView str) noexcept;


/// @brief 移除字符串尾部的ASCII空白字符
/// @param str 输入字符串
/// @return 移除尾部空白字符后的字符串视图
StringView StripTrailingAsciiWhitespace(StringView str) noexcept;


/// @brief 移除字符串首部的ASCII空白字符
/// @param str 输入字符串
/// @return 移除首部空白字符后的字符串视图
StringView StripLeadingAsciiWhitespace(StringView str) noexcept;





inline
bool aEqualsIgnoreCase(StringView piece1, StringView piece2) noexcept
{
    return piece1.size() == piece2.size() && (strnicmp(piece1.data(), piece2.data(), piece1.size()) == 0);
}

inline
StringView aStripAsciiWhitespace(StringView str) noexcept
{
    const char* begin = str.data();
    const char* end = begin + str.size();
    
    // Skip leading whitespace
    while (begin < end && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }
    
    // Skip trailing whitespace
    while (end > begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    
    return StringView(begin, static_cast<size_t>(end - begin));
}

inline
StringView StripTrailingAsciiWhitespace(StringView str) noexcept
{
    const char* begin = str.data();
    const char* end = begin + str.size();
    
    // Skip trailing whitespace
    while (end > begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        --end;
    }
    
    return StringView(begin, static_cast<size_t>(end - begin));
}

inline
StringView StripLeadingAsciiWhitespace(StringView str) noexcept
{
    const char* begin = str.data();
    const char* end = begin + str.size();
    
    // Skip leading whitespace
    while (begin < end && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }
    
    return StringView(begin, static_cast<size_t>(end - begin));
}



AST_NAMESPACE_END
 
