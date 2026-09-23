///
/// @file      Launch.cpp
/// @brief     发射弹道相关计算
/// @details   提供发射方位角（射向）与发射点纬度、目标轨道倾角之间的换算接口。
/// @author    axel
/// @date      2026-09-17
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

#include "Launch.hpp"
#include "AstUtil/Math.hpp"
#include "AstUtil/Constants.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

errc_t aInclinationToLaunchAzimuths(double inclination, double latitude, double& azimuthNorth, double& azimuthSouth)
{
    if (inclination < 0.0 || inclination > kPI)
        return eErrorInvalidParam;
    if (latitude < -kHalfPI || latitude > kHalfPI)
        return eErrorInvalidParam;

    // 极点附近射向不唯一：任意方位角对应的轨道倾角都是 π/2
    const double cosLat = std::cos(latitude);
    if (cosLat < 1e-12)
        return eErrorOutOfRange;

    // cos(i) = cos(lat) * sin(az)
    const double sinAz = std::cos(inclination) / cosLat;
    if (std::fabs(sinAz) > 1.0 + 1e-12)     // 目标倾角低于发射点纬度，无法直接入轨
        return eErrorOutOfRange;

    // 北向解落在 [-π/2, π/2]，南向解与北向解关于正东方向对称
    const double azimuth = asinSafe(sinAz);  // sinAz 已限定在 [-1, 1] 附近，箝位消除舍入误差
    azimuthNorth = aNormalizeAngle0To2Pi(azimuth);
    azimuthSouth = aNormalizeAngle0To2Pi(kPI - azimuth);
    return eNoError;
}

double aLaunchAzimuthToInclination(double azimuth, double latitude)
{
    if (latitude < -kHalfPI || latitude > kHalfPI)
        return kNaN;
    // cos(i) = cos(lat) * sin(az)
    return acosSafe(std::cos(latitude) * std::sin(azimuth));
}

AST_NAMESPACE_END
