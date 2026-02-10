/// @file      Object.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      19.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 
#include "Object.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Class.hpp"            // for Class
#include "AstUtil/Property.hpp"         // for Property
 
AST_NAMESPACE_BEGIN
 

static_assert(sizeof(Object) == sizeof(void*) * 2 + sizeof(uint32_t) * 2, "size not correct");      // 检查 Object 类的大小是否正确

err_t Object::getAttrBool(StringView path, bool &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueBool(this, value);
}

err_t Object::getAttrInt(StringView path, int &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueInt(this, value);
}

err_t Object::getAttrDouble(StringView path, double &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueDouble(this, value);
}

err_t Object::getAttrString(StringView path, std::string &value) const
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->getValueString(this, value);
}

err_t Object::setAttrBool(StringView path, bool value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueBool(this, value);
}

err_t Object::setAttrInt(StringView path, int value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueInt(this, value);
}

err_t Object::setAttrDouble(StringView path, double value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueDouble(this, value);
}

err_t Object::setAttrString(StringView path, StringView value)
{
    Property* prop = getProperty(path);
    if(!prop)
        return eErrorInvalidParam;
    return prop->setValueString(this, value);
}

Property *Object::getProperty(StringView fieldName) const
{
    return m_type->getProperty(fieldName);
}


AST_NAMESPACE_END