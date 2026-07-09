///
/// @file      DataElements.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-06
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

#include "DataElements.hpp"
#include <cassert>

AST_NAMESPACE_BEGIN

const DataElement* DataElements::getElement(StringView name) const
{
    // 检查直接名称
    for (const auto& element : elements_)
    {
        if (name == element.name())
        {
            return &element;
        }
    }
    // 检查别名
    for (const auto& alias : aliasMap_)
    {
        if (name == alias.first)
        {
            return find(alias.second);
        }
    }
    return nullptr;
}

void DataElements::addElement(const DataElement &element)
{
    elements_.push_back(element);
}

void DataElements::addAlias(const char *alias, const char *name)
{
    assert(strcmp(alias, name) != 0);
    aliasMap_.push_back({alias, name});
}

AST_NAMESPACE_END

