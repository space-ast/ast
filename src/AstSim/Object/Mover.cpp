///
/// @file      Mover.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "Mover.hpp"
#include "AstUtil/Class.hpp"
#include "AstSim/MotionProfile.hpp"
#include "AstSim/MotionOrbitDynamics.hpp"


AST_NAMESPACE_BEGIN

errc_t Mover::generateEphemeris()
{
    if(!motionProfile_)
        return eErrorNullPtr;
    errc_t rc = motionProfile_->makeEphemerisSimple(ephemeris_);
    if(rc)
        motionProfile_->makeEphemerisSpec(ephemeris_);
    return rc;
}

/*
采用委托模式（Delegate Pattern）实现运行时动态切换星历实现

如果c++有类似下面的语法糖就好了，类似于Kolin的语法糖
class Mover implements Point by ephemeris_
*/

Frame *Mover::getFrame() const
{
    return ephemeris_->getFrame();
}

errc_t Mover::getPos(const TimePoint &tp, Vector3d &pos) const
{
    return ephemeris_->getPos(tp, pos);
}

errc_t Mover::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    return ephemeris_->getPosVel(tp, pos, vel);
}

void Mover::setMotionProfile(MotionProfile* profile)
{
    if(!profile)
        return;
    if(profile->getParentScope() == nullptr)
        profile->setParentScope(this);
    motionProfile_ = profile;
}

State* Mover::getInitialState() const
{
    auto profile = motionProfile_.get();
    if(!profile)
        return nullptr;
    if(motionProfile_->isOfType(MotionOrbitDynamics::StaticType()))
    {
        return static_cast<MotionOrbitDynamics*>(profile)->getInitialState();
    }
    return nullptr;
}

AST_NAMESPACE_END