///
/// @file      UnitManager.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-15
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

#include "UnitManager.hpp"

AST_NAMESPACE_BEGIN

UnitManager& UnitManager::Instance()
{
    static UnitManager instance;
    return instance;
}


UnitManager::~UnitManager()
{
    for (auto& unit : units_)
    {
        delete unit.second;
    }
}

void UnitManager::addUnit(const Unit& unit)
{
    units_[unit.name()] = new Unit(unit);
}


void UnitManager::addUnit(std::string name, const Unit& unit)
{
    units_[name] = new Unit(unit);
}

Unit* UnitManager::getUnit(std::string name)
{
    auto it = units_.find(name);
    if (it == units_.end())
    {
        return nullptr;
    }
    return it->second;
}



AST_NAMESPACE_END
