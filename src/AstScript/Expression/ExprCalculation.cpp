///
/// @file      ExprCalculation.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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

#include "ExprCalculation.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

ExprCalculation* ExprCalculation::New(Object* obj, ObjectCalculation* calc)
{
    return new ExprCalculation(obj, calc);
}

ExprCalculation::ExprCalculation(Object* obj, ObjectCalculation* calc)
    : object_(obj)
    , calculation_(calc)
{
    syncTypeMatched();
}


Value* ExprCalculation::eval() const
{
    aWarning("not implemented");
    return nullptr;
}

errc_t ExprCalculation::setValue(Value* val)
{
    aWarning("not implemented");
    return eErrorReadonly;
}

std::string ExprCalculation::getExpression(Object * context) const
{
    aWarning("not implemented");
    return {};
}

errc_t ExprCalculation::calculate(double& val)
{
    if (!typeMatched_)
    {
        aError("type not matched");
        return eErrorInvalidType;
    }
    if(auto calc = calculation_.get())
    {
        return calc->calculate(object_.get(), val);
    }
    else
    {
        aError("calculation not set");
        return eErrorNullPtr;
    }
}

void ExprCalculation::setCalculation(ObjectCalculation* calc)
{
    calculation_ = calc;
    syncTypeMatched();
}

void ExprCalculation::setObject(Object* obj)
{
    object_ = obj;
    syncTypeMatched();
}

void ExprCalculation::syncTypeMatched()
{
    if(calculation_)
    {
        typeMatched_ = calculation_->isExpectedType(object_.get());
    }
}

AST_NAMESPACE_END
