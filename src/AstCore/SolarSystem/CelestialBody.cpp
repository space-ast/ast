///
/// @file      CelestialBody.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "AstCore/NoopOrientation.hpp"
#include "AstCore/RotationalData.hpp"
#include "AstCore/EarthOrientation.hpp"
#include "AstCore/MoonOrientation.hpp"
#include "AstCore/BodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/BodyEphemerisNoop.hpp"
#include "AstCore/FrameICRF.hpp"
#include "AstCore/FrameAssembly.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"



AST_NAMESPACE_BEGIN


CelestialBody::CelestialBody()
    : CelestialBody(StringView{})
{
    
}

CelestialBody::CelestialBody(SolarSystem *solarSystem)
    : CelestialBody(StringView{}, solarSystem)
{
}

CelestialBody::CelestialBody(StringView name, SolarSystem *solarSystem)
    : solarSystem_(solarSystem)
    , name_{name}
{
    orientation_  = new NoopOrientation();
    ephemeris_    = new BodyEphemerisDE(this);
    axesFixed_    = AxesBodyFixed::New(this);
    axesInertial_ = AxesBodyInertial::New(this);
    axesMOD_      = AxesBodyMOD::New(this);
    axesTOD_      = AxesBodyTOD::New(this);
}

CelestialBody::~CelestialBody()
{
}

void CelestialBody::setJplIndex(int index)
{
    jplIndex_ = index;
    if(auto de =  dynamic_cast<BodyEphemerisDE*>(ephemeris_.get())){
        de->setJplIndex(index);
    }
}

std::string CelestialBody::getDirpath() const
{
    if(auto ss = solarSystem_.get()){
        return fs::path(ss->getDirpath()) / name_;
    }
    return std::string();
}

SolarSystem *CelestialBody::getSolarSystem() const
{
    return solarSystem_.get();
}

errc_t CelestialBody::load(StringView filepath)
{
    fs::path path = std::string(filepath);
    if(!fs::is_regular_file(path))
    {
        path = path / (path.filename().string() + ".cb");
        if(!fs::is_regular_file(path))
        {
            return eErrorInvalidFile;
        }
    }
    BKVParser parser(path.string());
    if(!parser.isOpen())
    {
        aError("failed to open file %s", path.string().c_str());
        return eErrorInvalidFile;
    }
    BKVItemView item;
    BKVParser::EToken token;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "Name")){
                name_ = item.value().toString();
            }
            else if(aEqualsIgnoreCase(item.key(), "ReadOnly"))
            {
                // todo
            }
        }else if(token == BKVParser::eBlockBegin){
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition")){
                errc_t rc = loadAstroDefinition(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "SpinData")){
                errc_t rc = loadSpinData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "EphemerisData")){
                errc_t rc = loadEphemerisData(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Earth")){
                errc_t rc = loadEarth(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "Moon")){
                errc_t rc = loadMoon(parser);
                if(rc) return rc;
            }else if(aEqualsIgnoreCase(item.value(), "MeanEarthDefinition")){
                errc_t rc = loadMeanEarthDefinition(parser);
                if(rc) return rc;
            }
        }else if(token == BKVParser::eBlockEnd){

        }
    }while(token != BKVParser::eEOF);

    return eNoError;
}

errc_t CelestialBody::setGravityModel(StringView model)
{
    errc_t rc = this->loadGravityModel(model);
    if(rc){
        auto ss = getSolarSystem();
        fs::path filepath;
        if(ss)
            filepath = ss->getDirpath();
        else
            filepath = SolarSystem::defaultSolarSystemDir();
        filepath = filepath / this->name_ / std::string(model);
        rc = this->loadGravityModel(filepath.string());
        if(rc){
            aError("failed to load gravity model '%.*s'", (int)model.size(), model.data());
        }
    }
    return rc;
}

errc_t CelestialBody::getPosICRF(const TimePoint& tp, Vector3d &pos) const
{
    return ephemeris_->getPosICRF(tp, pos);
}

errc_t CelestialBody::getPosVelICRF(const TimePoint& tp, Vector3d &pos, Vector3d &vel) const
{
    return ephemeris_->getPosVelICRF(tp, pos, vel);
}

