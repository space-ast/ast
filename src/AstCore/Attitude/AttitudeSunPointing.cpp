///
/// @file      AttitudeSunPointing.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-05
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

#include "AttitudeSunPointing.hpp"
#include "AstMath/AttitudeUtil.hpp"

AST_NAMESPACE_BEGIN

AttitudeSunPointing::AttitudeSunPointing(Point* point, Frame* frame)
{
    this->setPoint(point);
    this->setFrame(frame);
}

AttitudeSunPointing::AttitudeSunPointing(Point* point, Body* body)
{
    this->setPoint(point);
    this->setFrame(body->getFrameInertial());
}

errc_t AttitudeSunPointing::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    Vector3d pos;
    errc_t rc = this->getPosLocal(tp, pos);
    if (rc) return rc;
    Vector3d sunPos;
    rc = this->getSunPosLocal(tp, sunPos);
    if (rc) return rc;
    // 体 X 对齐太阳方向，体 Z 约束到指向本坐标系原点的方向(对地指向)
    sunPos = sunPos - pos;
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos, 
        {0, 0, 1}, -pos, 
        rotation
    );
}

errc_t AttitudeSunPointing::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if (rc) return rc;
    Vector3d sunPos, sunVel;
    rc = this->getSunPosVelLocal(tp, sunPos, sunVel);
    if (rc) return rc;
    sunPos = sunPos - pos;
    sunVel = sunVel - vel;
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos, sunVel,
        {0, 0, 1}, -pos, -vel, 
        rotation
    );
}

AST_NAMESPACE_END
