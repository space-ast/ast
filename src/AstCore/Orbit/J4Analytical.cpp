///
/// @file      J4Analytical.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "J4Analytical.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

static void aGetJ2J4AnalyticalParams(const ModOrbElem &modOrbElem, double gm, double j2, double* j4_ptr, double re, double &meanAnomalyDot, double &raanDot, double &argPeriDot)
{
    double n = modOrbElem.getMeanMotion(gm);                      ///< 平均运动数
    double p = modOrbElem.getP();                                  ///< 半通径
    double p2 = p * p;                                              ///< 半通径的平方
    double p4 = p2 * p2;
    double e2 = modOrbElem.e()*modOrbElem.e();
    double e4 = e2 * e2;
    double sqrt1me2 = sqrt(1 - e2);                                ///< 1 - e^2 的平方根
    double re2 = re * re;                                         ///< 地球半径的平方
    double re4 = re2 * re2;
    double sini, cosi;
    sincos(modOrbElem.i(), &sini, &cosi);
    double sini2 = sini * sini;                                     ///< 轨道倾角的正弦值的平方
    double sini4 = sini2 * sini2;                                    ///< 轨道倾角的正弦值的四次方

    meanAnomalyDot =  n - 3*n*re2*j2 * sqrt1me2 / (4 * p2) * (3 * sini2 - 2);
    argPeriDot = 3 * n * j2 * re2 / (4 * p2) * (4 - 5 * sini2) ;
    raanDot = -3 * j2 * re2 * n * cosi / (2 * p2);
    
    if(j4_ptr != nullptr)
    {
        double j4 = *j4_ptr;
        meanAnomalyDot += 
            + 3 * n * re4 * j2 * j2 /(512 * p4 * sqrt1me2) * (320*e2 - 280 * e4 + (1600 - 1568*e2 + 328*e4)*sini2 + (-2096 + 1072 * e2 + 79 * e4) * sini4)
            - 45 * j4 * re4 * e2 * n * sqrt1me2 / (128*p4) *(-8 + 40 * sini - 35*sini2);
            ;
        argPeriDot += 
             + 9 * n * j2 * j2 * re4 / (384 * p4) * (56 * e2 + (760 - 36 * e2) * sini2 - (890+45*e2)*sini4)
             - 15 * j4 * re4 * n / (128 * p4) *(64 + 72 * e2 - (248 + 252 * e2) * sini2 + (196 + 189 * e2) * sini4)
            ;
        raanDot +=
            + 3 * j2 * j2 * re4 * n *cosi / (32 * p4) *(12 - 4 * e2 - (80 + 5 * e2) * sini2)
            + 15 * j4 * re4 * n *cosi /(32 * p4) * (8 + 12 * e2 - (14 + 21 * e2) * sini2)
            ;
    }
}

void aGetJ2AnalyticalParams(const ModOrbElem &modOrbElem, double gm, double j2, double re, double &meanAnomalyDot, double &raanDot, double &argPeriDot)
{
    aGetJ2J4AnalyticalParams(modOrbElem, gm, j2, nullptr, re, meanAnomalyDot, raanDot, argPeriDot);
}

void aGetJ4AnalyticalParams(const ModOrbElem &modOrbElem, double gm, double j2, double j4, double re, double &meanAnomalyDot, double &raanDot, double &argPeriDot)
{
    aGetJ2J4AnalyticalParams(modOrbElem, gm, j2, &j4, re, meanAnomalyDot, raanDot, argPeriDot);
}



J4Analytical::J4Analytical(const ModOrbElem &modOrbElem, const TimePoint &epoch, double gm, double j2, double j4, double re)
    : J2J4Analytical(modOrbElem, epoch, gm)
    , j2_(j2), j4_(j4), re_(re)
{
    aGetJ4AnalyticalParams(modOrbElem, gm, j2, j4, re, meanAnomalyDot_, raanDot_, argPeriDot_);
}

J4Analytical::J4Analytical(const Vector3d &pos, const Vector3d &vel, const TimePoint &epoch, double gm, double j2, double j4, double re)
    : J4Analytical(aCartToModOrbElem(pos, vel, gm), epoch, gm, j2, j4, re)
{
}

err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, Vector3d& r, Vector3d& v)
{
    TimePoint epoch{};      // @fixme 这里的epoch是个问题，应该根据实际情况传入，目前采用默认值
    J4Analytical j4Analytical(r, v, epoch, gm, j2, j4, re);
    return j4Analytical.prop(duration, r, v);}

err_t aJ4AnalyticalProp(double duration, double gm, double j2, double j4, double re, ModOrbElem &modOrbElem)
{
    TimePoint epoch{};      // @fixme 这里的epoch是个问题，应该根据实际情况传入，目前采用默认值
    J4Analytical j4Analytical(modOrbElem, epoch, gm, j2, j4, re);
    return j4Analytical.prop(duration, modOrbElem);
}

AST_NAMESPACE_END

