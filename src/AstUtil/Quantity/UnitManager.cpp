///
/// @file      UnitManager.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
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

UnitManager::UnitManager()
{
    addUnit(units::cm);
    addUnit(units::m);
    addUnit(units::km);
    addUnit(units::dm);
    addUnit(units::mm);
    addUnit(units::in);
    addUnit(units::ft);
    addUnit(units::yd);
    addUnit(units::mi);

    addUnit(units::kg);
    addUnit(units::g);
    addUnit(units::mg);
    addUnit(units::lb);

    addUnit(units::rad);
    addUnit(units::deg);
    addUnit("deg", units::deg);

    addUnit(units::s);
    addUnit(units::h);
    addUnit(units::day);
    addUnit(units::hour);
    addUnit(units::min);
    addUnit(units::ms);

    addUnit(units::A);

    addUnit(units::N);

    addUnit(units::m2);
    addUnit("m2", units::m2);

    addUnit("m3", units::m3);
    addUnit(units::L);
}

UnitManager::~UnitManager()
{
    for (auto& unit : units_)
    {
        delete unit.second;
    }
}

err_t UnitManager::addUnit(const Unit& unit)
{
    return _addUnit(unit.name(), unit);
}

err_t UnitManager::addUnit(StringView name, const Unit& unit)
{
    return _addUnit(name.to_string(), unit);
}

Unit* UnitManager::getUnit(StringView name)
{
    auto it = units_.find(name.to_string());
    if (it == units_.end())
    {
        return nullptr;
    }
    return it->second;
}

err_t UnitManager::_addUnit(const std::string &name, const Unit &unit)
{
    if (units_.find(name) != units_.end())
    {
        return eErrorInvalidParam;
    }
    units_[name] = new Unit(unit);
    return eNoError;
}

Unit* aUnitGet(StringView name)
{
    return UnitManager::Instance().getUnit(name);
}

err_t aUnitAdd(const Unit& unit)
{
    return UnitManager::Instance().addUnit(unit);
}

err_t aUnitAdd(StringView name, const Unit &unit)
{
    return UnitManager::Instance().addUnit(name, unit);
}

AST_NAMESPACE_END


