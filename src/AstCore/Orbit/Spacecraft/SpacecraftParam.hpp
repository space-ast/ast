///
/// @file      SpacecraftParam.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-09
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 航天器参数
class AST_CORE_API SpacecraftParam
{
public:
    SpacecraftParam() = default;
    ~SpacecraftParam() = default;
public:
    double cd() const { return cd_; }
    void setCd(double cd) { cd_ = cd; }

    double cr() const { return cr_; }
    void setCr(double cr) { cr_ = cr; }

    double dragArea() const { return dragArea_; }
    void setDragArea(double dragArea) { dragArea_ = dragArea; }
    
    double srpArea() const { return srpArea_; }
    void setSrpArea(double srpArea) { srpArea_ = srpArea; }

    double mass() const { return dryMass_ + fuelMass_; }

    double dryMass() const { return dryMass_; }
    void setDryMass(double dryMass) { dryMass_ = dryMass; }

    double fuelMass() const { return fuelMass_; }
    void setFuelMass(double fuelMass) { fuelMass_ = fuelMass; }

    double fuelDensity() const { return fuelDensity_; }
    void setFuelDensity(double fuelDensity) { fuelDensity_ = fuelDensity; }

    double k1() const { return k1_; }
    void setK1(double k1) { k1_ = k1; }
    
    double k2() const { return k2_; }
    void setK2(double k2) { k2_ = k2; }

    double radPressureArea() const { return radPressureArea_; }
    void setRadPressureArea(double radPressureArea) { radPressureArea_ = radPressureArea; }
    
    double radPressureCoeff() const { return radPressureCoeff_; }
    void setRadPressureCoeff(double radPressureCoeff) { radPressureCoeff_ = radPressureCoeff; }

    double tankPressure() const { return tankPressure_; }
    void setTankPressure(double tankPressure) { tankPressure_ = tankPressure; }

    double tankTemperature() const { return tankTemperature_; }
    void setTankTemperature(double tankTemperature) { tankTemperature_ = tankTemperature; }
protected:
    double cd_{2.2};                         ///< 阻力系数
    double cr_{1};                           ///< 光压系数
    double dragArea_{20};                    ///< 阻力面积
    double srpArea_{20};                     ///< SRP面积
    double dryMass_{500};                    ///< 干质量
    double fuelMass_{500};                   ///< 燃油质量
    double fuelDensity_{1000};               ///< 燃油密度
    double k1_{1};                           ///< 系数1
    double k2_{1};                           ///< 系数2
    double radPressureArea_{20};             ///< 辐射压力面积
    double radPressureCoeff_{1};             ///< 辐射压力系数
    double tankPressure_{5000};              ///< 罐压力
    double tankTemperature_{293.15};         ///< 罐温度
};

/*! @} */

AST_NAMESPACE_END
