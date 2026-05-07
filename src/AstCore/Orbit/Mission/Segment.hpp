///
/// @file      Segment.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-19
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
#include "MissionCommand.hpp"
#include "AstCore/SpacecraftState.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

// class ControlFlowEscape {};
// class ReturnEscape : public ControlFlowEscape{};


/// @brief 轨道段（虚基类），用于描述任务序列中的轨道段，例如初始状态段、轨道机动段、轨道预报段等
/// @details 轨道段描述任务序列中的轨道任务段
/// 参考了GMAT-Architectural-Specification.pdf第26章的Mission Control Sequence Commands
/// 参考了STK的轨道段MCSSegment
class AST_CORE_API Segment: public MissionCommand
{
public:
    AST_OBJECT(Segment)
    AST_PROPERT(InputState)
    AST_PROPERT(OutputState)
    Segment();
    ~Segment() override = default;
public:
    /// @brief 执行任务
    /// @return 错误码
    virtual errc_t execute() override = 0;

PROPERTIES:
    /// @brief 设置输入状态
    /// @param inputState 输入状态
    void setInputState(SpacecraftState* inputState){inputState_ = inputState;}

    /// @brief 获取输入状态
    /// @return 输入状态
    SpacecraftState* getInputState() const{return inputState_.get();}

    /// @brief 获取输出状态
    /// @return 输出状态
    SpacecraftState* getOutputState() const{return outputState_.get();}
private:
    WeakPtr<SpacecraftState> inputState_;          ///< 输入状态
    SharedPtr<SpacecraftState> outputState_;       ///< 输出状态
};




/*! @} */

AST_NAMESPACE_END
