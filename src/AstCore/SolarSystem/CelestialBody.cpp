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
#include "AstUtil/I18n.hpp"         // for N_


AST_NAMESPACE_BEGIN


errc_t aGetGravityParameter(const Body& body, StringView gravityModel, double& gm)
{
    GravityFieldHead gfHead;
    errc_t err = gfHead.load(gravityModel, body.getDirpath());
    if(err != eNoError){
        aError("Failed to load gravity field head from file: '%.*s'", (int)gravityModel.size(), gravityModel.data());
        gm = 0;
        return err;
    }
    gm = gfHead.getGM();
    return eNoError;
}


CelestialBody* CelestialBody::Resolve(StringView value)
{
    (void)N_("SolarSystemBarycenter");
    (void)N_("EarthMoonBarycenter");
    (void)N_("Mercury");
    (void)N_("Venus");
    (void)N_("Earth");
    (void)N_("Mars");
    (void)N_("Jupiter");
    (void)N_("Saturn");
    (void)N_("Uranus");
    (void)N_("Neptune");
    (void)N_("Pluto");
    (void)N_("Moon");
    (void)N_("Sun");

    // 火星卫星
    (void)N_("Phobos");
    (void)N_("Deimos");

    // 木星卫星
    (void)N_("Io");
    (void)N_("Europa");
    (void)N_("Ganymede");
    (void)N_("Callisto");
    (void)N_("Amalthea");
    (void)N_("Himalia");
    (void)N_("Elara");
    (void)N_("Pasiphae");
    (void)N_("Sinope");
    (void)N_("Lysithea");
    (void)N_("Carme");
    (void)N_("Ananke");
    (void)N_("Leda");
    (void)N_("Thebe");
    (void)N_("Adrastea");
    (void)N_("Metis");

    // 土星卫星
    (void)N_("Mimas");
    (void)N_("Enceladus");
    (void)N_("Tethys");
    (void)N_("Dione");
    (void)N_("Rhea");
    (void)N_("Titan");
    (void)N_("Hyperion");
    (void)N_("Iapetus");
    (void)N_("Phoebe");
    (void)N_("Janus");
    (void)N_("Epimetheus");
    (void)N_("Helene");
    (void)N_("Telesto");
    (void)N_("Calypso");
    (void)N_("Atlas");
    (void)N_("Prometheus");
    (void)N_("Pandora");
    (void)N_("Pan");
    (void)N_("Methone");
    (void)N_("Pallene");
    (void)N_("Polydeuces");
    (void)N_("Daphnis");
    (void)N_("Anthe");
    (void)N_("Aegaeon");

    // 天王星卫星
    (void)N_("Ariel");
    (void)N_("Umbriel");
    (void)N_("Titania");
    (void)N_("Oberon");
    (void)N_("Miranda");
    (void)N_("Cordelia");
    (void)N_("Ophelia");
    (void)N_("Bianca");
    (void)N_("Cressida");
    (void)N_("Desdemona");
    (void)N_("Juliet");
    (void)N_("Portia");
    (void)N_("Rosalind");
    (void)N_("Belinda");
    (void)N_("Puck");

    // 海王星卫星
    (void)N_("Triton");
    (void)N_("Nereid");
    (void)N_("Naiad");
    (void)N_("Thalassa");
    (void)N_("Despina");
    (void)N_("Galatea");
    (void)N_("Larissa");
    (void)N_("Proteus");

    // 冥王星卫星
    (void)N_("Charon");

    // 行星质心
    (void)N_("MercuryBarycenter");
    (void)N_("VenusBarycenter");
    (void)N_("MarsBarycenter");
    (void)N_("JupiterBarycenter");
    (void)N_("SaturnBarycenter");
    (void)N_("UranusBarycenter");
    (void)N_("NeptuneBarycenter");
    (void)N_("PlutoBarycenter");

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
        aError("unsupported ephemeris source %d, defaulting to body ephemeris", (int)(ephemerisSource));
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

