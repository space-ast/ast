///
/// @file      MissionCommandLoader.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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
#include "InitialStateLoader.hpp"
#include "ManeuverLoader.hpp"
#include "PropagateLoader.hpp"
#include "SequenceLoader.hpp"
#include "TargeterSequenceLoader.hpp"
#include "ReturnLoader.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MissionCommand;

/// @brief 从值加载任务命令
/// @param value 值
/// @param missionCommand 任务命令对象
AST_LOADER_API errc_t aLoadMissionCommand(const Value& value, SharedPtr<MissionCommand>& missionCommand);

/// @brief 从文件加载任务命令
/// @param filepath 文件路径
/// @param missionCommand 任务命令对象
AST_LOADER_API errc_t aLoadMissionCommand(StringView filepath, SharedPtr<MissionCommand>& missionCommand);

/*! @} */

AST_NAMESPACE_END
