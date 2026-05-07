///
/// @file      ExprAttribute.cpp
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

#include "ExprAttribute.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN

ExprAttribute* ExprAttribute::New(const Attribute& attribute)
{
    return new ExprAttribute(attribute);
}   

ExprAttribute::ExprAttribute(const Attribute& attribute)
    : attribute_(attribute)
{

}

Value *ExprAttribute::eval() const
{
    EValueType valueType = attribute_.getValueType();
    switch (valueType)
    {
    case EValueType::eBool:
        return aNewValueBool(attribute_.getValueBool());
    case EValueType::eInt:
        return aNewValueInt(attribute_.getValueInt());
    case EValueType::eDouble:
    case EValueType::eQuantity:
        return aNewValueDouble(attribute_.getValueDouble());
    case EValueType::eTimePoint:
    case EValueType::eString:
        return aNewValueString(attribute_.getValueString());
    default:
        aWarning("invalid value type");
        return nullptr;
    }

    return nullptr;
}

errc_t ExprAttribute::setValue(Value *value)
{
    if(!value)
        return eErrorNullInput;
    if(value->isBool())
        return attribute_.setValueBool(value->toBool());
    else if(value->isInt())
        return attribute_.setValueInt(value->toInt());
    else if(value->isDouble() || value->isQuantity())
        return attribute_.setValueDouble(value->toDouble());
    else
        return attribute_.setValueString(value->toString());
}

std::string ExprAttribute::getExpression(Object *context) const
{
    aWarning("not implemented");
    return {};
}

AST_NAMESPACE_END

