///
/// @file      OrbitManeuver.cpp
/// @brief     轨道机动相关计算函数
/// @details   
/// @author    axel
/// @date      2026-08-29
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

#include "OrbitManeuver.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Math.hpp"
#include "AstCore/OrbitGeometry.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

double aApsisDeltaV(double burnRadius, double currentOtherRadius, double targetRadius, double gm)
{
    // 拱点速度由 vis-viva 求得：v = sqrt(2·gm·r_other / (r_burn·(r_burn + r_other)))
    const double v0 = std::sqrt(2.0 * gm * currentOtherRadius
                            / (burnRadius * (burnRadius + currentOtherRadius)));
    const double v1 = std::sqrt(2.0 * gm * targetRadius
                            / (burnRadius * (burnRadius + targetRadius)));
    return v1 - v0;
}

double aApsisDeltaV(const ModOrbElem& modOrbElem, double targetRadius, double gm)
{
    double burnRadius;
    double otherRadius;
    const double eps = 1e-5;
    if(fabs(aNormalizeAngleNegPiToPi(modOrbElem.trueA_)) < eps)
    {
        burnRadius = modOrbElem.getPeriRad();   
        otherRadius = modOrbElem.getApoRad();   
    }
    else if(fabs(aNormalizeAngleNegPiToPi(modOrbElem.trueA_ - kPI)) < eps)
    {
        burnRadius = modOrbElem.getApoRad();   
        otherRadius = modOrbElem.getPeriRad();   
    }
    else
    {
        aError("burn position is not periapsis or apoapsis, with trueA_ = %.16g", rad2deg(modOrbElem.trueA_));
        double sma = modOrbElem.getSMA();
        burnRadius = aOrbitRadius(sma, modOrbElem.e_, modOrbElem.trueA_);
        otherRadius = sma * 2 - burnRadius;
    }
    return aApsisDeltaV(burnRadius, otherRadius, targetRadius, gm);
}

double aApsisDeltaV(const CartState& state, double targetRadius, double gm)
{
    ModOrbElem modOrbElem;
    aCartToModOrbElem(state.pos(), state.vel(), gm, modOrbElem);
    return aApsisDeltaV(modOrbElem, targetRadius, gm);
}

AST_NAMESPACE_END
