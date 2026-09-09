///
/// @file      Property.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-09
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

#include "Property.hpp"
#include "PropertyObject.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

errc_t aFakeGet(const void *container, void *value)
{
    aError(_("getter 为空"));
    return eErrorNullInput;
}

errc_t aFakeSet(void* container, const void* value)
{
    aError(_("setter 为空"));
    return eErrorNullInput;
}

double Property::getValueDouble(const void* container)
{
    double value = 0;
    errc_t rc = this->getValueDouble(container, value);
    if(rc)
    {
        aError(_("获取 double 值失败"));
    }
    return value;
}

int Property::getValueInt(const void* container)
{
    int value = 0;
    errc_t rc = this->getValueInt(container, value);
    if(rc)
    {
        aError(_("获取 int 值失败"));
    }
    return value;
}

bool Property::getValueBool(const void* container)
{
    bool value = false;
    errc_t rc = this->getValueBool(container, value);
    if(rc)
    {
        aError(_("获取 bool 值失败"));
    }
    return value;
}

std::string Property::getValueString(const void* container)
{
    std::string value;
    errc_t rc = this->getValueString(container, value);
    if(rc)
    {
        aError(_("获取 string 值失败"));
    }
    return value;
}


bool Property::isObject() const
{
    return dynamic_cast<const PropertyObject*>(this) != nullptr;
}

errc_t Property::setValueObject(void* container, Object* value)
{
    if(auto prop = dynamic_cast<PropertyObject*>(this))
    {
        return prop->setValue(container, value);
    }
    else
    {
        aError(_("属性不是对象类型"));
    }
    return eNoError;
}

errc_t Property::getValueObject(const void* container, Object*& value)
{
    if(auto prop = dynamic_cast<PropertyObject*>(this))
    {
        return prop->getValue(container, &value);
    }
    else
    {
        value = nullptr;
        aError(_("属性不是对象类型"));
    }
    return eNoError;
}



AST_NAMESPACE_END

