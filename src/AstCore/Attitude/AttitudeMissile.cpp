///
/// @file      AttitudeMissile.cpp
/// @brief     ECI 速度对齐 + 对地约束姿态
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

#include "AttitudeMissile.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstMath/AttitudeUtil.hpp"

AST_NAMESPACE_BEGIN

AttitudeMissile::AttitudeMissile(Point* point, Body* body)
{
    setPoint(point);
    setBody(body);
}

void AttitudeMissile::setBody(Body* body)
{
    if (body)
        this->AttitudeTrajectoryRelated::setFrame(body->getFrameInertial());
}

errc_t AttitudeMissile::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if(rc) return rc;
    return aAlignConstrainTransform({1, 0, 0}, vel, {0, 0, -1}, pos, rotation);
}


errc_t AttitudeMissile::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel, acc;
    errc_t rc = this->getPosVelAccLocal(tp, pos, vel, acc);
    if(rc) return rc;
    return aAlignConstrainTransform({1, 0, 0}, vel, acc, {0, 0, -1}, pos, vel, rotation);
}

AST_NAMESPACE_END
