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


AST_NAMESPACE_BEGIN


CelestialBody* CelestialBody::Resolve(StringView value)
{
    return aResolveBody(value);
}

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
    if (solarSystem)
        setParentScope(solarSystem);

    shape_        = new NoopShape();
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

Axes *CelestialBody::getEpochAxesReference() const
{
    if(this->isEarth()){
        return aAxesJ2000();
    }else{
        return this->getAxesInertial();
    }
}

AST_NAMESPACE_END

