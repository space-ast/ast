///
/// @file      ForceModelLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-21
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

#include "ForceModelLoader.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/FileSystem.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadGravityForce(const Value& value, GravityForce& gravityForce)
{
    std::string type = value["Type"];
    if(type != "GravityFieldFunc"){
        aError("unsupported force type '%s'", type.c_str());
        return eErrorInvalidParam;
    }
    // 基础重力场配置参数
    gravityForce.maxDegree_ = value["Degree"];
    gravityForce.maxOrder_ = value["Order"];

    // 重力场模型
    {
        std::string gravityFile = value["GravityFile"];
        // 特殊处理一下
        if(StringView(gravityFile).starts_with(R"(STKData\CentralBodies\)"))
        {
            gravityForce.model_ = fs::path(gravityFile).stem().string();
        }
        else
        {
            gravityForce.model_ = gravityFile;
        }
    }
    gravityForce.useSecularVariations_ = value["UseSecularVariations"];

    // 固体潮汐配置参数
    gravityForce.truncateSolidTides_ = value["TruncateSolidTides"];
    gravityForce.includeTimeDependentSolidTides_ = value["IncludeTimeDependentSolidTides"];
    gravityForce.minAmplitudeSolidTides_ = value["SolidTideMinAmp"];
    std::string solidTideType = value["SolidTideType"];
    if(solidTideType == "Permanent tide only")
        gravityForce.solidTideType_ = ESolidTideType::ePermanentOnly;
    else if(solidTideType == "None")
        gravityForce.solidTideType_ = ESolidTideType::eNone;
    else if(solidTideType == "Full tide")
        gravityForce.solidTideType_ = ESolidTideType::eFull;
    else{
        aError("unsupported solid tide type '%s', default to None", solidTideType.c_str());
        gravityForce.solidTideType_ = ESolidTideType::eNone;
    }

    // 海洋潮汐配置参数
    gravityForce.useOceanTides_ = value["UseOceanTides"];
    gravityForce.maxDegreeOceanTides_ = value["OceanTideMaxDegree"];
    gravityForce.maxOrderOceanTides_ = value["OceanTideMaxOrder"];
    gravityForce.minAmplitudeOceanTides_ = value["OceanTideMinAmp"];
    
    return eNoError;
}


errc_t aLoadPointMassForce(const Value& value, PointMassForce& pointMassForce)
{
    std::string type = value["Type"];
    if(type != "TwoBodyFunc" && type != "ThirdBodyFunc"){
        aError("unsupported force type '%s'", type.c_str());
        return eErrorInvalidParam;
    }
    pointMassForce.specifiedGM_ = value["Mu"];
    const std::string gravSource = value["GravSource"];
    if(gravSource == "Cb File")
        pointMassForce.gmSource_ = EGMSource::eBodyGravity;
    else if(gravSource == "DE File")
        pointMassForce.gmSource_ = EGMSource::eJplDE;
    else if(gravSource == "User Specified")
        pointMassForce.gmSource_ = EGMSource::eSpecifiedValue;
    else{
        aWarning("unsupported grav source '%s', default to body gravity", gravSource.c_str());
        pointMassForce.gmSource_ = EGMSource::eBodyGravity;
    }
    return eNoError;
}

errc_t aLoadThirdBodyForce(const Value& value, ThirdBodyForce& thirdBodyForce)
{
    std::string type = value["Type"];
    if(type != "ThirdBodyFunc"){
        aError("unsupported force type '%s'", type.c_str());
        return eErrorInvalidParam;
    }

    // 天体
    const std::string bodyName = value["ThirdBody"];
    auto body = aGetBody(bodyName);
    if(body)
        thirdBodyForce.setBody(body);
    else
    {
        aError("third body '%s' not found", bodyName.c_str());
    }

    // 三体星历来源
    const std::string ephemerisSource = value["EphemerisSource"];
    if(ephemerisSource == "Cb File")
        thirdBodyForce.setEphemerisSource(EEphemerisSource::eBodyEphemeris);
    else if(ephemerisSource == "DE File")
        thirdBodyForce.setEphemerisSource(EEphemerisSource::eJplDE);
    else if(ephemerisSource == "SPICE Body Centered")
        thirdBodyForce.setEphemerisSource(EEphemerisSource::eJplSpice);
    else{
        aWarning("unsupported ephemeris source '%s', default to Body Ephemeris", ephemerisSource.c_str());
        thirdBodyForce.setEphemerisSource(EEphemerisSource::eBodyEphemeris);
    }

    // 三体引力模式
    std::string mode = value["Mode"];
    EBodyAttractionType attractionType = EBodyAttractionType::eUnknown;
    if(mode == "Point Mass")
    {
        attractionType = EBodyAttractionType::ePointMass;
    }
    else if(mode == "Gravity Field")
    {
        attractionType = EBodyAttractionType::eGravity;
    }
    else
    {
        aWarning("unsupported attraction type '%s', default to point mass", mode.c_str());
        attractionType = EBodyAttractionType::ePointMass;
    }
    thirdBodyForce.setAttractionType(attractionType);

    // 引力场模式的配置参数
    errc_t rc = aLoadGravityForce(value["GravityField"], thirdBodyForce.gravity());
    if(rc != eNoError)
    {
        aWarning("failed to load gravity force model");
    }
    
    // 点质量模式的配置参数
    rc = aLoadPointMassForce(value, thirdBodyForce.pointMass());
    if(rc != eNoError)
    {
        aWarning("failed to load point mass force model");
    }
    return eNoError;
}



