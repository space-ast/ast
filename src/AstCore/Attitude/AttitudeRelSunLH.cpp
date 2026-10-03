///
/// @file      AttitudeRelSunLH.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-03
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

#include "AttitudeRelSunLH.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/AttitudeUtil.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

AttitudeRelSunLH::AttitudeRelSunLH(Point *point, Frame *frame)
{
    this->setPoint(point);
    this->setFrame(frame);
    this->setSun(aGetSun());
}


errc_t AttitudeRelSunLH::getSunPosLocal(const TimePoint& tp, Vector3d& sunPos) const
{
    auto frame = this->frame();
    auto sun = this->sun();
    if(!frame || !sun) return eErrorNullPtr;
    return sun->getPosIn(frame, tp, sunPos);
}

errc_t AttitudeRelSunLH::getSunPosVelLocal(const TimePoint& tp, Vector3d& sunPos, Vector3d& sunVel) const
{
    auto frame = this->frame();
    auto sun = this->sun();
    if(!frame || !sun) return eErrorNullPtr;
    return sun->getPosVelIn(frame, tp, sunPos, sunVel);
}

errc_t AttitudeRelSunLH::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    Vector3d pos;
    errc_t rc = this->getPosLocal(tp, pos);
    if(rc) return rc;
    Vector3d sunPos;
    rc = this->getSunPosLocal(tp, sunPos);
    if(rc) return rc;
    sunPos = sunPos - pos;   
    return aAlignConstrainTransform({0, 0, -1}, pos, {1, 0, 0}, sunPos, rotation);
}

errc_t AttitudeRelSunLH::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if(rc) return rc;
    Vector3d sunPos, sunVel;
    rc = this->getSunPosVelLocal(tp, sunPos, sunVel);
    if(rc) return rc;
    sunPos = sunPos - pos;
    sunVel = sunVel - vel;
    return aAlignConstrainTransform({0, 0, -1}, pos, vel, {1, 0, 0}, sunPos, sunVel, rotation);
}


void AttitudeRelSunLH::setSun(Body *sun)
{
    this->sun_ = sun;
}


AST_NAMESPACE_END
