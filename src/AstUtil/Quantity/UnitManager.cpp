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
#include "AstUtil/Logger.hpp"
#include <unordered_map>
#include <vector>

AST_NAMESPACE_BEGIN

UnitManager& UnitManager::Instance()
{
    static UnitManager instance;
    return instance;
}

UnitManager::UnitManager()
{
    // 长度单位
    addUnit(units::cm);
    addUnit(units::m);
    addUnit(units::km);
    addUnit(units::dm);
    addUnit(units::mm);
    addUnit(units::in);
    addUnit(units::ft);
    addUnit(units::yd);
    addUnit(units::mi);
    addUnit(u8"米", units::m);
    addUnit(u8"厘米", units::cm);
    addUnit(u8"千米", units::km);
    addUnit(u8"公里", units::km);

    // 质量单位
    addUnit(units::kg);
    addUnit(units::g);
    addUnit(units::mg);
    addUnit(units::lb);
    addUnit(u8"千克", units::kg);
    addUnit(u8"公斤", units::kg);
    addUnit(u8"克", units::g);
    addUnit(u8"毫克", units::mg);

    // 角度单位
    addUnit(units::rad);
    addUnit(units::deg);
    addUnit("deg", units::deg);
    addUnit(units::arcsec);
    addUnit("arcsec", units::arcsec);
    addUnit(u8"度", units::deg);
    addUnit(u8"弧度", units::rad);

    // 时间单位
    addUnit(units::s);
    addUnit("sec", units::s);
    addUnit("h", units::h);
    addUnit("hour", units::hour);
    addUnit("hr", units::hour);
    addUnit(units::day);
    addUnit(units::minute);
    addUnit(units::ms);
    addUnit(u8"秒", units::s);
    addUnit(u8"毫秒", units::ms);
    addUnit(u8"天", units::day);
    addUnit(u8"分钟", units::minute);
    addUnit(u8"小时", units::hour);

    // 电流单位
    addUnit(units::A);
    addUnit(u8"安培", units::A);

    // 力单位
    addUnit(units::N);
    addUnit(u8"牛顿", units::N);

    // 面积单位
    addUnit(units::m2);
    addUnit("m2", units::m2);
    addUnit(u8"平方米", units::m2);

    // 体积单位
    addUnit("m3", units::m3);
    addUnit(units::L);
    addUnit(u8"立方米", units::m3);
    addUnit(u8"升", units::L);

    // 压强单位
    addUnit(units::Pa);
    addUnit(u8"帕斯卡", units::Pa);

    // 温度单位
    addUnit(units::K);
    addUnit(u8"开尔文", units::K);
    addUnit(units::degC);
    addUnit("degC", units::degC);
    addUnit("celsius", units::degC);
    addUnit(u8"摄氏度", units::degC);
    addUnit(units::degF);
    addUnit("degF", units::degF);
    addUnit("fahrenheit", units::degF);
    addUnit(u8"华氏度", units::degF);
    addUnit(units::degR);
    addUnit("degR", units::degR);
    addUnit("rankine", units::degR);
    addUnit(u8"兰氏度", units::degR);

    // 磁感应强度(磁通密度)单位
    addUnit(units::T);
    addUnit(units::nT);
    addUnit(units::Gs);
    addUnit(u8"特斯拉", units::T);
    addUnit(u8"纳特斯拉", units::nT);
    addUnit(u8"纳特", units::nT);
    addUnit(u8"高斯", units::Gs);

    // 分贝/贝尔单位
    addUnit(units::dB);
    addUnit("db", units::dB);
    addUnit(units::dBm);
    addUnit("dbm", units::dBm);
    addUnit(units::dBW);
    addUnit("dbw", units::dBW);
    addUnit(units::B);
    addUnit("bel", units::B);

    // 无量纲
    addUnit("unitValue", Unit::None());
    addUnit(Unit::Percent());
}

UnitManager::~UnitManager()
{
    for (auto& unit : units_)
    {
        delete unit.second;
    }
}

errc_t UnitManager::addUnit(const Unit& unit)
{
    return _addUnit(unit.name(), unit);
}

errc_t UnitManager::addUnit(StringView name, const Unit& unit)
{
    return _addUnit(std::string(name), unit);
}

