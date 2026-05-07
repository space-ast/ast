///
/// @file      Value.cpp
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

#include "Value.hpp"
#include "ValNull.hpp"
#include "ValDouble.hpp"
#include "ValInt.hpp"
#include "ValBool.hpp"
#include "ValDict.hpp"
#include "ValQuantity.hpp"
#include "ValString.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include <limits>

AST_NAMESPACE_BEGIN


Value& Value::NullValue()
{
    return *aValueNull();
}

ValDict *Value::toValDict() const
{
    if(this->getType() == &ValDict::staticType)
    {
        return static_cast<ValDict*>(const_cast<Value*>(this));
    }
    return nullptr;
}

static NamedValueVector emptyMap;

const NamedValueVector& Value::items() const
{
    if(auto dict = toValDict())
        return dict->getMap().items();
    return emptyMap;
}



void Value::insert(const std::string& name, Value* value)
{
    ValDict* dict = toValDict();
    if(dict)
        dict->insert(name, value);
    else{
        SharedPtr<Value> p{value};
        A_UNUSED(p);
    }
}

Value& Value::operator[](const std::string& name)
{
    if(auto dict = toValDict())
    {
        auto value = dict->find(name);
        if(value)
            return *value;
    }
    return NullValue();
}

const Value& Value::operator[](const std::string& name) const
{
    return const_cast<Value*>(this)->operator[](name);
}

Value& Value::operator[](const char* name)
{
    return this->operator[](std::string(name));
}

const Value& Value::operator[](const char* name) const
{
    return this->operator[](std::string(name));
}

Value& Value::operator[](size_t index)
{
    // todo: 实现数组索引
    return NullValue();
}

const Value& Value::operator[](size_t index) const
{
    // todo: 实现数组索引
    return NullValue();
}

bool Value::isNull() const
{
    return getType() == &ValNull::staticType;
}

bool Value::isBool() const
{
    return aValueIsBool(const_cast<Value*>(this));
}

bool Value::isInt() const
{
    return aValueIsInt(const_cast<Value*>(this));
}

bool Value::isDouble() const
{
    return aValueIsDouble(const_cast<Value*>(this));
}

bool Value::isQuantity() const
{
    return aValueIsQuantity(const_cast<Value*>(this));
}

bool Value::isString() const
{
    return aValueIsString(const_cast<Value*>(this));
}

std::string Value::toString() const
{
    auto value = const_cast<Value*>(this);
    if(aValueIsString(value)){
        return static_cast<ValString*>(value)->value();
    }
    if(aValueIsDouble(value)){
        return aFormatDouble(static_cast<ValDouble*>(value)->value());
    }   
    if(aValueIsInt(value)){
        return aFormatInt(static_cast<ValInt*>(value)->value());
    }
    if(aValueIsBool(value)){
        return aFormatBool(static_cast<ValBool*>(value)->value());
    }
    return getExpression();
}


Quantity Value::toQuantity() const
{
    auto value = const_cast<Value*>(this);
    if(aValueIsQuantity(value)){
        return static_cast<ValQuantity*>(value)->quantity();
    }
    return Quantity(toDouble());
}

double Value::toDouble() const
{
    auto value = const_cast<Value*>(this);
    if(aValueIsQuantity(value)){
        return static_cast<ValQuantity*>(value)->quantity().getValueSI();
    }
    if(aValueIsDouble(value)){
        return static_cast<ValDouble*>(value)->value();
    }
    if(aValueIsInt(value)){
        return static_cast<ValInt*>(value)->value();
    }
    if(aValueIsBool(value)){
        return static_cast<ValBool*>(value)->value() ? 1.0 : 0.0;
    }
    if(aValueIsString(value)){
        double ret;
        errc_t rc = aParseDouble(static_cast<ValString*>(value)->value(), ret);
        if(rc == eNoError){
            return ret;
        }
    }
    aError("Value is not an arithmetic value and is not a valid arithmetic string");
    return std::numeric_limits<double>::quiet_NaN();
}
int Value::toInt() const
{
    auto value = const_cast<Value*>(this);
    if(aValueIsQuantity(value)){
        return static_cast<int>(static_cast<ValQuantity*>(value)->quantity().getValueSI());
    }
    if(aValueIsInt(value)){
        return static_cast<ValInt*>(value)->value();
    }
    if(aValueIsBool(value)){
        return static_cast<ValBool*>(value)->value() ? 1 : 0;
    }
    if(aValueIsDouble(value)){
        return static_cast<int>(static_cast<ValDouble*>(value)->value());
    }
    if(aValueIsString(value)){
        int ret;
        errc_t rc = aParseInt(static_cast<ValString*>(value)->value(), ret);
        if(rc == eNoError){
            return ret;
        }
    }
    aError("Value is not an integer and is not a valid integer string");
    return 0;
}
bool Value::toBool() const
{
    auto value = const_cast<Value*>(this);
    if(aValueIsQuantity(value)){
        return static_cast<ValQuantity*>(value)->quantity().getValueSI() != 0.0;
    }
    if(aValueIsBool(value)){
        return static_cast<ValBool*>(value)->value();
    }
    if(aValueIsInt(value)){
        return static_cast<ValInt*>(value)->value() != 0;
    }
    if(aValueIsDouble(value)){
        return static_cast<ValDouble*>(value)->value() != 0.0;
    }
    if(aValueIsString(value)){
        bool ret;
        errc_t rc = aParseBool(static_cast<ValString*>(value)->value(), ret);
        if(rc == eNoError){
            return ret;
        }
    }
    aError("Value is not a bool and is not a valid boolean string");
    return false;
}

Value::operator std::string() const
{
    return toString();
}
Value::operator Quantity() const
{
    return toQuantity();
}
Value::operator double() const
{
    return toDouble();
}
Value::operator int() const
{
    return toInt();
}
Value::operator bool() const
{
    return toBool();
}

AST_NAMESPACE_END
