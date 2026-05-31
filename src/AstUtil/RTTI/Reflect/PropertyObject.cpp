///
/// @file      PropertyObject.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-03-30
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

#include "PropertyObject.hpp"
#include "AstUtil/PropertyVisitor.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/RTTIAPI.hpp"

AST_NAMESPACE_BEGIN

PropertyObject::PropertyObject(FPropertyGet getter, FPropertySet setter, Class* cls)
    : Property(getter, setter)
    , class_(cls)
{
}

errc_t PropertyObject::getValueBool(const void* container, bool& value)
{
    aError("failed to get bool value from property object");
    return -1;
}

errc_t PropertyObject::setValueBool(void* container, bool value)
{
    aError("failed to set bool value to property object");
    return -1;
}

errc_t PropertyObject::getValueInt(const void* container, int& value)
{
    Object* object = nullptr;
    errc_t rc = getValue(container, &object);
    if(rc != 0)
        return rc;
    if(!object)
        return -1;
    value = object->getID();
    return 0;
}

errc_t PropertyObject::setValueInt(void* container, int value)
{
    Object* object = aGetObject(value);
    return setValue(container, object);
}

errc_t PropertyObject::getValueString(const void* container, std::string& value)
{
    Object* object = nullptr;
    errc_t rc = getValue(container, &object);
    if(rc != 0)
        return rc;
    value = object->getRepresentation();
    if(value.empty())
    {
        aError("failed to get string value from property object");
        return -1;
    }
    return 0;
}

errc_t PropertyObject::setValueString(void* container, StringView value)
{
    Object* object = aResolveObject(value, class_);
    if(!object)
        aWarning("failed to resolve object: '%.*s'", value.size(), value.data());
    return setValue(container, object);
}

errc_t PropertyObject::getValueDouble(const void* container, double& value)
{
    aError("failed to get double value from property object");
    return -1;
}

errc_t PropertyObject::setValueDouble(void* container, double value)
{
    aError("failed to set double value to property object");
    return -1;
}


errc_t PropertyObject::setValue(void* container, const InputType* value)
{
    if(!value)
    {
        aError("null input value for property object");
        return eErrorNullInput;
    }
    if(class_ && !value->isOfType(class_))
    {
        aError("invalid type for property object, expect '%s', but given '%s'", class_->name().c_str(), value->getType()->name().c_str());
        return eErrorInvalidType;
    }
    return setter_(container, &value);
}


errc_t PropertyObject::getValue(const void* container, OutputType* value)
{
    return getter_(container, value);
}

errc_t PropertyObject::accept(PropertyVisitor& visitor, const void* container)
{
    return visitor.visit(*this, container);
}

AST_NAMESPACE_END