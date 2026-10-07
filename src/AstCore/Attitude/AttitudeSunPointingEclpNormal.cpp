///
/// @file      AttitudeSunPointingEclpNormal.cpp
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

#include "AttitudeSunPointingEclpNormal.hpp"
#include "AstCore/AxesICRF.hpp"
#include "AstMath/AttitudeUtil.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstUtil/Constants.hpp"
#include "AstUtil/Literals.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

namespace {

/// @brief J2000 平黄赤交角，FK5 IAU76 理论取值 84381.448 角秒
constexpr double kObliquityJ2000 = 84381.448_arcsec;

/// @brief 黄道面法向(黄道北极)的单位向量，在 ICRF 下为常量
/// @details ICRF 的 XY 平面绕 X 轴转过平黄赤交角即得黄道面
///          故黄道法向在 ICRF 下为 (0, -sin ε, cos ε)
Vector3d eclipticNormalICRF()
{
    return Vector3d{0.0, -std::sin(kObliquityJ2000), std::cos(kObliquityJ2000)};
}

} // namespace

AttitudeSunPointingEclpNormal::AttitudeSunPointingEclpNormal(Point* point, Frame* frame)
{
    this->setPoint(point);
    this->setFrame(frame);
}

AttitudeSunPointingEclpNormal::AttitudeSunPointingEclpNormal(Point* point, Body* body)
{
    this->setPoint(point);
    this->setFrame(body->getFrameInertial());
}

errc_t AttitudeSunPointingEclpNormal::getEclipticNormalLocal(const TimePoint& tp, Vector3d& normal) const
{
    auto frame = this->frame();
    if (!frame) return eErrorNullPtr;
    Rotation rot;
    errc_t rc = aAxesTransform(aAxesICRF(), frame->getAxes(), tp, rot);
    if (rc) return rc;
    normal = rot.transformVector(eclipticNormalICRF());
    return eNoError;
}

errc_t AttitudeSunPointingEclpNormal::getEclipticNormalLocal(const TimePoint& tp, Vector3d& normal, Vector3d& normalRate) const
{
    auto frame = this->frame();
    if (!frame) return eErrorNullPtr;
    KinematicRotation rot;
    errc_t rc = aAxesTransform(aAxesICRF(), frame->getAxes(), tp, rot);
    if (rc) return rc;
    // 黄道法向在 ICRF 下固定不变，其变化率全部来自参考系自身的转动
    rot.transformVectorVelocity(eclipticNormalICRF(), Vector3d::Zero(), normal, normalRate);
    return eNoError;
}

errc_t AttitudeSunPointingEclpNormal::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    Vector3d pos;
    errc_t rc = this->getPosLocal(tp, pos);
    if (rc) return rc;
    Vector3d sunPos;
    rc = this->getSunPosLocal(tp, sunPos);
    if (rc) return rc;
    sunPos = sunPos - pos;
    Vector3d eclpNormal;
    rc = this->getEclipticNormalLocal(tp, eclpNormal);
    if (rc) return rc;
    // 体 X 对齐太阳方向，体 Z 约束到黄道面法向
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos,
        {0, 0, 1}, eclpNormal,
        rotation
    );
}

errc_t AttitudeSunPointingEclpNormal::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    Vector3d pos, vel;
    errc_t rc = this->getPosVelLocal(tp, pos, vel);
    if (rc) return rc;
    Vector3d sunPos, sunVel;
    rc = this->getSunPosVelLocal(tp, sunPos, sunVel);
    if (rc) return rc;
    sunPos = sunPos - pos;
    sunVel = sunVel - vel;
    Vector3d eclpNormal, eclpNormalRate;
    rc = this->getEclipticNormalLocal(tp, eclpNormal, eclpNormalRate);
    if (rc) return rc;
    return aAlignConstrainTransform(
        {1, 0, 0}, sunPos, sunVel,
        {0, 0, 1}, eclpNormal, eclpNormalRate,
        rotation
    );
}

AST_NAMESPACE_END
