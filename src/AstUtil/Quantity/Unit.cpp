///
/// @file      Unit.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
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

#include "Unit.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/ParseFormat.hpp"

AST_NAMESPACE_BEGIN


/// @brief 从缩放因子生成无量纲单位名称
/// @param scale 缩放因子
/// @return 单位名称
static std::string unit_name_from_scale_dimless(double scale)
{
    if (scale == 1.0)
    {
        return aText(""); // @fixme: 在这里使用空字符串吗？
    }
    return aText("×") + aFormatDouble(scale);
}

/// @brief 新建无量纲单位
/// @param scale 缩放因子
/// @return 单位表示
static Unit::UnitRepHandle unit_new_dimenless(double scale)
{
    return std::make_shared<Unit::UnitRep>(unit_name_from_scale_dimless(scale), scale, EDimension::eUnit, Unit::SubUnitListConst{});
}

/// @brief 单位名称分隔符
const char* unit_name_separator = aText("·");

/// @brief 从单位子项列表生成单位名称
/// @param subUnits 单位子项列表
/// @return 单位名称
static std::string unit_name_from_subunits(const Unit::SubUnitListConst& subUnits)
{
    if (subUnits.empty())
    {
        return "";
    }
    std::string name;
    for (const auto& unitpair : subUnits)
    {
        if (!name.empty())
        {
            name += unit_name_separator;
        }
        name += unitpair.first->name_;
        if (unitpair.second != 1)
        {
            name += "^" + std::to_string(unitpair.second);
        }
    }
    return name;
}

/// @brief 生成单位名称
/// @param unit 单位
/// @return 单位名称
static std::string unit_name_generate(const Unit& unit)
{
    auto name =  unit_name_from_subunits(unit.rep_->subUnits_);
    if (name.empty())
    {
        if(unit.rep_->dimension_ == EDimension::eUnit){
            return unit_name_from_scale_dimless(unit.rep_->scale_);
        }
    }
    return name;
}

/// @brief 从单位子项列表计算缩放因子
/// @param subUnits 单位子项列表
/// @return 缩放因子
static double unit_scale_from_subunits(const Unit::SubUnitListConst& subUnits)
{
    double scale = 1.0;
    for (const auto& unitpair : subUnits)
    {
        scale *= std::pow(unitpair.first->scale_, unitpair.second);
    }
    return scale;
}


/// @brief 化简单位子项
/// @param subUnits 单位子项列表
/// @param newSubUnits 新的单位子项列表
/// @param dimlessUnit 无量纲单位
/// @param extra_scale 额外缩放因子
static void unit_reduce_subunits(const Unit::SubUnitListConst& subUnits, Unit::SubUnitListConst& newSubUnits_, Unit::UnitRepHandleConst& dimlessUnit)
{
    if (subUnits.empty())
    {
        return;
    }
    Unit::SubUnitListConst newSubUnits;                             // 新的单位子项列表
    std::vector<Unit::UnitRepHandleConst> dimlessUnits;             // 无量纲单位项列表
    double dimlessScale = 1.0;                                      // 额外缩放因子(量纲间乘除产生的缩放因子)

    for (auto it = subUnits.begin(); it != subUnits.end(); it++)
    {
        if (it->second == 0)
        {
            continue; // 跳过指数为0的单位子项
        }
        else
        {
            if(it->first->dimension_ == EDimension::eUnit){
                dimlessScale *= std::pow(it->first->scale_, it->second);
                dimlessUnits.push_back(it->first);
            }else{
                // 查找是否已存在相同量纲的单位
                auto found = std::find_if(newSubUnits.begin(), newSubUnits.end(),
                    [it](const auto& newUnitpair)
                    {
                        return newUnitpair.first->dimension_ == it->first->dimension_;
                    });
                if (found != newSubUnits.end())
                {
                    if(found->first->scale_ == it->first->scale_){
                        // 相同量纲的单位子项，指数相加
                        // do nothing here
                    }
                    else if(found->first->scale_ > it->first->scale_)
                    {
                        dimlessScale *= std::pow(found->first->scale_ / it->first->scale_, found->second);   // 更新额外的缩放因子
                        found->first = it->first;                                                            // 更新单位子项
                    }else // if(found->first->scale_ < it->first->scale_)
                    {
                        dimlessScale *= std::pow(it->first->scale_ / found->first->scale_, it->second);      // 更新额外的缩放因子
                        // found->first = found->first;                                                      // 更新单位子项
                    }
                    found->second += it->second;
                }
                else
                {
                    newSubUnits.push_back(*it);
                }
            }
        }
    }
    for (auto it = newSubUnits.begin(); it != newSubUnits.end(); )
    {
        if (it->second == 0)
        {
            it = newSubUnits.erase(it); // 删除指数为0的单位子项
        }
        else {
            it++;
        }
    }
    dimlessUnit = nullptr;                                          // 清空
    if(dimlessScale != 1.0){
        for(auto& it: dimlessUnits){
            if(it->scale_ == dimlessScale){
                dimlessUnit = it;
            }
        }
        if(!dimlessUnit){
            dimlessUnit = unit_new_dimenless(dimlessScale);
        }
    }
    newSubUnits_ = std::move(newSubUnits);
    // return unit_scale_from_subunits(subUnits);
}

