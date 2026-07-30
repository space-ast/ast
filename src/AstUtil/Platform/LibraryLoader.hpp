///
/// @file      LibraryLoader.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      16.11.2025
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
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN
 
/*!
    @addtogroup Platform
    @{
*/

/// @brief 加载动态库
/// @param filepath 动态库路径
/// @return 
AST_UTIL_CAPI void* aLoadLibrary(const char* filepath);


/// @brief 从动态库中查询函数指针
/// @param lib 动态库
/// @param funcName 函数名称
/// @return 
AST_UTIL_CAPI void* aGetProcAddress(void* lib, const char* funcName);


/// @brief 从动态库中获取函数指针
/// @param filepath 动态库路径
/// @param funcName 函数名称
/// @return 
AST_UTIL_CAPI void* aResolveProcAddress(const char* filepath, const char* funcName);


/// @brief 卸载动态库
/// @param lib 动态库句柄
/// @return 0: 成功
AST_UTIL_CAPI errc_t aFreeLibrary(void* lib);


/// @brief 获取最后的错误信息
/// @return 错误信息字符串
AST_UTIL_CAPI const char* aGetLoadError();


/// @brief 判断文件名是否具有有效的动态库后缀
/// @param fileName 文件名
/// @return 是否为有效的库后缀
AST_UTIL_CAPI bool aIsLibrary(StringView fileName);


/*! @} */

AST_NAMESPACE_END
 
