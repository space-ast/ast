///
/// @file      ObjectLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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

#include "ObjectLoader.hpp"
#include "AstCore/CelestialBodyLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/BKVParser.hpp"

#include "AstSim/Mover.hpp"
#include "AstSim/Spacecraft.hpp"
#include "AstSim/Satellite.hpp"
#include "AstSim/Aircraft.hpp"
#include "AstSim/GroundVehicle.hpp"
#include "AstSim/LaunchVehicle.hpp"
#include "AstSim/Missile.hpp"
#include "AstSim/Ship.hpp"
#include "AstSim/Submarine.hpp"
#include "AstLoader/MoverLoader.hpp"
#include "AstSim/Facility.hpp"
#include "AstLoader/FacilityLoader.hpp"
#include "AstSim/LineTarget.hpp"
#include "AstSim/MTO.hpp"
#include "AstSim/Place.hpp"
#include "AstSim/Planet.hpp"
#include "AstSim/Star.hpp"
#include "AstSim/Target.hpp"
#include "AstSim/Volumetric.hpp"
#include "AstSim/AdvCAT.hpp"
#include "AstSim/AreaTarget.hpp"
#include "AstSim/Chain.hpp"
#include "AstSim/CommSystem.hpp"
#include "AstSim/Constellation.hpp"
#include "AstSim/CoverageDefinition.hpp"
#include "AstSim/Sensor.hpp"
#include "AstSim/AttitudeCoverage.hpp"
#include "AstSim/Radar.hpp"
#include "AstSim/Antenna.hpp"
#include "AstSim/Receiver.hpp"
#include "AstSim/FigureOfMerit.hpp"
#include "AstSim/Transmitter.hpp"
#include "AstSim/AttitudeFigureOfMerit.hpp"
#include "AstSim/Scenario.hpp"


#include "LineTargetLoader.hpp"
#include "MTOLoader.hpp"
#include "PlaceLoader.hpp"
#include "PlanetLoader.hpp"
#include "StarLoader.hpp"
#include "TargetLoader.hpp"
#include "VolumetricLoader.hpp"
#include "AdvCATLoader.hpp"
#include "AreaTargetLoader.hpp"
#include "ChainLoader.hpp"
#include "CommSystemLoader.hpp"
#include "ConstellationLoader.hpp"
#include "CoverageDefinitionLoader.hpp"
#include "SensorLoader.hpp"
#include "AttitudeCoverageLoader.hpp"
#include "RadarLoader.hpp"
#include "AntennaLoader.hpp"
#include "ReceiverLoader.hpp"
#include "FigureOfMeritLoader.hpp"
#include "TransmitterLoader.hpp"
#include "AttitudeFigureOfMeritLoader.hpp"
#include "ScenarioLoader.hpp"

AST_NAMESPACE_BEGIN

// errc_t aLoadObject(StringView filepath, SharedPtr<Object> &object)
// {
//     
// }