static void unit_reduce_subunits(Unit::SubUnitListConst& subUnits, Unit::UnitRepHandleConst& dimlessUnit)
{
    unit_reduce_subunits(subUnits, subUnits, dimlessUnit);
}


static double unit_reduce_subunits(Unit::SubUnitListConst& subUnits)
{
    Unit::UnitRepHandleConst dimlessUnit;
    unit_reduce_subunits(subUnits, dimlessUnit);
    if(dimlessUnit){
        subUnits.emplace(subUnits.begin(), Unit::UnitRepPairConst{ dimlessUnit, 1 });
    }
    return unit_scale_from_subunits(subUnits);
}

/// @brief 化简单位
/// @param unit 单位
/// @return 缩放因子
static double unit_reduce(Unit& unit)
{
    if(unit.dimension() == EDimension::eUnit){
        double scale = unit.getScale();
        unit.rep_ = unit_new_dimenless(scale);
        return scale;
    }
    return unit_reduce_subunits(unit.rep_->subUnits_);
}


/// @brief 单位乘法
/// @param unit1 单位1
/// @param unit2 单位2
/// @return 单位乘积
Unit unit_multiply(const Unit& unit1, const Unit& unit2)
{
    Unit::SubUnitListConst subunits;
    if (unit1.rep_->subUnits_.empty()) {
        subunits.push_back({unit1.rep_, 1});
    }
    else {
        subunits.insert(subunits.begin(), unit1.rep_->subUnits_.begin(), unit1.rep_->subUnits_.end());
    }
    if (unit2.rep_->subUnits_.empty())
    {
        subunits.push_back({unit2.rep_, 1});
    }
    else {
        subunits.insert(subunits.begin(), unit2.rep_->subUnits_.begin(), unit2.rep_->subUnits_.end());
    }
    Unit retval = Unit(unit1.getScale() * unit2.getScale(), unit1.dimension() * unit2.dimension(), subunits);
    // 化简单位子项
    double scale = unit_reduce(retval);
    if(fabs(scale - retval.getScale()) > 1e-15 * fabs(scale))
    {
        aError("unexpected condition, scale(%lf) != retval.getScale()(%lf)", scale, retval.getScale());
        assert(scale == retval.getScale());
    }
    return retval;
}

/// @brief 单位除法
/// @param unit1 单位1
/// @param unit2 单位2
/// @return 单位商
Unit unit_divide(const Unit& unit1, const Unit& unit2)
{
    Unit::SubUnitListConst subunits;
    if (unit1.rep_->subUnits_.empty()) {
        subunits.push_back({unit1.rep_, 1});
    }
    else {
        subunits.insert(subunits.begin(), unit1.rep_->subUnits_.begin(), unit1.rep_->subUnits_.end());
    }
    if (unit2.rep_->subUnits_.empty())
    {
        subunits.push_back({unit2.rep_, -1});
    }
    else {
        for(auto unitpair: unit2.rep_->subUnits_){
            subunits.push_back({unitpair.first, -unitpair.second});
        }
    }
    Unit retval = Unit(unit1.getScale() / unit2.getScale(), unit1.dimension() / unit2.dimension(), subunits);
    // 化简单位子项
    double scale = unit_reduce(retval);
    if (fabs(scale - retval.getScale()) > 1e-15 * fabs(scale))
    {
        aError("unexpected condition, scale(%lf) != retval.getScale()(%lf)", scale, retval.getScale());
        assert(scale == retval.getScale());
    }
    return retval;
}

