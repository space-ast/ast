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
#include "AstMath/Euler.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN
using namespace _AST literals;

err_t aEarthICRFToMoonICRF(const TimePoint &tp, const Vector3d &posInEarthICRF, Vector3d &posInMoonICRF)
{
    Vector3d moonPos;
    err_t rc = aJplDeGetPosICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos);
    posInMoonICRF = posInEarthICRF - moonPos;
    return rc;
}

err_t aEarthICRFToMoonICRF(const TimePoint &tp, const Vector3d &posInEarthICRF, const Vector3d &velInEarthICRF, Vector3d &posInMoonICRF, Vector3d &velInMoonICRF)
{
    Vector3d moonPos, moonVel;
    err_t rc = aJplDeGetPosVelICRF(tp, JplDe::eMoon, JplDe::eEarth, moonPos, moonVel);
    posInMoonICRF = posInEarthICRF - moonPos;
    velInMoonICRF = velInEarthICRF - moonVel;
    return rc;
}

err_t aICRFToMoonPrincipalAxesTransform(const TimePoint &tp, Rotation &rotation)
{
    Vector3d ang;
    err_t rc = aJplDeGetLibration(tp, ang);
    if(rc) return rc;
    static_assert(sizeof(Euler) == sizeof(Vector3d), "Euler and Vector3d must have the same size");
    aEuler313ToMatrix((const Euler&)ang, rotation.getMatrix());
    return eNoError;
}

err_t aMoonPAToMeanEarthTransform(Rotation &rotation)
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

void aMoonPA421ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-67.92_arcsec, -78.56_arcsec, -0.30_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonPA418ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-68.00_arcsec, -78.62_arcsec, -0.27_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

void aMoonPA403ToMeanEarthTransform(Rotation &rotation)
{
    Euler angle{-63.8986_arcsec, -79.0768_arcsec, -0.1462_arcsec};
    aEuler321ToMatrix(angle, rotation.getMatrix());
}

err_t aICRFToMoonMeanEarthTransform_DE(const TimePoint &tp, Rotation &rotation)
{
    err_t rc = aICRFToMoonPrincipalAxesTransform(tp, rotation);
    if(rc) return rc;
    Rotation rotation2;
    rc = aMoonPAToMeanEarthTransform(rotation2);
    rotation *= rotation2;
    return rc;
}


err_t aICRFToMoonMeanEarthTransform(const TimePoint &tp, Rotation &rotation)
{
    return aICRFToMoonMeanEarthTransform_DE(tp, rotation);
}

AST_NAMESPACE_END
