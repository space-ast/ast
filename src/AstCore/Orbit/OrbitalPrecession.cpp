///
/// @file      OrbitalPrecession.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-27
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

#include "OrbitalPrecession.hpp"
#include "AstUtil/Constants.h"
#include <cmath>


AST_NAMESPACE_BEGIN


double aRAANRate(double gm, double j2, double rb, double a, double ecc, double inc)
{
    // double n = sqrt(gm / pow(a, 3));
    double n = aMeanAnomalyRate(gm, j2, rb, a, ecc, inc);
    return -3./2. * j2 * n * pow(rb / a, 2) * cos(inc) / pow(1 - ecc * ecc, 2);
}

double aArgPerRate(double gm, double j2, double rb, double a, double ecc, double inc)
{
    // double n = sqrt(gm / pow(a, 3));
    double n = aMeanAnomalyRate(gm, j2, rb, a, ecc, inc);
    return 3./4. * j2 * n * pow(rb / a, 2) * (5 * pow(cos(inc), 2) - 1) / pow(1 - ecc * ecc, 2);
}

double aMeanAnomalyRate(double gm, double j2, double rb, double a, double ecc, double inc)
{
    double n = sqrt(gm / pow(a, 3));
    return n + 3./2. * j2 * n * pow(rb / a, 2) * (1 - 3./2. * pow(sin(inc), 2)) * pow(1 - ecc * ecc, -3./2.);
}

double aJ2Period(double gm, double j2, double rb, double a, double ecc, double inc)
{
    double rate = aMeanAnomalyRate(gm, j2, rb, a, ecc, inc) + aArgPerRate(gm, j2, rb, a, ecc, inc);
    return kTwoPI / rate;
}


AST_NAMESPACE_END
