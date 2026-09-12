///
/// @file      TwoBodySax.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-19
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

#include "MotionOrbitDynamicsKeplerianSax.hpp"
#include "CommonlyUsedHeaders.hpp"

AST_NAMESPACE_BEGIN


errc_t MotionOrbitDynamicsKeplerianSax::keyValue(StringView key, const ValueView &value)
{
    if(aEqualsIgnoreCase(key, "RadiusOfPerigee")){
        radiusOfPerigee_ = value.toDouble();
    }else if(aEqualsIgnoreCase(key, "Inclination")){
        inclination_ = deg2rad(value.toDouble());
    }else if(aEqualsIgnoreCase(key, "Eccentricity")){
        eccentricity_ = value.toDouble();
    }else if(aEqualsIgnoreCase(key, "ArgOfPerigee")){
        argOfPerigee_ = deg2rad(value.toDouble());
    }else if(aEqualsIgnoreCase(key, "RightAscension")){
        rightAscension_ = deg2rad(value.toDouble());
    }else if(aEqualsIgnoreCase(key, "TrueAnomaly")){
        trueAnomaly_ = deg2rad(value.toDouble());
    }else if(aEqualsIgnoreCase(key, "TimeStep")){
        timeStep_ = value.toDouble();
    }else if(aEqualsIgnoreCase(key, "OrbElemCoordSys")){
        orbElemCoordSys_ = value.toString();
        orbElemCoordAxes_ = getBody()->getAxes(orbElemCoordSys_);
        if(!orbElemCoordAxes_)
        {
            aError(_("获取轨道根数坐标系 '%.*s' 失败"), (int)orbElemCoordSys_.size(), orbElemCoordSys_.data());
        }
    }else if(aEqualsIgnoreCase(key, "PropagationCoordSys")){
        propagationCoordSys_ = value.toString();
        propagationCoordAxes_ = getBody()->getAxes(propagationCoordSys_);
        if(!propagationCoordAxes_)
        {
            aError(_("获取预报坐标系 '%.*s' 失败"), (int)propagationCoordSys_.size(), propagationCoordSys_.data());
        }
    }else if(aEqualsIgnoreCase(key, "DisplayCoordType")){
        displayCoordType_ = value.toInt();
    }else if(aEqualsIgnoreCase(key, "DisplayCoordSys")){
        displayCoordSys_ = value.toString();
        displayCoordAxes_ = getBody()->getAxes(displayCoordSys_);
        if(!displayCoordAxes_)
        {
            aError(_("获取展示坐标系 '%.*s' 失败"), (int)displayCoordSys_.size(), displayCoordSys_.data());
        }
    }else if(aEqualsIgnoreCase(key, "EllipseType")){
        // @todo 这是什么??? 椭圆类型是什么意思???
    }
    // 其他公共参数
    return MotionOrbitDynamicsSax::keyValue(key, value);
}

errc_t MotionOrbitDynamicsKeplerianSax::getMotion(ScopedPtr<MotionProfile>& motion)
{
    if(vehiclePathData_.centralBody_ == nullptr){
        aError(_("天体为空"));
        return eErrorNullPtr;
    }
    if(!orbElemCoordAxes_)
    {
        aError(_("轨道根数坐标系为空"));
        return eErrorNullPtr;
    }
    if(!propagationCoordAxes_)
    {
        aError(_("预报坐标系为空"));
        return eErrorNullPtr;
    }
    // 这里可以通过类型来判断是否需要新建MotionProfile
    auto motionOrbitDynamics = newMotionOrbitDynamics();
    auto body = vehiclePathData_.centralBody_; AST_CHECK_NULLPTR(body);
    ModOrbElem modOrbElem{};
    modOrbElem.rp() = radiusOfPerigee_;
    modOrbElem.e() = eccentricity_;
    modOrbElem.i() = inclination_;
    modOrbElem.raan() = rightAscension_;
    modOrbElem.argper() = argOfPerigee_;
    modOrbElem.trueA() = trueAnomaly_;
    auto frame = body->makeFrame(orbElemCoordAxes_);
    SharedPtr<StateKeplerian> stateKeplerian = StateKeplerian::New();  // 一定是轨道根数吗??
    stateKeplerian->setFrame(frame);
    stateKeplerian->setState(modOrbElem);
    stateKeplerian->setStateEpoch(makeStateEpoch());
    motionOrbitDynamics->setInitialState(stateKeplerian);
    motionOrbitDynamics->setInterval(makeInterval());
    motionOrbitDynamics->setPropagationFrame(body->makeFrame(propagationCoordAxes_));

    motion = motionOrbitDynamics;
    return eNoError;
}

AST_NAMESPACE_END