Frame *CelestialBody::getFrame() const
{
    return aFrameICRF();
}

errc_t CelestialBody::getPos(const TimePoint &tp, Vector3d &pos) const
{
    return getPosICRF(tp, pos);
}

errc_t CelestialBody::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    return getPosVelICRF(tp, pos, vel);
}

Axes *CelestialBody::getAxes(StringView name) const
{
    /// @todo 这里考虑使用哈希表来存储映射关系
    if(aEqualsIgnoreCase(name, "Inertial"))
        return axesInertial_.get();
    else if(aEqualsIgnoreCase(name, "Fixed"))
        return axesFixed_.get();
    else if(aEqualsIgnoreCase(name, "MOD"))
        return axesMOD_.get();
    else if(aEqualsIgnoreCase(name, "TOD"))
        return axesTOD_.get();
    else if(aEqualsIgnoreCase(name, "TrueOfDate"))
        return axesTOD_.get();
    else if(aEqualsIgnoreCase(name, "MeanOfDate"))
        return axesMOD_.get();
    else {
        // 尝试从全局哈希表中获取轴系
        auto axes = aGetAxes(name);
        if(!axes)
            aWarning("unsupported axes name '%.*s'", (int)name.size(), name.data());
        return axes;
    }
}

HFrame CelestialBody::makeEpochFrame(Axes *sourceAxes, const TimePoint &tp, Axes *reference) const
{
    return makeFrame(aMakeEpochAxes(sourceAxes, tp, reference));
}

HFrame CelestialBody::makeEpochFrame(Axes *sourceAxes, EventTime *time, Axes *reference) const
{
    return makeFrame(aMakeEpochAxes(sourceAxes, time, reference));
}

HAxes CelestialBody::makeAxesMOE(const TimePoint &tp) const
{
    return makeEpochAxes(getAxesMOD(), tp, getEpochAxesReference());
}

HAxes CelestialBody::makeAxesMOE(EventTime *time) const
{
    return makeEpochAxes(getAxesMOD(), time, getEpochAxesReference());
}

HAxes CelestialBody::makeAxesTOE(const TimePoint &tp) const
{
    return makeEpochAxes(getAxesTOD(), tp, getEpochAxesReference());
}

HAxes CelestialBody::makeAxesTOE(EventTime *time) const
{
    return makeEpochAxes(getAxesTOD(), time, getEpochAxesReference());
}

HFrame CelestialBody::makeFrameMOE(const TimePoint &tp) const
{
    return makeFrame(makeAxesMOE(tp));
}

HFrame CelestialBody::makeFrameMOE(EventTime *time) const
{
    return makeFrame(makeAxesMOE(time));
}

HFrame CelestialBody::makeFrameTOE(const TimePoint &tp) const
{
    return makeFrame(makeAxesTOE(tp));
}

HFrame CelestialBody::makeFrameTOE(EventTime *time) const
{
    return makeFrame(makeAxesTOE(time));
}

HFrame CelestialBody::makeFrame(Axes *axes) const
{
    return new FrameAssembly(getPointCenter(), axes);
}

HFrame CelestialBody::makeFrameInertial() const
{
    return makeFrame(getAxesInertial());
}

HFrame CelestialBody::makeFrameFixed() const
{
    return makeFrame(getAxesFixed());
}

HFrame CelestialBody::makeFrameMOD() const
{
    return makeFrame(getAxesMOD());
}

HFrame CelestialBody::makeFrameTOD() const
{
    return makeFrame(getAxesTOD());
}

HFrame CelestialBody::makeFrameJ2000() const
{
    return makeFrame(aAxesJ2000());
}

HFrame CelestialBody::makeFrameICRF() const
{
    return makeFrame(aAxesICRF());
}

Axes *CelestialBody::getEpochAxesReference() const
{
    if(this->isEarth()){
        return aAxesJ2000();
    }else{
        return this->getAxesInertial();
    }
}

errc_t CelestialBody::loadGravityModel(StringView model)
{
    return gravityField_.load(model, 6, 6, getDirpath());
}

