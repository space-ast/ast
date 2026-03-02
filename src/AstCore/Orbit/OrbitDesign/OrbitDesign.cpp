///
/// @file      OrbitDesign.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-25
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

#include "OrbitDesign.hpp"
#include "AstCore/OrbitalPrecession.hpp"
#include "AstMath/Zeros.hpp"
#include "AstUtil/Constants.h"
#include "AstUtil/Logger.hpp"
#include <limits>

AST_NAMESPACE_BEGIN


err_t aSunSynchronousInclination(double gm, double j2, double rb, double bodyMeanMotion, double a, double ecc, double& inc)
{
    auto func = [gm, j2, rb, a, ecc, bodyMeanMotion](double inc) -> double 
    {
        return aRAANRate(gm, j2, rb, a, ecc, inc) - bodyMeanMotion;
    };
    SolverStats stats;
    inc = brentq(func, 0, kPI, 1e-12, 1e-14, 100, stats);
    if(stats.error_num != 0){
        inc = std::numeric_limits<double>::quiet_NaN();
        aError("failed to solve inclination for sun synchronous orbit.");
    }
    return stats.error_num;
}


err_t aSunSynchronousSemiMajorAxis(double gm, double j2, double rb, double bodyMeanMotion, double inc, double ecc, double &semiMajorAxis)
{
    auto func = [gm, j2, rb, inc, ecc, bodyMeanMotion](double a) -> double 
    {
        return aRAANRate(gm, j2, rb, a, ecc, inc) - bodyMeanMotion;
    };
    SolverStats stats;
    semiMajorAxis = brentq(func, rb, 1e4 * rb, 1e-12, 1e-14, 100, stats);
    if(stats.error_num != 0){
        semiMajorAxis = std::numeric_limits<double>::quiet_NaN();
        aError("failed to solve semiMajorAxis for sun synchronous orbit.");
    }
    return stats.error_num;
}


AST_NAMESPACE_END