static EAtmDensityModel _aStringToAtmDensityModel(const std::string& type)
{
    if(type == "US_Standard_Atmosphere")
        return EAtmDensityModel::e1976Standard;
    else if(type == "Harris-Priester")
        return EAtmDensityModel::eHarrisPriester;
    else if(type == "Jacchia_1960")
        return EAtmDensityModel::eJacchia60;
    else if(type == "Jacchia_1970")
        return EAtmDensityModel::eJacchia70;
    else if(type == "Jacchia_1971")
        return EAtmDensityModel::eJacchia71;
    else if(type == "Jacchia-Roberts")
        return EAtmDensityModel::eJacchiaRoberts;
    else if(type == "MSIS 1986")
        return EAtmDensityModel::eMSIS1986;
    else if(type == "MSISE 1990")
        return EAtmDensityModel::eMSISE1990;
    else if(type == "NRLMSISE 2000")
        return EAtmDensityModel::eNRLMSISE2000;
    else if(type == "Cira72Func")
        return EAtmDensityModel::eCIRA72;
    else if(type == "DTM 2012")
        return EAtmDensityModel::eDTM2012;
    else{
        aWarning("unsupported atmospheric density model type '%s', use default 'NRLMSISE 2000' model instead", type.c_str());
        return EAtmDensityModel::eNRLMSISE2000;
    }
}

static ESunPosition _aStringToSunPosition(const std::string& sunPosition)
{
    if(sunPosition == "ApparentSunToTrueCB")
        return ESunPosition::eApparentSunToTrueCB;
    else if(sunPosition == "Apparent")
        return ESunPosition::eApparent;
    else // if(sunPosition == "True")
        return ESunPosition::eTrue;
}


errc_t aLoadDragForce(const Value& value, DragForce& dragForce)
{
    std::string category = value["Category"];
    if(category != "Atmospheric Models"){
        aError("unsupported force category '%s'", category.c_str());
        return eErrorInvalidParam;
    }
    // 解析大气阻力模型
    dragForce.atmDensityModel_ = _aStringToAtmDensityModel(value["Type"]);
    dragForce.lowAltAtmDensityModel_ = EAtmDensityModel::eNone; // 默认没有这个配置项
    dragForce.atmBlendingRange_ = 0;

    // 解析太阳辐射和地磁指数
    dragForce.useFluxApFile_ = (value["AtmosDataSource"].toString() == "Data File");
    dragForce.f10p7Average_ = value["F10p7Avg"];
    dragForce.f10p7Daily_ = value["F10p7"];
    dragForce.kp_ = value["Kp"];
    
    dragForce.fluxApFile_ = std::string(value["AtmosDataFile"]);
    std::string atmosDataGeoMagFluxUpdateRate = value["AtmosDataGeoMagFluxUpdateRate"];
    if(atmosDataGeoMagFluxUpdateRate == "Daily")
        dragForce.geoMagFluxUpdateRate_ = EGeoMagFluxUpdateRate::eDaily;
    else if(atmosDataGeoMagFluxUpdateRate == "3-Hourly")
        dragForce.geoMagFluxUpdateRate_ = EGeoMagFluxUpdateRate::e3Hourly;
    else{
        aWarning("unsupported atmospheric data geo mag flux update rate '%s', default to Daily", atmosDataGeoMagFluxUpdateRate.c_str());
        dragForce.geoMagFluxUpdateRate_ = EGeoMagFluxUpdateRate::eDaily;
    }
    std::string atmosDataGeoMagFluxSource = value["AtmosDataGeoMagFluxSource"];
    if(atmosDataGeoMagFluxSource == "Read Kp from file")
        dragForce.geoMagFluxSource_ = EGeoMagFluxSource::eKp;
    else if(atmosDataGeoMagFluxSource == "Read Ap from file")
        dragForce.geoMagFluxSource_ = EGeoMagFluxSource::eAp;
    else{
        aWarning("unsupported atmospheric data geo mag flux source '%s', default to Ap", atmosDataGeoMagFluxSource.c_str());
        dragForce.geoMagFluxSource_ = EGeoMagFluxSource::eAp;
    }

    dragForce.useApproxAltForDrag_ = value["UseApproximateAltitude"];
    dragForce.sunPosition_ = _aStringToSunPosition(value["SunPosition"]);
    return eNoError;
}


