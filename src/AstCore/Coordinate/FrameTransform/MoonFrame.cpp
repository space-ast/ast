///
/// @file      MoonFrame.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-29
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

#include "MoonFrame.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Euler.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN
using namespace _AST literals;

// #define _AST_DEBUG_MOON_FRAME

errc_t aEarthICRFToMoonICRF(const TimePoint &tp, const Vector3d &posInEarthICRF, Vector3d &posInMoonICRF)
{
    Vector3d moonPos;
    errc_t rc = aJplDeGetPosICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos);
    posInMoonICRF = posInEarthICRF - moonPos;
    return rc;
}

errc_t aEarthICRFToMoonICRF(const TimePoint &tp, const Vector3d &posInEarthICRF, const Vector3d &velInEarthICRF, Vector3d &posInMoonICRF, Vector3d &velInMoonICRF)
{
    Vector3d moonPos, moonVel;
    errc_t rc = aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos, moonVel);
    posInMoonICRF = posInEarthICRF - moonPos;
    velInMoonICRF = velInEarthICRF - moonVel;
    return rc;
}

errc_t aMoonICRFToEarthICRF(const TimePoint &tp, const Vector3d &posInMoonICRF, Vector3d &posInEarthICRF)
{
    Vector3d moonPos;
    errc_t rc = aJplDeGetPosICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos);
    posInEarthICRF = posInMoonICRF + moonPos;
    return rc;
}

errc_t aMoonICRFToEarthICRF(const TimePoint &tp, const Vector3d &posInMoonICRF, const Vector3d &velInMoonICRF, Vector3d &posInEarthICRF, Vector3d &velInEarthICRF)
{
    Vector3d moonPos, moonVel;
    errc_t rc = aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos, moonVel);
    posInEarthICRF = posInMoonICRF + moonPos;
    velInEarthICRF = velInMoonICRF + moonVel;
    return rc;
}

errc_t aICRFToMoonPrincipalAxesTransform(const TimePoint &tp, Rotation &rotation)
{
    Euler ang{};
    errc_t rc = aJplDeGetLibration(tp, ang);
    if(rc) return rc;
    // static_assert(sizeof(Euler) == sizeof(Vector3d), "Euler and Vector3d must have the same size");
    aEuler313ToMatrix(ang, rotation.getMatrix());
    return eNoError;
}

errc_t aICRFToMoonPrincipalAxesTransform(const TimePoint& tp, KinematicRotation& rotation)
{
    Euler ang{}, angRate{};
    errc_t rc = aJplDeGetLibration(tp, ang, angRate);
    if(rc) return rc;
    aEuler313ToMatrix(ang, rotation.getMatrix());
    // 计算ICRF系下，PA相对于ICRF系的角速度
    // @todo 考虑将下面的逻辑提取出来一个函数
    {
        double s1, c1, s2, c2;
        sincos(ang.angle1(), &s1, &c1);
        sincos(ang.angle2(), &s2, &c2);
        Vector3d rotationRate
        {
             angRate.angle3() * s1 * s2 + angRate.angle2() * c1,
            -angRate.angle3() * c1 * s2 + angRate.angle2() * s1,
             angRate.angle3() * c2 + angRate.angle1()
        };
        rotation.setRotationRate(rotationRate);
    }

    #ifdef _AST_DEBUG_MOON_FRAME
    {
        // 计算PA系下，PA相对于ICRF系的角速度
        double s2, c2, s3, c3;
        sincos(ang.angle2(), &s2, &c2);
        sincos(ang.angle3(), &s3, &c3);
        Vector3d rotationRateInPA
        {
             angRate.angle2() * c3 + angRate.angle1() * s2 * s3,
            -angRate.angle2() * s3 + angRate.angle1() * s2 * c3,
             angRate.angle3() + angRate.angle1() * c2
        };
        Vector3d rotationRateInICRF = rotation.getRotation().transformVectorInv(rotationRateInPA);
        Vector3d rotationRate = rotation.getRotationRate();
        printf("rotationRateInICRF: %.15g, %.15g, %.15g\n", rotationRateInICRF.x(), rotationRateInICRF.y(), rotationRateInICRF.z());
        printf("rotationRate      : %.15g, %.15g, %.15g\n", rotationRate.x(), rotationRate.y(), rotationRate.z());
    }
    #endif
    return eNoError;
}



errc_t aMoonPAToMeanEarthTransform(Rotation &rotation)
{
    auto denum = aJplDeNum();
    switch (denum)
    {
    case 403:
        aMoonPA403ToMeanEarthTransform(rotation);
        break;
    case 418:
        aMoonPA418ToMeanEarthTransform(rotation);
        break;
    case 421:
        aMoonPA421ToMeanEarthTransform(rotation);
        break;
    case 0:
    case 430:
        aMoonPA430ToMeanEarthTransform(rotation);
        break;
    default:
        aError("JPL DE version %d is not supported", denum);
        return eErrorInvalidParam;
    }
    return eNoError;
}

void aMoonPA430ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-67.573_arcsec, -78.58_arcsec, -0.285_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonMeanEarthToPA430Transform(Rotation &rotation)
{
    aMoonPA430ToMeanEarthTransform(rotation);
    rotation = rotation.inverse();
}

void aMoonPA421ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-67.92_arcsec, -78.56_arcsec, -0.30_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonMeanEarthToPA421Transform(Rotation &rotation)
{
    aMoonPA421ToMeanEarthTransform(rotation);
    rotation = rotation.inverse();
}

void aMoonPA418ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-68.00_arcsec, -78.62_arcsec, -0.27_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonMeanEarthToPA418Transform(Rotation &rotation)
{
    aMoonPA418ToMeanEarthTransform(rotation);
    rotation = rotation.inverse();
}

void aMoonPA403ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-63.8986_arcsec, -79.0768_arcsec, -0.1462_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonMeanEarthToPA403Transform(Rotation &rotation)
{
    aMoonPA403ToMeanEarthTransform(rotation);
    rotation = rotation.inverse();
}

errc_t aICRFToMoonMeanEarthTransform_DE(const TimePoint &tp, Rotation &rotation)
{
    errc_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
    if (rc != eNoError) return rc;
    Rotation rotation2;
    rc = aMoonPAToMeanEarthTransform(rotation2);
    if (rc != eNoError) return rc;
    rotation *= rotation2;
    return rc;
}

errc_t aICRFToMoonMeanEarthTransform_DE(const TimePoint &tp, KinematicRotation &rotation)
{
    errc_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
    if (rc != eNoError) return rc;
    Rotation rotation2;
    rc = aMoonPAToMeanEarthTransform(rotation2);
    if (rc != eNoError) return rc;
    // @note 这里不需要处理角速度，PA和MeanEarth之间不存在旋转角速度，只有一个静态旋转
    rotation.getRotation() *= rotation2;
    return rc;
}

errc_t aICRFToMoonMeanEarthTransform(const TimePoint &tp, Rotation &rotation)
{
    return aICRFToMoonMeanEarthTransform_DE(tp, rotation);
}


errc_t aICRFToMoonMeanEarthTransform(const TimePoint &tp, KinematicRotation &rotation)
{
    return aICRFToMoonMeanEarthTransform_DE(tp, rotation);
}

AST_NAMESPACE_END
