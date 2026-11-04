///
/// @file      ScStateCalcDeltaFromMaster.cpp
/// @brief     相对主航天器的差值计算量
/// @details
/// @author    axel
/// @date      2026-05-02
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

#include "ScStateCalcDeltaFromMaster.hpp"
#include "AstCore/State.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

void ScStateCalcDeltaFromMaster::setCalculation(ScStateCalculation* calculation)
{
    if(!calculation)
        return;
    if(!calculation->getParentScope())
        calculation->setParentScope(this);
    calculation_ = calculation;
}

errc_t ScStateCalcDeltaFromMaster::calculate(const SpacecraftState& state, double& result)
{
    auto calculation = this->calculation();         AST_CHECK_NULLPTR(calculation);
    auto reference = this->reference();             AST_CHECK_NULLPTR(reference);

    // 两个状态在同一历元下比较
    TimePoint epoch;
    errc_t rc = state.getStateEpoch(epoch);         AST_CHECK_ERRCODE(rc, "failed to get state epoch");

    // 主航天器在该历元的位置速度（相对其所在坐标系）
    Vector3d pos, vel;
    rc = reference->getPosVel(epoch, pos, vel);     AST_CHECK_ERRCODE(rc, "failed to get reference state");

    Frame* frame = reference->getFrame();           AST_CHECK_NULLPTR(frame);

    // 由主航天器在该历元的轨道状态构造参考状态
    SpacecraftState referenceState;
    referenceState.setOrbitState(State::New(EStateType::eCartesian));
    referenceState.setFrame(frame);
    referenceState.setState(CartState{pos, vel});
    referenceState.setStateEpoch(epoch);

    // 参考计算量在主航天器状态下的取值
    double referenceValue;
    rc = calculation->calculate(referenceState, referenceValue);    AST_CHECK_ERRCODE(rc, "calculation failed");
    // 参考计算量在当前状态下的取值
    double currentValue;
    rc = calculation->calculate(state, currentValue);               AST_CHECK_ERRCODE(rc, "calculation failed");

    result = currentValue - referenceValue;
    return eNoError;
}

Dimension ScStateCalcDeltaFromMaster::getDimension() const
{
    if(auto calculation = calculation_.get())
        return calculation->getDimension();
    return Dimension::Unit();
}

AST_NAMESPACE_END
