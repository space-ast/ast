///
/// @file      AttitudeNadirNormal.cpp
/// @brief     对地指向 + 轨道法向约束姿态
/// @details
/// @author    axel
/// @date      2026-09-28
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

#include "AttitudeNadirNormal.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstMath/AttitudeUtil.hpp"
#include "AstMath/GeometryUtil.hpp"


AST_NAMESPACE_BEGIN

AttitudeNadirNormal::AttitudeNadirNormal(Point *point, Frame *frame)
{
    this->setPoint(point);
    this->setFrame(frame);
}

AttitudeNadirNormal::AttitudeNadirNormal(Point *point, Body *body)
{
    this->setPoint(point);
    this->setFrame(body->getFrameInertial());
}

errc_t AttitudeNadirNormal::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if(rc) return rc;
    Vector3d h = pos.cross(vel);
    return aAlignConstrainTransform({0, 0, -1}, pos, {1, 0, 0}, h, rotation);
}


errc_t AttitudeNadirNormal::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel, acc;
    errc_t rc = this->getPosVelAccLocal(tp, pos, vel, acc);
    if(rc) return rc;
    Vector3d h, hdot;
    aVectorCross(pos, vel, vel, acc, h, hdot);
    return aAlignConstrainTransform({0, 0, -1}, pos, vel, {1, 0, 0}, h, hdot, rotation);
}

AST_NAMESPACE_END
