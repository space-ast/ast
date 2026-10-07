///
/// @file      AttitudeSunPointingCbiZ.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-06
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

#include "AttitudeSunPointingCbiZ.hpp"
#include "AstMath/AttitudeUtil.hpp"

AST_NAMESPACE_BEGIN

AttitudeSunPointingCbiZ::AttitudeSunPointingCbiZ(Point* point, Body* body)
{
    this->setPoint(point);
    this->setBody(body);
}

void AttitudeSunPointingCbiZ::setBody(Body* body)
{
    if (body)
        this->AttitudeTrajectoryRelated::setFrame(body->getFrameInertial());
}

errc_t AttitudeSunPointingCbiZ::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    Vector3d pos;
    errc_t rc = this->getPosLocal(tp, pos);
    if (rc) return rc;
    Vector3d sunPos;
    rc = this->getSunPosLocal(tp, sunPos);
    if (rc) return rc;
    sunPos = sunPos - pos;
    // 体 X 对齐太阳方向，体 Z 约束到参考坐标系的 Z 轴(参考系取 ECI 系即为 ECI Z 轴)
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos,
        {0, 0, 1}, Vector3d{0.0, 0.0, 1.0},
        rotation
    );
}

errc_t AttitudeSunPointingCbiZ::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if (rc) return rc;
    Vector3d sunPos, sunVel;
    rc = this->getSunPosVelLocal(tp, sunPos, sunVel);
    if (rc) return rc;
    sunPos = sunPos - pos;
    sunVel = sunVel - vel;
    // 约束方向在参考坐标系下固定不变，其变化率为零
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos, sunVel,
        {0, 0, 1}, Vector3d{0.0, 0.0, 1.0}, Vector3d::Zero(),
        rotation
    );
}

AST_NAMESPACE_END
