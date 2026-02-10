///
/// @file      Environment.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-20
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
#include "Terminal.hpp"
#include "GUI.hpp"
#include <cstdio>

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Platform
    @{
*/

/// @brief 检查是否在持续集成环境中运行
/// @return 如果在CI环境中运行，返回true；否则返回false
AST_UTIL_CAPI bool aIsCI();


/// @brief 检查文件是否为终端
/// @param file 文件指针
/// @return 如果文件为终端，返回true；否则返回false
AST_UTIL_CAPI bool aIsTerminal(FILE* file);


/// @brief 检查文件是否为普通文件
/// @param file 文件指针
/// @return 如果文件为普通文件，返回true；否则返回false
AST_UTIL_CAPI bool aIsFile(FILE* file);


/// @brief 检查标准输入是否为终端
/// @return 如果标准输入为终端，返回true；否则返回false
AST_UTIL_CAPI bool aStdInIsTerminal();


/// @brief 检查标准输入是否为文件
/// @return 如果标准输入为文件，返回true；否则返回false
AST_UTIL_CAPI bool aStdInIsFile();


/// @brief 检查标准输出是否为终端
/// @return 如果标准输出为终端，返回true；否则返回false
AST_UTIL_CAPI bool aStdOutIsTerminal();


/// @brief 检查标准输出是否为文件
/// @return 如果标准输出为文件，返回true；否则返回false
AST_UTIL_CAPI bool aStdOutIsFile();

/// @brief 检查标准错误是否为终端
/// @return 如果标准错误为终端，返回true；否则返回false
AST_UTIL_CAPI bool aStdErrIsTerminal();

/// @brief 检查标准错误是否为文件
/// @return 如果标准错误为文件，返回true；否则返回false
AST_UTIL_CAPI bool aStdErrIsFile();


/*! @} */


AST_NAMESPACE_END