errc_t aLoadSolarRadiationPressure(const Value& value, SolarRadiationPressure& srp, Body* centerBody)
{
    std::string category = value["Category"];
    if(category != "SRP Models"){
        aError("unsupported force category '%s'", category.c_str());
        return eErrorInvalidParam;
    }
    std::string shadowModel = value["ShadowModel"];
    if(shadowModel == "No Shadow")
        srp.shadowModel_ = EShadowModel::eNone;
    else if(shadowModel == "Cylindrical")
        srp.shadowModel_ = EShadowModel::eCylindrical;
    else if(shadowModel == "Dual Cone")
        srp.shadowModel_ = EShadowModel::eDualCone;
    else{
        aWarning("unsupported shadow model '%s', default to 'Dual Cone'", shadowModel.c_str());
        srp.shadowModel_ = EShadowModel::eDualCone;
    }

    srp.atmAltForEclipse_ = value["AtmosphereAlt"];
    srp.detectShadowBoundaries_ = value["IncludeBoundaryMitigation"];

    srp.sunPosition_ = _aStringToSunPosition(value["SunPosition"]);
    

    // 加载遮挡天体    
    srp.eclipsingBodies_.clear();
    if(centerBody)
        srp.eclipsingBodies_.push_back(centerBody);
    const auto& listOtherEclipsingBodies = value["OtherEclipsingBodies"];
    for(const auto& item : listOtherEclipsingBodies.items())
    {
        std::string bodyName = item.second->toString();
        auto body = aGetBody(bodyName);
        if(body)
            srp.eclipsingBodies_.push_back(body);
        else
            aWarning("eclipsing body '%s' not found, ignore it", bodyName.c_str());
    }
    return eNoError;
}

errc_t aLoadForceModel(const Value& value, HPOPForceModel& forceModel)
{
    // 加载中心体
    {
        std::string centerBody = value["CentralBody"].toString();
        auto body = aGetBody(centerBody);
        if(body)
        {
            forceModel.setCentralBody(body);
        }
        else
        {
            aError("central body '%s' not found, use default body 'Earth' instead", centerBody.c_str());
            forceModel.setCentralBody(aGetEarth());
        }
    }
    
    // 加载力模型配置参数
    const auto& items = value["PropFuncs"].items();
    for(const auto& item : items)
    {
        if(!item.second)
            continue;
        const auto& force = *item.second;
        std::string type = force["Type"];
        if(type == "GravityFieldFunc")
        {
            errc_t rc = aLoadGravityForce(force, forceModel.gravity());
            if(rc)
            {
                aWarning("failed to load gravity force");
            }
        }
        else if(type == "TwoBodyFunc")
        {
            errc_t rc = aLoadPointMassForce(force, forceModel.pointMass());
            if(rc)
            {
                aWarning("failed to load point mass force");
            }
        }
        else if(type == "ThirdBodyFunc")
        {
            ThirdBodyForce thirdBody;
            errc_t rc = aLoadThirdBodyForce(force, thirdBody);
            if(rc)
            {
                aWarning("failed to load third body force");
            }
            forceModel.addThirdBody(thirdBody);
        }
        else if(type == "GenRelativityFunc")
        {
            forceModel.useRelativityCorrection(true);
        }
        else if(type == "RadPressureFunc")
        {
            aWarning("RadPressure force is not supported");
        }
        else if(type == "YarkovskyFunc")
        {
            aWarning("Yarkovsky force is not supported");
        }
        else if(type == "StateTransFunc")
        {
            aWarning("State Transition Matrix is not supported");
        }
        else if(type == "HPOPPluginFunc")
        {
            aWarning("HPOPPlugin force is not supported");
        }
        else
        {
            std::string category = force["Category"];
            if(category == "Atmospheric Models")
            {
                errc_t rc = aLoadDragForce(force, forceModel.drag());
                if(rc)
                {
                    aWarning("failed to load drag force");
                }
            }
            else if(category == "SRP Models")
            {
                errc_t rc = aLoadSolarRadiationPressure(force, forceModel.srp(), forceModel.centralBody());
                if(rc)
                {
                    aWarning("failed to load solar radiation pressure");
                }
            }
            else
            {
                aWarning("unsupported force category '%s' and type '%s'", category.c_str(), type.c_str());
            }
        }
    }
    return eNoError;
}

AST_NAMESPACE_END
