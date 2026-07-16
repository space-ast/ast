///
/// @file      SpacecraftState.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-25
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "SpacecraftParam.hpp"
#include "AstCore/State.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 航天器状态，包含轨道状态、航天器参数（质量、面积、阻力系数、光压、密度、压力、温度等）
/// @details 参考orekit的SpacecraftState类
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif
class AST_CORE_API SpacecraftState: public ObjectNamed, public SpacecraftParam
{
public:
    AST_OBJECT(SpacecraftState)
    AST_PROPERT(Frame)
    AST_PROPERT(Mass)
    AST_PROPERT(FuelMass)
    AST_PROPERT(DryMass)
    AST_PROPERT(Cd)
    AST_PROPERT(Cr)
    AST_PROPERT(DragArea)
    AST_PROPERT(SRPArea)
    AST_PROPERT(K1)
    AST_PROPERT(K2)
    AST_PROPERT(FuelDensity)
    AST_PROPERT(RadPressureArea)
    AST_PROPERT(RadPressureCoeff)
    AST_PROPERT(TankPressure)
    AST_PROPERT(TankTemperature)
    AST_PROPERT(OrbitState)
;
 
    static SpacecraftState* NewDefault();
    
    SpacecraftState();
    ~SpacecraftState() = default;
PROPERTIES:
    Frame* getFrame() const;
    void setFrame(Frame* frame);
    
    /// @brief 获取质量
    /// @return 质量
    mass_d getMass() const{return fuelMass_ + dryMass_;}
    mass_d getFuelMass() const{return fuelMass_;}
    mass_d getDryMass() const{return dryMass_;}

    /// @brief 设置质量
    /// @param mass 质量
    void setFuelMass(mass_d fuelMass){fuelMass_ = fuelMass;}
    void setDryMass(mass_d dryMass){dryMass_ = dryMass;}

    double getCd() const{return cd_;}
    void setCd(double cd){cd_ = cd;}

    double getCr() const{return cr_;}
    void setCr(double cr){cr_ = cr;}
    
    area_d getDragArea() const{return dragArea_;}
    void setDragArea(area_d dragArea){dragArea_ = dragArea;}

    area_d getSRPArea() const{return srpArea_;}
    void setSRPArea(area_d srpArea){srpArea_ = srpArea;}

    double getK1() const{return k1_;}
    void setK1(double k1){k1_ = k1;}

    double getK2() const{return k2_;}
    void setK2(double k2){k2_ = k2;}

    density_d getFuelDensity() const{return fuelDensity_;}
    void setFuelDensity(density_d fuelDensity){fuelDensity_ = fuelDensity;}

    area_d getRadPressureArea() const{return radPressureArea_;}
    void setRadPressureArea(area_d radPressureArea){radPressureArea_ = radPressureArea;}

    double getRadPressureCoeff() const{return radPressureCoeff_;}
    void setRadPressureCoeff(double radPressureCoeff){radPressureCoeff_ = radPressureCoeff;}


    pressure_d getTankPressure() const{return tankPressure_;}
    void setTankPressure(pressure_d tankPressure){tankPressure_ = tankPressure;}

    temperature_d getTankTemperature() const{return tankTemperature_;}
    void setTankTemperature(temperature_d tankTemperature){tankTemperature_ = tankTemperature;}


    /// @brief 获取轨道状态
    /// @return 轨道状态
    State* getOrbitState() const;

    /// @brief 设置轨道状态
    /// @param orbitState 轨道状态
    void setOrbitState(State* orbitState){orbitState_ = orbitState;}
public:
    /// @brief 设置轨道状态类型
    /// @param stateType 轨道状态类型
    void setStateType(EStateType stateType);

    /// @brief 获取轨道状态类型
    /// @return 轨道状态类型
    EStateType getStateType() const;

    const SpacecraftParam& spacecraftParam() const{return *this;}
    SpacecraftParam& spacecraftParam(){return *this;}
    void setSpacecraftParam(const SpacecraftParam& spacecraftParam){this->spacecraftParam() = spacecraftParam;}
public:
    errc_t getState(ModOrbElem& orbElem) const;
    errc_t getState(CartState& state) const;
    errc_t getStateIn(Frame* frame, CartState& state) const;
    errc_t getStateIn(Frame* frame, ModOrbElem& orbElem) const;
    errc_t getStateInBodyInertial(Body* body, CartState& state) const;
    errc_t setState(const ModOrbElem& orbElem);
    errc_t setState(const CartState& state);
    void setStateEpoch(const TimePoint& stateEpoch);
    errc_t getStateEpoch(TimePoint& stateEpoch) const;
    void copyFrom(const SpacecraftState& srcState);
private:
    HState orbitState_{};                ///< 轨道状态
};


using ScState = SpacecraftState;

/*! @} */

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
AST_NAMESPACE_END
