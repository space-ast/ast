///
/// @file      IO.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      30.11.2025
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
#include <cstdio>       // for std::fopen, std::printf, ...
#include <cstdarg>      // for va_list, va_start, va_end


// 只有当启用命名空间且定义了AST_ENABLE_OVERRIDE_STDLIB时，才覆盖标准库的函数
// 避免在极端情况下的名称冲突，例如using namespace std; using namespace ast; 虽然这不是推荐的做法

#if defined AST_BUILD_LIB && defined AST_ENABLE_NAMESPACE
#   ifndef AST_ENABLE_OVERRIDE_STDLIB  
#       define AST_ENABLE_OVERRIDE_STDLIB
#   endif // !AST_ENABLE_OVERRIDE_STDLIB
#endif



AST_NAMESPACE_BEGIN




namespace posix{

using std::FILE;

#ifdef _WIN32

/// @brief 打开文件
/// @param filepath 文件路径，utf-8编码
/// @param mode 文件打开模式
/// @return 文件指针
AST_UTIL_API
std::FILE* fopen(const char* filepath, const char* mode);

/// @brief 格式化输出
/// @param format 格式化字符串，utf-8编码
/// @param args 可变参数列表
/// @return 输出字符数
AST_UTIL_API
int vprintf(const char* format, va_list args);

/// @brief 格式化输出
/// @param format 格式化字符串，utf-8编码
/// @param ... 可变参数
/// @return 输出字符数
AST_UTIL_API
int printf(const char* format, ...);

/// @brief 重新打开文件
/// @param filepath 文件路径，utf-8编码
/// @param mode 文件打开模式
/// @param stream 文件指针
/// @return 文件指针
AST_UTIL_API
FILE* freopen(const char* filepath, const char* mode, FILE* stream);

#else
using std::fopen;
using std::vprintf;
using std::printf;
using std::freopen;

#endif
}


#ifdef AST_ENABLE_OVERRIDE_STDLIB       // 是否覆盖标准库的函数
using namespace posix;
#endif

AST_UTIL_CAPI int ast_printf(const char* format, ...);


A_ALWAYS_INLINE 
std::FILE* ast_fopen(const char* filepath, const char* mode)
{
    return posix::fopen(filepath, mode);
}

A_ALWAYS_INLINE 
int ast_vprintf(const char* format, va_list args)
{
    return posix::vprintf(format, args);
}

A_ALWAYS_INLINE
FILE* ast_freopen(const char* filepath, const char* mode, FILE* stream)
{
    return posix::freopen(filepath, mode, stream);
}



/// @brief 获取文件当前行号
/// @param file 文件指针
/// @return 当前行号，从1开始计数
AST_UTIL_API int aCurrentLineNumber(std::FILE* file);






AST_NAMESPACE_END
 