Unit* UnitManager::getUnit(StringView name)
{
    auto it = units_.find(std::string(name));
    if (it == units_.end())
    {
        return nullptr;
    }
    return it->second;
}

Unit* UnitManager::getSiUnit(Dimension dim)
{
    Unit* unit = _getSiUnitCache(dim);
    if(unit)
        return unit;
    else
    {
        // 从基本单位进行组合
        {
            std::array<std::pair<Dimension, int>, 8> basicDimensions;
            dim.decompose(basicDimensions);
            Unit composedUnit = Unit::None();
            for(auto& item : basicDimensions)
            {
                int exponent = item.second;
                Dimension basicDim = item.first;
                if(exponent == 0)
                    continue;
                Unit* basicUnit = _getSiUnitCache(basicDim);
                if(basicUnit)
                {
                    composedUnit = composedUnit * basicUnit->pow(exponent);
                }
                else
                {
                    aError("failed to get si unit for basic dimension %s", basicDim.name().c_str());
                    return nullptr;
                }
            }
            Unit* siUnit = new Unit(composedUnit);
            siUnits_[dim] = siUnit;
            return siUnit;
        }
    }
    return nullptr;
}

errc_t UnitManager::_addUnit(const std::string &name, const Unit &unit)
{
    if (units_.find(name) != units_.end())
    {
        aError("unit %s already exists", name.c_str());
        return eErrorInvalidParam;
    }
    units_[name] = new Unit(unit);
    return eNoError;
}

std::vector<Unit> UnitManager::getUnitsByDimension(Dimension dim) const
{
    std::vector<Unit> result;
    // 同名别名(同一个 units::X 拷贝)共享同一 rep 指针，按指针去重是 O(N)
    // 且仍能把 K(eScale) 与摄氏度(eAffine)区分开，避免 getScale() 相同被误合并
    std::unordered_map<const UnitRep*, int> indexByRep;
    for (const auto& item : units_)
    {
        const Unit* unit = item.second;
        if (unit->dimension() != dim)
            continue;
        const UnitRep* rep = unit->rep_.get();
        auto it = indexByRep.find(rep);
        if (it == indexByRep.end())
        {
            indexByRep[rep] = static_cast<int>(result.size());
            result.push_back(*unit);
        }
        else if (!unit->name().empty()
            && unit->name().size() < result[it->second].name().size())
        {
            result[it->second] = *unit;
        }
    }
    return result;
}

Unit* UnitManager::_getSiUnitCache(Dimension dim)
{
    // 1. 先从缓存中查找
    auto it = siUnits_.find(dim);
    if (it != siUnits_.end())
    {
        return it->second;
    }
    else
    {
        // 2. 从单位表中查找
        for (auto& item : units_)
        {
            auto unit = item.second;
            if (unit->dimension() == dim && unit->getScale() == 1.0
                && unit->rep_->kind() == EUnitKind::eScale)
            {
                //Unit* siUnit = new Unit(unit->clone());
                Unit* siUnit = new Unit(*unit);
                siUnits_[dim] = siUnit;
                return siUnit;
            }
        }
    }
    return nullptr;
}

Unit* aUnitGet(StringView name)
{
    return UnitManager::Instance().getUnit(name);
}

Unit* aUnitGetSI(Dimension dim)
{
    return UnitManager::Instance().getSiUnit(dim);
}

Unit* aUnitGetDefault(Dimension dim)
{
    // 角度单位默认使用度
    if(dim == Dimension::Angle())
    {
        if(auto* unit = aUnitGet("deg"))
            return unit;
    }
    else if(dim == Dimension::AngularVelocity())
    {
        if(auto* unit = aUnitGet("deg/sec"))
            return unit;
    }
    return aUnitGetSI(dim);
}

errc_t aUnitAdd(const Unit& unit)
{
    return UnitManager::Instance().addUnit(unit);
}

errc_t aUnitAdd(StringView name, const Unit &unit)
{
    return UnitManager::Instance().addUnit(name, unit);
}

std::vector<Unit> aUnitsGetByDimension(Dimension dim)
{
    return UnitManager::Instance().getUnitsByDimension(dim);
}

AST_NAMESPACE_END


