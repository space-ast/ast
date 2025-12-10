///
/// @file      Struct.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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

#pragma once

#include "AstGlobal.h"
#include "Field.hpp"
#include <unordered_map>
#include <vector>


AST_NAMESPACE_BEGIN

class Property;

class Struct: public Field
{
public:
    using Field::Field;

    // @todo: 考虑使用StringView作为索引
    using PropertyMap = std::unordered_map<std::string, Property*>;
    using PropertyList = std::vector<Property*>;
    
    Property* addProerty(StringView name, Property* property);

    /// @brief 添加属性
    /// @param property 属性
    /// @return Property* 属性指针
    Property* addProperty(Property* property);
    
    /// @brief 获取属性
    /// @param name 属性名
    /// @return Property* 属性指针
    Property* getProperty(StringView name);

protected:
    PropertyList m_properties;
    PropertyMap  m_propertyMap;
};



AST_NAMESPACE_END