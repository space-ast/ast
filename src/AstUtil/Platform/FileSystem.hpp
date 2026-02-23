///
/// @file      FileSystem.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      23.11.2025
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
#include "FileSystemSimple.hpp"
#include <string>

// 是否允许使用 std::experimental::filesystem 的c++实验特性，如果存在的话
// #define AST_USE_STD_FILESYSTEM_EXPERIMENTAL

// 判断 c++ 标准库是否有std::filesystem
#ifdef __cpp_lib_filesystem
#   include <filesystem>
#   define _A_STD_FILESYSTEM std::filesystem
#elif defined(__cplusplus) && __cplusplus >= 201703L
#   include <filesystem>
#   define _A_STD_FILESYSTEM std::filesystem
#elif defined AST_USE_STD_FILESYSTEM_EXPERIMENTAL
#   define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#   if defined _MSC_VER
#      include <experimental/filesystem>
#      define _A_STD_FILESYSTEM std::experimental::filesystem
#   elif defined(__has_include) && __has_include(<experimental/filesystem>) && defined AST_USE_STD_FILESYSTEM_EXPERIMENTAL
#      include <experimental/filesystem>
#      define _A_STD_FILESYSTEM std::experimental::filesystem
#   endif
#else
#   undef _A_STD_FILESYSTEM
#endif


AST_NAMESPACE_BEGIN


/*!
    @addtogroup Platform
    @{
*/


// 如果 c++ 标准库没有 filesystem功能，则切换到ast项目对filesystem功能的简化实现
#if defined(_A_STD_FILESYSTEM) && defined(AST_USE_STD_FILESYSTEM)
// #pragma message("use std::filesystem")
namespace filesystem = _A_STD_FILESYSTEM;
#else
// #pragma message("use ast::simple_fs")
namespace filesystem = fs_simple;
#endif

namespace fs = filesystem;


/// @brief 获取当前的AstUtil动态库所在目录
/// @return 
AST_UTIL_API std::string aLibDir();


/// @brief 获取当前的可执行文件所在目录
/// @return 
AST_UTIL_API std::string aExeDir();


/// @brief 获取当前的AstUtil动态库的名称
/// @return 
AST_UTIL_API std::string aLibPath();


/// @brief 获取当前的可执行文件路径
/// @return 
AST_UTIL_API std::string aExePath();



/*! @} */


AST_NAMESPACE_END
 
