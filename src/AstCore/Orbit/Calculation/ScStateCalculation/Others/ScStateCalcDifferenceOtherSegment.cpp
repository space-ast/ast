///
/// @file      ScStateCalcDifferenceOtherSegment.cpp
/// @brief
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

#include "ScStateCalcDifferenceOtherSegment.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

void ScStateCalcDifferenceOtherSegment::setCalculation(ScStateCalculation* calculation)
{
    if(!calculation)
        return;
    if(!calculation->getParentScope())
        calculation->setParentScope(this);
    calculation_ = calculation;
}

errc_t ScStateCalcDifferenceOtherSegment::calculate(const SpacecraftState& state, double& result)
{
    auto calculation = this->calculation();             AST_CHECK_NULLPTR(calculation);
    auto otherSegment = this->otherSegment();           AST_CHECK_NULLPTR(otherSegment);
    // 另一轨道段参与计算的状态由 segmentStateToUse 指定
    SpacecraftState* otherState = nullptr;
    switch(segmentStateToUse())
    {
    case ESegmentStateToUse::eInitial:
        otherState = otherSegment->getInputState();
        break;
    case ESegmentStateToUse::eFinal:
        otherState = otherSegment->getOutputState();
        break;
    }
    AST_CHECK_NULLPTR(otherState);
    errc_t rc;
    // 参考计算量在另一轨道段选定状态下的取值
    double otherValue;
    rc = calculation->calculate(*otherState, otherValue);   AST_CHECK_ERRCODE(rc, "calculation failed");
    // 参考计算量在当前状态下的取值
    double currentValue;
    rc = calculation->calculate(state, currentValue);       AST_CHECK_ERRCODE(rc, "calculation failed");
    // 按配置的差值顺序作差
    if(differenceOrderToUse() == ESegmentDifferenceOrder::eCurrentMinusSegment)
        result = currentValue - otherValue;
    else
        result = otherValue - currentValue;
    return rc;
}

Dimension ScStateCalcDifferenceOtherSegment::getDimension() const
{
    if(auto calculation = calculation_.get())
        return calculation->getDimension();
    return Dimension::Unit();
}

AST_NAMESPACE_END
