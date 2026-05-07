///
/// @file      ScStateCalcDifference.cpp
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

#include "ScStateCalcDifference.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstCore/Segment.hpp"

AST_NAMESPACE_BEGIN

void ScStateCalcDifference::setCalculation(ScStateCalculation* calculation)
{
    if(!calculation)
        return;
    if(!calculation->getParentScope())
        calculation->setParentScope(this);
    calculation_ = calculation;
}

Segment* ScStateCalcDifference::getSegment()
{
    if(auto segment = segment_.get())
        return segment;
    Segment* segment = (Segment*)aGetAncestorScope(this, Segment::StaticType());
    segment_ = segment;
    return segment;
}

errc_t ScStateCalcDifference::calculate(const SpacecraftState& state, double& result)
{
    auto segment = getSegment();                    AST_CHECK_NULLPTR(segment);
    auto calculation = this->calculation();         AST_CHECK_NULLPTR(calculation);
    auto inputState = segment->getInputState();     AST_CHECK_NULLPTR(inputState);
    errc_t rc;
    double initialValue;
    rc = calculation->calculate(*inputState, initialValue);  AST_CHECK_ERRCODE(rc, "calculation failed");
    double currentValue;
    rc = calculation->calculate(state, currentValue);   AST_CHECK_ERRCODE(rc, "calculation failed");
    if(differenceOrderToUse() == EDifferenceOrderToUse::eCurrentMinusInitial)
        result = currentValue - initialValue;
    else
        result = initialValue - currentValue;
    return rc;
}

bool ScStateCalcDifference::isAngle() const
{
    if(auto calculation = calculation_.get())
        return calculation->isAngle();
    return false;
}

AST_NAMESPACE_END