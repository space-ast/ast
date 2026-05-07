///
/// @file      MissionCommand.hpp
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "Command.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class MissionCommand;
using PMissianCommand = MissionCommand*;
using HMissionCommand = SharedPtr<MissionCommand>;

/// @brief 任务命令（虚基类），用于定义任务序列中的任务命令，例如初始状态、轨道机动、轨道预报等
/// @details 任务命令负责执行任务序列中的任务
/// 参考GMAT的GmatCommand.hpp
class AST_CORE_API MissionCommand: public Command
{
public:
    AST_OBJECT(MissionCommand)

    /// @brief 控制命令
    enum EControlCommand
    {
        eReturn = -999,         ///< 返回到上级
        eStop = -1999,          ///< 停止任务序列
        eBreak = -2999,         ///< 跳出当前任务序列
        eContinue = -3999,      ///< 继续当前任务序列的下一次执行
    };
    MissionCommand() = default;
    ~MissionCommand() override = default;
public:
    /// @brief 执行任务
    /// @return 错误码
    virtual errc_t execute() override= 0;
private:
    
};



/*! @} */

AST_NAMESPACE_END
