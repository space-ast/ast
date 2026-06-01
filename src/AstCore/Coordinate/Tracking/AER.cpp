///
/// @file      AER.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-01
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

#include "AER.hpp"
#include "AstUtil/Math.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

void aNEDToAER(const Vector3d& ned, AER& aer)
{
    double n = ned.x();
    double e = ned.y();
    double d = ned.z();

    double r = std::sqrt(n * n + e * e + d * d);
    aer.range_ = r;

    if (r > 0.0)
    {
        aer.azimuth_ = std::atan2(e, n);
        aer.elevation_ = std::asin(-d / r);  // |d| <= |r| 一定成立
    }
    else
    {
        aer.azimuth_ = 0.0;
        aer.elevation_ = 0.0;
    }
}


void aAERToNED(const AER& aer, Vector3d& ned)
{
    double sinEl, cosEl, sinAz, cosAz;
    sincos(aer.elevation_, &sinEl, &cosEl);
    sincos(aer.azimuth_,   &sinAz, &cosAz);

    ned.x() = aer.range_ * cosEl * cosAz;
    ned.y() = aer.range_ * cosEl * sinAz;
    ned.z() = -aer.range_ * sinEl;
}


void aENUToAER(const Vector3d& enu, AER& aer)
{
    double e = enu.x();
    double n = enu.y();
    double u = enu.z();

    double r = std::sqrt(e * e + n * n + u * u);
    aer.range_ = r;

    if (r > 0.0)
    {
        aer.azimuth_ = std::atan2(e, n);
        aer.elevation_ = std::asin(u / r);  // |u| <= |r| 一定成立
    }
    else
    {
        aer.azimuth_ = 0.0;
        aer.elevation_ = 0.0;
    }
}


void aAERToENU(const AER& aer, Vector3d& enu)
{
    double sinEl, cosEl, sinAz, cosAz;
    sincos(aer.elevation_, &sinEl, &cosEl);
    sincos(aer.azimuth_,   &sinAz, &cosAz);

    enu.x() = aer.range_ * cosEl * sinAz;
    enu.y() = aer.range_ * cosEl * cosAz;
    enu.z() = aer.range_ * sinEl;
}




AST_NAMESPACE_END

