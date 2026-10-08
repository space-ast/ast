///
/// @file      AccessLoader.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-08
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AstSim/Access.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class Scenario;

/// @brief 加载 Access 对象
/// @details 加载 .sca 文件中的全部 Access 对象，并挂到场景之下
/// @param filepath .sca 文件路径
/// @param scenario 所属场景
/// @return 错误码
AST_LOADER_API errc_t aLoadAccess(StringView filepath, Scenario& scenario);


/// @brief 加载 Access 对象
/// @details 加载 .sca 文件中的全部 Access 对象，并挂到场景之下
/// @param filepath .sca 文件路径
/// @param scenario 所属场景
/// @param accesses 输出参数，本次加载产生的 Access 对象列表（调用时会被清空）
/// @return 错误码
AST_LOADER_API errc_t aLoadAccess(StringView filepath, Scenario& scenario, std::vector<HAccess>& accesses);

/*! @} */

AST_NAMESPACE_END