errc_t CelestialBody::loadAstroDefinition(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "GravityModel")){
                std::string model = item.value().toString();
                fs::path filepath = parser.getFilePath();
                /*!
                @bug
                如果model 被恶意写成 ../../../../etc/passwd，
                拼接后就成了 /home/user/project/configs/../../../../etc/passwd，
                经过路径解析后可能指向 /etc/passwd，从而读取系统关键文件。
                */
                filepath = filepath.parent_path() / model;
                errc_t rc = this->loadGravityModel(filepath.string());
                if(rc) return rc;
            }
            else if(aEqualsIgnoreCase(item.key(), "Gm")){
                gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "SystemGM")){
                systemGM_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "ShapeName")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "Shape")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "Radius")){
                // todo
                radius_ = item.value().toDouble();
            }
            else if(aEqualsIgnoreCase(item.key(), "MajorAxis")){
                // todo
                radius_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "MiddleAxis")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MinorAxis")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MaxAltitude")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "RefDistance")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "MinRadius")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "DTEDSearchScale")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "ParentName")){
                // todo 
            }else if(aEqualsIgnoreCase(item.key(), "PathGenerator")){
                // todo
            }
        }else if(token == BKVParser::eBlockBegin){

        }else if(token == BKVParser::eBlockEnd){
            if(aEqualsIgnoreCase(item.value(), "AstroDefinition")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadSpinData(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "RotationDefinitionFile")){
                auto rotData = new RotationalData();
                std::string model = item.value().toString();
                fs::path filepath = parser.getFilePath();
                filepath = filepath.parent_path() / model;
                errc_t rc = rotData->load(filepath.string());
                this->orientation_ = rotData;
                if(rc) return rc;
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "SpinData")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadEphemerisData(BKVParser & parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "EphemerisSource")){
                if(aEqualsIgnoreCase(item.value(), "JplDe")){
                    ephemeris_ = new BodyEphemerisDE(jplIndex_);
                }else if(aEqualsIgnoreCase(item.value(), "JplSpice")){
                    auto ephemerisSPK = new BodyEphemerisSPK(jplSpiceId_);
                    std::string spkDir = aGetConfigValue("SPK_DIR").toString();
                    if(spkDir.empty())
                        spkDir = aGetDefaultSPKDir();
                    std::string spkFile = spkDir + "/" + aAsciiStrToLower(name_) + ".bsp";
                    if(fs::is_regular_file(spkFile)){
                        errc_t rc = ephemerisSPK->openSPKFile(spkFile);
                        if(rc){
                            aWarning("failed to open SPK file '%s'", spkFile.c_str());
                        }
                    }
                    ephemeris_ = ephemerisSPK;
                }
            }else if(aEqualsIgnoreCase(item.key(), "JplSpiceId")){
                jplSpiceId_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "JplIndex")){
                jplIndex_ = item.value().toInt();
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "EphemerisData")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadEarth(BKVParser &parser)
{
    this->orientation_ = new EarthOrientation();

    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "UseFK5IAU76Theory")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ICRFTheory")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ICRF_XYS_Algorithm")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "NutationUpdateInterval")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ApplyPoleWander")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "OceanTideFilename")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "IAU1980NUTATIONMETHOD")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "UseUpdatedEquationOfEquinox")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "UtmReferenceEllipsoid")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Earth")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadMoon(BKVParser &parser)
{
    this->orientation_ = new MoonOrientation();

    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "FixedFrame")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "FixedFrameForGravity")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "Moon")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

errc_t CelestialBody::loadMeanEarthDefinition(BKVParser &parser)
{
    BKVParser::EToken token;
    BKVItemView item;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eKeyValue){
            if(aEqualsIgnoreCase(item.key(), "DeNum")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "XAngle")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "YAngle")){
                // todo
            }else if(aEqualsIgnoreCase(item.key(), "ZAngle")){
                // todo
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            if(aEqualsIgnoreCase(item.value(), "MeanEarthDefinition")){
                break;
            }
        }
    }while(token != BKVParser::eEOF);
    return eNoError;
}

AST_NAMESPACE_END

