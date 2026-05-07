///
/// @file      Variable.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "Variable.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/ParseFormat.hpp"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/UnitManager.hpp"

AST_NAMESPACE_BEGIN


Variable* Variable::New()
{
    return new Variable();
}


Variable::Variable(StringView name, Expr *expr, bool bind)
    : name_(name), expr_(expr), bind_(bind)
{};

Variable::Variable(Expr *expr, bool bind)
    : name_{}, expr_(expr), bind_(bind)
{};

Value *Variable::eval() const
{
    if(expr_.get() == nullptr) {
        return nullptr;
    }
    return expr_->eval();
}

errc_t Variable::setValue(Value *val)
{
    if(!val){
        return eErrorNullInput;
    }
    if(bind_){
        // 双向绑定，设置表达式的值
        return expr_->setValue(val);
    }else{
        // 单向绑定，直接替换表达式
        expr_ = val;
        return eNoError;
    }
}

errc_t Variable::setExpr(Expr *expr)
{
    expr_ = expr;
    bind_ = false;
    return eNoError;
}

errc_t Variable::setBindExpr(Expr *expr)
{
    expr_ = expr;
    bind_ = true;
    return eNoError;
}

errc_t Variable::setExpr(Value* value)
{
    return setValue(const_cast<Value*>(value));
}

errc_t Variable::setExpr(StringView value)
{
    return setExpr(aNewValue(value));
}

errc_t Variable::setExpr(const std::string& value)
{
    return setExpr(aNewValue(value));
}

errc_t Variable::setExpr(const Quantity& quantity)
{
    return setExpr(aNewValue(quantity));
}

errc_t Variable::setExpr(double value)
{
    return setExpr(aNewValue(value));
}

errc_t Variable::setExpr(int value)
{
    return setExpr(aNewValue(value));
}

errc_t Variable::setExpr(bool value)
{
    return setExpr(aNewValue(value));
}

errc_t Variable::bind(Expr *expr)
{
    expr_ = expr;
    bind_ = true;
    return eNoError;
}

std::string Variable::value() const
{
    SharedPtr<Value> value = eval();
    if(value){
        return value->toString();
    }
    return {};
}

errc_t Variable::setValue(StringView value)
{
    SharedPtr<Value> val = eval();
    if(val)
    {
        if(val->isQuantity())
        {
            Quantity quantityNew;
            aQuantityParse(value, quantityNew);
            if(quantityNew.dimension() == EDimension::eUnit)
            {
                Quantity quantityOld = val->toQuantity();
                if(quantityOld.dimension() != EDimension::eUnit)
                {
                    if(auto punit = aUnitGetSI(quantityOld.dimension()))
                    {
                        quantityNew.setUnit(*punit);
                    }
                }
            }
            return this->setValue(aNewValue(quantityNew));
        }
        else if(val->isDouble())
        {
            return this->setValue(aNewValue(aParseDouble(value)));
        }
        else if(val->isInt())
        {
            return this->setValue(aNewValue(aParseInt(value)));
        }
        else if(val->isBool())
        {
            return this->setValue(aNewValue(aParseBool(value)));
        }
    }
    return setValue(aNewValue(value));
}

AST_NAMESPACE_END

