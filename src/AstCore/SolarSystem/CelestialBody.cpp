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
#include "AstCore/NoopShape.hpp"
#include "AstCore/NoopOrientation.hpp"
#include "AstCore/BodyEphemerisDE.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/BodyEphemerisNoop.hpp"
#include "AstCore/FrameICRF.hpp"
#include "AstCore/FrameAssembly.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/Resolve.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/I18n.hpp"         // for AST_TR_NOOP


AST_NAMESPACE_BEGIN


errc_t aGetGravityParameter(const Body& body, StringView gravityModel, double& gm)
{
    GravityFieldHead gfHead;
    errc_t err = gfHead.load(gravityModel, body.getDirpath());
    if(err != eNoError){
        aError(_("从文件加载重力场头失败：'%.*s'"), (int)gravityModel.size(), gravityModel.data());
        gm = 0;
        return err;
    }
    gm = gfHead.getGM();
    return eNoError;
}


CelestialBody* CelestialBody::Resolve(StringView value)
{
    (void)AST_TR_NOOP("SolarSystemBarycenter");
    (void)AST_TR_NOOP("EarthMoonBarycenter");
    (void)AST_TR_NOOP("Mercury");
    (void)AST_TR_NOOP("Venus");
    (void)AST_TR_NOOP("Earth");
    (void)AST_TR_NOOP("Mars");
    (void)AST_TR_NOOP("Jupiter");
    (void)AST_TR_NOOP("Saturn");
    (void)AST_TR_NOOP("Uranus");
    (void)AST_TR_NOOP("Neptune");
    (void)AST_TR_NOOP("Pluto");
    (void)AST_TR_NOOP("Moon");
    (void)AST_TR_NOOP("Sun");

    // 火星卫星
    (void)AST_TR_NOOP("Phobos");
    (void)AST_TR_NOOP("Deimos");

    // 木星卫星
    (void)AST_TR_NOOP("Io");
    (void)AST_TR_NOOP("Europa");
    (void)AST_TR_NOOP("Ganymede");
    (void)AST_TR_NOOP("Callisto");
    (void)AST_TR_NOOP("Amalthea");
    (void)AST_TR_NOOP("Himalia");
    (void)AST_TR_NOOP("Elara");
    (void)AST_TR_NOOP("Pasiphae");
    (void)AST_TR_NOOP("Sinope");
    (void)AST_TR_NOOP("Lysithea");
    (void)AST_TR_NOOP("Carme");
    (void)AST_TR_NOOP("Ananke");
    (void)AST_TR_NOOP("Leda");
    (void)AST_TR_NOOP("Thebe");
    (void)AST_TR_NOOP("Adrastea");
    (void)AST_TR_NOOP("Metis");

    // 土星卫星
    (void)AST_TR_NOOP("Mimas");
    (void)AST_TR_NOOP("Enceladus");
    (void)AST_TR_NOOP("Tethys");
    (void)AST_TR_NOOP("Dione");
    (void)AST_TR_NOOP("Rhea");
    (void)AST_TR_NOOP("Titan");
    (void)AST_TR_NOOP("Hyperion");
    (void)AST_TR_NOOP("Iapetus");
    (void)AST_TR_NOOP("Phoebe");
    (void)AST_TR_NOOP("Janus");
    (void)AST_TR_NOOP("Epimetheus");
    (void)AST_TR_NOOP("Helene");
    (void)AST_TR_NOOP("Telesto");
    (void)AST_TR_NOOP("Calypso");
    (void)AST_TR_NOOP("Atlas");
    (void)AST_TR_NOOP("Prometheus");
    (void)AST_TR_NOOP("Pandora");
    (void)AST_TR_NOOP("Pan");
    (void)AST_TR_NOOP("Methone");
    (void)AST_TR_NOOP("Pallene");
    (void)AST_TR_NOOP("Polydeuces");
    (void)AST_TR_NOOP("Daphnis");
    (void)AST_TR_NOOP("Anthe");
    (void)AST_TR_NOOP("Aegaeon");

    // 天王星卫星
    (void)AST_TR_NOOP("Ariel");
    (void)AST_TR_NOOP("Umbriel");
    (void)AST_TR_NOOP("Titania");
    (void)AST_TR_NOOP("Oberon");
    (void)AST_TR_NOOP("Miranda");
    (void)AST_TR_NOOP("Cordelia");
    (void)AST_TR_NOOP("Ophelia");
    (void)AST_TR_NOOP("Bianca");
    (void)AST_TR_NOOP("Cressida");
    (void)AST_TR_NOOP("Desdemona");
    (void)AST_TR_NOOP("Juliet");
    (void)AST_TR_NOOP("Portia");
    (void)AST_TR_NOOP("Rosalind");
    (void)AST_TR_NOOP("Belinda");
    (void)AST_TR_NOOP("Puck");

    // 海王星卫星
    (void)AST_TR_NOOP("Triton");
    (void)AST_TR_NOOP("Nereid");
    (void)AST_TR_NOOP("Naiad");
    (void)AST_TR_NOOP("Thalassa");
    (void)AST_TR_NOOP("Despina");
    (void)AST_TR_NOOP("Galatea");
    (void)AST_TR_NOOP("Larissa");
    (void)AST_TR_NOOP("Proteus");

    // 冥王星卫星
    (void)AST_TR_NOOP("Charon");

    // 行星质心
    (void)AST_TR_NOOP("MercuryBarycenter");
    (void)AST_TR_NOOP("VenusBarycenter");
    (void)AST_TR_NOOP("MarsBarycenter");
    (void)AST_TR_NOOP("JupiterBarycenter");
    (void)AST_TR_NOOP("SaturnBarycenter");
    (void)AST_TR_NOOP("UranusBarycenter");
    (void)AST_TR_NOOP("NeptuneBarycenter");
    (void)AST_TR_NOOP("PlutoBarycenter");

    return aResolveBody(value);
}

