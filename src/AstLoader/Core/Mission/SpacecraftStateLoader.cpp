///
/// @file      SpacecraftStateLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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

#include "SpacecraftStateLoader.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadSpacecraftState(const Value& value, SpacecraftState& spacecraftState)
{
    if(value.isNull()){
        aError("value is null for SpacecraftState loading");
        return eErrorInvalidParam;
    }
    HState state;
    auto& dictOrbitState = value["Orbit_State"];
    // std::string coordType = dictCoordinateType.toString();
    // if(coordType == "Cartesian")
    {
        StateCartesian* cartesian = StateCartesian::New();
        state = cartesian;
        auto& dictCartesian = dictOrbitState["Cartesian"];
        if(!dictCartesian.isNull()){
            cartesian->setX(dictCartesian["X"]);
            cartesian->setY(dictCartesian["Y"]);
            cartesian->setZ(dictCartesian["Z"]);
            cartesian->setVx(dictCartesian["Vx"]);
            cartesian->setVy(dictCartesian["Vy"]);
            cartesian->setVz(dictCartesian["Vz"]);
        }
    }
    if(state){
        TimePoint epoch = TimePoint::Parse(dictOrbitState["Epoch"].toString());
        state->setStateEpoch(epoch);
        spacecraftState.setOrbitState(state);
        std::string coordSystem = dictOrbitState["CoordSysRep"];
        state->setFrameByName(coordSystem);
    }
    spacecraftState.setCd(dictOrbitState["Cd"]);
    spacecraftState.setCr(dictOrbitState["Cr"]);
    spacecraftState.setDragArea(dictOrbitState["DragArea"]);
    spacecraftState.setSRPArea(dictOrbitState["SRPArea"]);
    spacecraftState.setDryMass(dictOrbitState["DryMass"]);
    spacecraftState.setFuelMass(dictOrbitState["FuelMass"]);
    spacecraftState.setFuelDensity(dictOrbitState["FuelDensity"]);
    spacecraftState.setK1(dictOrbitState["K1"]);
    spacecraftState.setK2(dictOrbitState["K2"]);
    spacecraftState.setRadPressureCoeff(dictOrbitState["RadPressureCoeff"]);
    spacecraftState.setRadPressureArea(dictOrbitState["RadPressureArea"]);
    spacecraftState.setTankPressure(dictOrbitState["TankPressure"]);
    spacecraftState.setTankTemperature(dictOrbitState["TankTemperature"]);
    return 0;
}

AST_NAMESPACE_END
