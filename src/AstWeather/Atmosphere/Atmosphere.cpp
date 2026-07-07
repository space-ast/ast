///
/// @file      Atmosphere.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-22
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

#include "Atmosphere.hpp"
#include "AstWeather/atmos76.hpp"
#include "AstWeather/val_atm_jr.h"
#include "AstMath/Vector.hpp"


AST_NAMESPACE_BEGIN


double aUSSA1976(double alt)
{
    double sigma, delta, theta;
    Atmosphere(alt/1000.0, sigma, delta, theta);
    A_UNUSED(delta);
    A_UNUSED(theta);

    const double RHOZERO = 1.225; // 海平面标准密度, kg/m³
    return sigma * RHOZERO;
}

double aJacchiaRoberts(
    double height, const Vector3d &space_craft, const Vector3d &sun, 
    double a1_time, double tkp, double xtemp
)
{
    height /= 1000.0;
    auto space_craft_temp = space_craft / 1000;
    double rho = jac_rob(height, space_craft_temp.data(), sun.normalized().data(), a1_time, tkp, xtemp);
    return rho * 1000.0;
}

AST_NAMESPACE_END

