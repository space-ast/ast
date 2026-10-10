///
/// @file      ScenarioLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class Scenario;

/// @brief 加载场景
/// @details 从文件加载场景
/// @param filepath 文件路径
/// @param scenario 场景引用
/// @return 错误码
AST_LOADER_CAPI errc_t aLoadScenario(StringView filepath, Scenario& scenario);


/// @brief 查找场景文件
/// @details dirpath 场景目录路径
/// @param scenarioPath 场景文件路径
/// @return 错误码
AST_LOADER_CAPI errc_t aFindScenarioFile(StringView dirpath, std::string& scenarioPath);

/*! @} */

AST_NAMESPACE_END