CelestialBody::CelestialBody()
{
    shape_        = new NoopShape();
    orientation_  = new NoopOrientation();
    ephemeris_    = new BodyEphemerisDE(this);
    axesFixed_    = AxesBodyFixed::New(this);
    axesInertial_ = AxesBodyInertial::New(this);
    axesMOD_      = AxesBodyMOD::New(this);
    axesTOD_      = AxesBodyTOD::New(this);
}

CelestialBody::CelestialBody(SolarSystem *solarSystem)
    : CelestialBody()
{
    solarSystem_ = solarSystem;
    this->setParentScope(solarSystem);
}

CelestialBody::CelestialBody(CelestialBody *parentBody)
    : CelestialBody()
{
    parent_ = parentBody;
    this->setParentScope(parentBody);
    if (parentBody) {
        solarSystem_ = parentBody->getSolarSystem();
    }
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
        return fs::path(ss->getDirpath()) / name();
    }
    return std::string();
}

SolarSystem *CelestialBody::getSolarSystem() const
{
    return solarSystem_.get();
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
        filepath = filepath / this->name() / std::string(model);
        rc = this->loadGravityModel(filepath.string());
        if(rc){
            aError(_("加载重力场模型 '%.*s' 失败"), (int)model.size(), model.data());
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

BodyEphemeris* CelestialBody::getEphemeris(EEphemerisSource ephemerisSource) const
{
    switch(ephemerisSource){
    case EEphemerisSource::eBodyEphemeris:
        return ephemeris_.get();
    case EEphemerisSource::eJplDE:
    {
        if(!ephemerisDE_)
        {
            ephemerisDE_ = new BodyEphemerisDE(const_cast<CelestialBody*>(this));
        }
        return ephemerisDE_.get();
    }
    case EEphemerisSource::eJplSpice:
    {
        if(!ephemerisSpice_)
        {
            ephemerisSpice_ = new BodyEphemerisSPK(const_cast<CelestialBody*>(this));
        }
        return ephemerisSpice_.get();
    }
    case EEphemerisSource::eJplSpiceBarycenter:
    {
        if(!ephemerisSpiceBarycenter_)
        {
            ESpiceId barycenterId = aGetPlanetBarycenterId(ESpiceId(this->jplSpiceId_));
            ephemerisSpiceBarycenter_ = new BodyEphemerisSPK(barycenterId);
        }
        return ephemerisSpiceBarycenter_.get();
    }
    default:
        aError(_("不支持的行星历来源 %d，将使用天体的默认星历"), (int)(ephemerisSource));
        return ephemeris_.get();
    }
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
            aWarning(_("不支持的轴系名称 '%.*s'"), (int)name.size(), name.data());
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


Frame *CelestialBody::getFrameInertial() const
{
    auto frame = frameInertial_.get();
    if(frame)
    {
        return frame;
    }
    else
    {
        HFrame frame = aFindChild<Frame*>(const_cast<CelestialBody*>(this), "Inertial");
        if(!frame){
            frame = makeFrameInertial();
            frame->setName("Inertial");
            frame->setParentScope(const_cast<CelestialBody*>(this));
        }
        frameInertial_ = frame;
        return frame;
    }
}

Frame *CelestialBody::getFrameFixed() const
{
    auto frame = frameFixed_.get();
    if(frame)
    {
        return frame;
    }
    else
    {
        HFrame frame = aFindChild<Frame*>(const_cast<CelestialBody*>(this), "Fixed");
        if(!frame){
            frame = makeFrameFixed();
            frame->setName("Fixed");
            frame->setParentScope(const_cast<CelestialBody*>(this));
        }
        frameFixed_ = frame;
        return frame;
    }
}


Frame *CelestialBody::getFrameICRF() const
{
    HFrame frame = aFindChild<Frame*>(const_cast<CelestialBody*>(this), "ICRF");
    if(!frame){
        frame = makeFrameICRF();
        frame->setName("ICRF");
        frame->setParentScope(const_cast<CelestialBody*>(this));
    }
    return frame;
}


Frame *CelestialBody::getFrameJ2000() const
{
    HFrame frame = aFindChild<Frame*>(const_cast<CelestialBody*>(this), "J2000");
    if(!frame){
        frame = makeFrameJ2000();
        frame->setName("J2000");
        frame->setParentScope(const_cast<CelestialBody*>(this));
    }
    return frame;
}


Axes *CelestialBody::getEpochAxesReference() const
{
    if(this->isEarth()){
        return aAxesJ2000();
    }else{
        return this->getAxesInertial();
    }
}


ESpiceId aGetPlanetBarycenterId(ESpiceId planetId)
{
    std::div_t result = std::div(planetId, 100);
    if(result.rem == 99 && result.quot < 10)
    {
        return static_cast<ESpiceId>(result.quot);
    }
    return planetId;
}

AST_NAMESPACE_END

