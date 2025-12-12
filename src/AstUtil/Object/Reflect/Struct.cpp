///
/// @file      Struct.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-10
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

#include "Struct.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Property.hpp"


AST_NAMESPACE_BEGIN

Struct::~Struct()
{
    for (auto property : m_properties)
    {
        delete property;
    }
}

Property *Struct::addProperty(StringView name, Property *property)
{
    m_propertyMap[name.to_string()] = property;
    m_properties.push_back(property);
    return property;
}

Property *Struct::addProperty(Property *property)
{
    return addProperty(property->name(), property);
}

Property *Struct::getProperty(StringView name)
{
    auto it = m_propertyMap.find(name.to_string());
    if (it == m_propertyMap.end())
    {
        return nullptr;
    }
    return it->second;
}

AST_NAMESPACE_END
