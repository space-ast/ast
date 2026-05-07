///
/// @file      MotionBasic.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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
#include "AstSim/MotionProfile.hpp"
#include "AstSim/MotionWithIntervalStep.hpp"
#include "AstCore/State.hpp"
#include "AstCore/EventInterval.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class MotionOrbitDynamics;
using PMotionOrbitDynamics = MotionOrbitDynamics*;
// using HMotionOrbitDynamics = SharedPtr<MotionOrbitDynamics>;
using MotionBasic = MotionOrbitDynamics; // 兼容处理

/// @brief 轨道动力学基础运动模型数据结构
class AST_SIM_API MotionOrbitDynamics: public MotionWithIntervalStep
{
public:
    AST_OBJECT(MotionOrbitDynamics)
    AST_PROPERT(InitialState)
    AST_PROPERT(PropagationFrame)
    MotionOrbitDynamics() = default;
    ~MotionOrbitDynamics() = default;


    /// @brief 获取状态类型
    /// @return 状态类型
    EStateType getStateType() const;

    /// @brief 设置状态类型
    /// @param type 状态类型
    void setStateType(EStateType type);
protected:
    struct PropagationParams
    {
        TimePoint    epoch_;                      ///< 预报时间点
        CartState    stateInPropagationFrame_;    ///< 预报坐标系下的初始状态
        Frame*       propagationFrame_;           ///< 预报坐标系指针
    };

    errc_t getPropagationParams(PropagationParams& params) const;

    errc_t discreteInterval(const TimePoint& epoch, double stepSize, std::vector<double>& times) const;
PROPERTIES:
    State* getInitialState() const { return initialState_.get(); }
    void setInitialState(State* state) { initialState_ = state; }
    Frame* getPropagationFrame() const { return propagationFrame_.get(); }
    void setPropagationFrame(Frame* frame) { propagationFrame_ = frame; }
protected:
    SharedPtr<State>            initialState_;          ///< 初始状态
    SharedPtr<Frame>            propagationFrame_;      ///< 预报坐标系
};

/*! @} */

AST_NAMESPACE_END