/// @brief 单位幂
/// @param unit 单位
/// @param exponent 指数
/// @return 单位幂
Unit unit_power(const Unit& unit, int exponent)
{
    Unit::SubUnitListConst subunits;
    if (unit.rep_->subUnits_.empty()) {
        subunits.push_back({unit.rep_, exponent});
    }
    else {
        subunits = unit.rep_->subUnits_;
        for(auto& unitpair: subunits){
            unitpair.second *= exponent;
        }
    }

    Unit retval = Unit(std::pow(unit.getScale(), exponent), unit.dimension().pow(exponent), subunits);
    // double scale = unit_reduce_subunits(subunits);
    double scale = unit_scale_from_subunits(subunits);
    if (fabs(scale - retval.getScale()) > 1e-15 * fabs(scale))
    {
        aError("unexpected condition, scale(%lf) != retval.getScale()(%lf)", scale, retval.getScale());
        assert(scale == retval.getScale());
    }
    return retval;
}

/// @brief 设置单位名称
/// @param unit 单位
/// @param name 名称
void unit_setname(Unit& unit, StringView name)
{
    unit.rep_->name_ = std::string(name);
}


Unit aUnitMultiply(const Unit& unit1, const Unit& unit2)
{
    Unit retval = unit_multiply(unit1, unit2);
    unit_setname(retval, unit_name_generate(retval));
    return retval;
}

Unit aUnitMultiply(const Unit& unit1, const Unit& unit2, StringView newname)
{
    Unit retval = unit_multiply(unit1, unit2);
    unit_setname(retval, newname);
    return retval;
}

Unit aUnitDivide(const Unit& unit1, const Unit& unit2)
{
    Unit retval = unit_divide(unit1, unit2);
    unit_setname(retval, unit_name_generate(retval));
    return retval;
}

Unit aUnitDivide(const Unit& unit1, const Unit& unit2, StringView newname)
{
    Unit retval = unit_divide(unit1, unit2);
    unit_setname(retval, newname);
    return retval;
}

Unit aUnitPower(const Unit& unit, int exponent)
{
    Unit retval = unit_power(unit, exponent);
    unit_setname(retval, unit_name_generate(retval));
    return retval;
}


Unit aUnitPower(const Unit& unit, int exponent, StringView newname)
{
    Unit retval = unit_power(unit, exponent);
    unit_setname(retval, newname);
    return retval;
}

void aUnitFactorize(const Unit &unit, Unit &newUnit, double &scale)
{
    Unit::UnitRepHandleConst dimless;
    unit_reduce_subunits(unit.rep_->subUnits_, newUnit.rep_->subUnits_, dimless);
    newUnit.rep_->scale_ = unit_scale_from_subunits(newUnit.rep_->subUnits_);
    newUnit.rep_->name_ = unit_name_from_subunits(newUnit.rep_->subUnits_);
    if(dimless)
        scale = dimless->scale_;
    else
        scale = 1.0;
}

void aUnitFactorize(Unit &unit, double &scale)
{
    aUnitFactorize(unit, unit, scale);
}

Unit Unit::Scale(double scale)
{
    return Unit(unit_name_from_scale_dimless(scale), scale, EDimension::eUnit);
}


namespace units
{
    Unit mm = Unit::Millimeter();
    Unit cm = Unit::Centimeter();
    Unit dm = Unit::Decimeter();
    Unit m = Unit::Meter();
    Unit km = Unit::Kilometer();

    Unit in = Unit::Inch();
    Unit yd = Unit::Yard();
    Unit ft = Unit::Foot();
    Unit mi = Unit::Mile();
    
    Unit sec = Unit::Second();
    Unit s = sec;
    Unit min = Unit::Minute();
    Unit hour = Unit::Hour();
    Unit h = hour;
    Unit day = Unit::Day();

    Unit kg = Unit::Kilogram();
    Unit g = Unit::Gram();
    Unit mg = Unit::Milligram();
    Unit lb = Unit::Pound();

    Unit N = Unit::Newton();

    Unit deg = Unit::Degree();
    Unit rad = Unit::Radian();

    Unit m2 = Unit::SquareMeter();

    Unit m3 = Unit::CubicMeter();
    Unit L = Unit::Liter();
}

AST_NAMESPACE_END

