///
/// @file      RKF78.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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

#pragma once

#include "AstGlobal.h"
#include "AstMath/ODEVarStepIntegrator.hpp"

AST_NAMESPACE_BEGIN

/// @brief     RKF78 积分器
/// @details   
/// 参考文献：Classical Fifth-, Sixth-, Seventh-, and Eighth-Order Runge-Kutta Formulas with Stepsize Control
class AST_MATH_API RKF78 : public ODEVarStepIntegrator
{
public:
    err_t initialize(ODE& ode) final;

    err_t singleStep(ODE& ode, double t0, double step, const double* y0, double* yf) final;
};

AST_NAMESPACE_END
