///
/// @file      MotionOrbitDynamics.cpp
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

#include "MotionOrbitDynamics.hpp"
#include "AstCore/EventInterval.hpp"
#include "AstCore/EventIntervalExplicit.hpp"


AST_NAMESPACE_BEGIN

EStateType MotionOrbitDynamics::getStateType() const
{
    if(initialState_)
    {
        return initialState_->getStateType();
    }
    return EStateType::eUnknown;
}

void MotionOrbitDynamics::setStateType(EStateType type)
{
    if(initialState_)
    {
        if(initialState_->getStateType() == type)
        {
            return;
        }
        initialState_ = State::MakeShared(*initialState_, type);
    }else{
        initialState_ = State::MakeShared(type);
    }
}

errc_t MotionOrbitDynamics::getPropagationParams(PropagationParams &params) const
{
    errc_t rc;
    // rc = this->getInterval(params.interval_);   AST_CHECK_ERRCODE(rc, "failed to get interval");
    params.propagationFrame_ = this->getPropagationFrame();   AST_CHECK_NULLPTR(params.propagationFrame_);
    auto initialState = this->getInitialState();   AST_CHECK_NULLPTR(initialState);
    rc = initialState->getStateEpoch(params.epoch_);   AST_CHECK_ERRCODE(rc, "failed to get initial state epoch");
    rc = initialState->getStateIn(params.propagationFrame_, params.stateInPropagationFrame_);   AST_CHECK_ERRCODE(rc, "failed to get initial state");
    return eNoError;
}

errc_t MotionOrbitDynamics::discretizeInterval(const TimePoint &epoch, double stepSize, std::vector<double> &times) const
{
    errc_t rc;
    TimeInterval interval{};
    rc = this->getInterval(interval);   AST_CHECK_ERRCODE(rc, "failed to get interval");
    rc = interval.discretize(epoch, stepSize, times);   AST_CHECK_ERRCODE(rc, "failed to discretize interval");
    return rc;
}

AST_NAMESPACE_END


