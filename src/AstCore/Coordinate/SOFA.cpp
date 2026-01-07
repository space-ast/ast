///
/// @file      SOFA.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-06
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

#include "SOFA.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN


#define DAS2R (kArcSecToRad)
#define DJC (kDaysPerJulianCentury)
#define DJ00 (kJ2000Epoch)

void aPrecession_IAU1976(const TimePoint& tp, double& zeta, double& z, double& theta)
{
   double t0, t, tas2r, w;

    /* Interval between fundamental epoch J2000.0 and start date (JC). */
   t0 = 0; // ((date01 - DJ00) + date02) / DJC;

    /* Interval over which precession required (JC). */
   t = tp.daysFromJ2000TT() / DJC; //((date11 - date01) + (date12 - date02)) / DJC;

    /* Euler angles. */
   tas2r = t * DAS2R;
   w = 2306.2181 + (1.39656 - 0.000139 * t0) * t0;

   zeta = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;

   z = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;

   theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)
          + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;
}



AST_NAMESPACE_END