errc_t aLoadObject(StringView filepath, StringView objectType, SharedPtr<Object> &object)
{
    if(aEqualsIgnoreCase(objectType, "Satellite"))
    {
        auto satellite = new Satellite();
        object = satellite;
        return aLoadMover(filepath, *satellite);
    }
    else if(aEqualsIgnoreCase(objectType, "Spacecraft"))
    {
        auto spacecraft = new Spacecraft();
        object = spacecraft;
        return aLoadMover(filepath, *spacecraft);
    }
    else if(aEqualsIgnoreCase(objectType, "Aircraft"))
    {
        auto aircraft = new Aircraft();
        object = aircraft;
        return aLoadMover(filepath, *aircraft);
    }
    else if(aEqualsIgnoreCase(objectType, "GroundVehicle"))
    {
        auto groundVehicle = new GroundVehicle();
        object = groundVehicle;
        return aLoadMover(filepath, *groundVehicle);
    }
    else if(aEqualsIgnoreCase(objectType, "LaunchVehicle"))
    {
        auto launchVehicle = new LaunchVehicle();
        object = launchVehicle;
        return aLoadMover(filepath, *launchVehicle);
    }
    else if(aEqualsIgnoreCase(objectType, "Missile"))
    {
        auto missile = new Missile();
        object = missile;
        return aLoadMover(filepath, *missile);
    }
    else if(aEqualsIgnoreCase(objectType, "Ship"))
    {
        auto ship = new Ship();
        object = ship;
        return aLoadMover(filepath, *ship);
    }
    else if(aEqualsIgnoreCase(objectType, "Submarine"))
    {
        auto submarine = new Submarine();
        object = submarine;
        return aLoadMover(filepath, *submarine);
    }
    else if(aEqualsIgnoreCase(objectType, "Facility"))
    {
        auto facility = new Facility();
        object = facility;
        return aLoadFacility(filepath, *facility);
    }
    else if(aEqualsIgnoreCase(objectType, "LineTarget"))
    {
        auto lineTarget = new LineTarget();
        object = lineTarget;
        return aLoadLineTarget(filepath, *lineTarget);
    }
    else if(aEqualsIgnoreCase(objectType, "MTO"))
    {
        auto mto = new MTO();
        object = mto;
        return aLoadMTO(filepath, *mto);
    }
    else if(aEqualsIgnoreCase(objectType, "Place"))
    {
        auto place = new Place();
        object = place;
        return aLoadPlace(filepath, *place);
    }
    else if(aEqualsIgnoreCase(objectType, "Planet"))
    {
        auto planet = new Planet();
        object = planet;
        return aLoadPlanet(filepath, *planet);
    }
    else if(aEqualsIgnoreCase(objectType, "Star"))
    {
        auto star = new Star();
        object = star;
        return aLoadStar(filepath, *star);
    }
    else if(aEqualsIgnoreCase(objectType, "Target"))
    {
        auto target = new Target();
        object = target;
        return aLoadTarget(filepath, *target);
    }
    else if(aEqualsIgnoreCase(objectType, "Volumetric"))
    {
        auto volumetric = new Volumetric();
        object = volumetric;
        return aLoadVolumetric(filepath, *volumetric);
    }
    else if(aEqualsIgnoreCase(objectType, "AdvCAT"))
    {
        auto advCAT = new AdvCAT();
        object = advCAT;
        return aLoadAdvCAT(filepath, *advCAT);
    }
    else if(aEqualsIgnoreCase(objectType, "AreaTarget"))
    {
        auto areaTarget = new AreaTarget();
        object = areaTarget;
        return aLoadAreaTarget(filepath, *areaTarget);
    }
    else if(aEqualsIgnoreCase(objectType, "Chain"))
    {
        auto chain = new Chain();
        object = chain;
        return aLoadChain(filepath, *chain);
    }
    else if(aEqualsIgnoreCase(objectType, "CommSystem"))
    {
        auto commSystem = new CommSystem();
        object = commSystem;
        return aLoadCommSystem(filepath, *commSystem);
    }
    else if(aEqualsIgnoreCase(objectType, "Constellation"))
    {
        auto constellation = new Constellation();
        object = constellation;
        return aLoadConstellation(filepath, *constellation);
    }
    else if(aEqualsIgnoreCase(objectType, "CoverageDefinition"))
    {
        auto coverageDefinition = new CoverageDefinition();
        object = coverageDefinition;
        return aLoadCoverageDefinition(filepath, *coverageDefinition);
    }
    else if(aEqualsIgnoreCase(objectType, "Sensor"))
    {
        auto sensor = new Sensor();
        object = sensor;
        return aLoadSensor(filepath, *sensor);
    }
    else if(aEqualsIgnoreCase(objectType, "AttitudeCoverage"))
    {
        auto attitudeCoverage = new AttitudeCoverage();
        object = attitudeCoverage;
        return aLoadAttitudeCoverage(filepath, *attitudeCoverage);
    }
    else if(aEqualsIgnoreCase(objectType, "Radar"))
    {
        auto radar = new Radar();
        object = radar;
        return aLoadRadar(filepath, *radar);
    }
    else if(aEqualsIgnoreCase(objectType, "Antenna"))
    {
        auto antenna = new Antenna();
        object = antenna;
        return aLoadAntenna(filepath, *antenna);
    }
    else if(aEqualsIgnoreCase(objectType, "Receiver"))
    {
        auto receiver = new Receiver();
        object = receiver;
        return aLoadReceiver(filepath, *receiver);
    }
    else if(aEqualsIgnoreCase(objectType, "FigureOfMerit"))
    {
        auto figureOfMerit = new FigureOfMerit();
        object = figureOfMerit;
        return aLoadFigureOfMerit(filepath, *figureOfMerit);
    }
    else if(aEqualsIgnoreCase(objectType, "Transmitter"))
    {
        auto transmitter = new Transmitter();
        object = transmitter;
        return aLoadTransmitter(filepath, *transmitter);
    }
    else if(aEqualsIgnoreCase(objectType, "AttitudeFigureOfMerit"))
    {
        auto attitudeFigureOfMerit = new AttitudeFigureOfMerit();
        object = attitudeFigureOfMerit;
        return aLoadAttitudeFigureOfMerit(filepath, *attitudeFigureOfMerit);
    }
    else if(aEqualsIgnoreCase(objectType, "Scenario"))
    {
        auto scenario = new Scenario();
        object = scenario;
        return aLoadScenario(filepath, *scenario);
    }
    else
    {
        aError(_("不支持的对象类型: '%.*s'"), objectType.size(), objectType.data());
        return eErrorInvalidParam;
    }
    return eNoError;
}

AST_NAMESPACE_END