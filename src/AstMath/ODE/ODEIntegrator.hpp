///
/// @file      ODEIntegrator.hpp
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
#include "OrdinaryDifferentialEquation.hpp"

AST_NAMESPACE_BEGIN

/// @brief ODE 积分器基类
/// @details ~
class ODEIntegrator
{
public:
    virtual ~ODEIntegrator() {};

    /// @brief 初始化积分器
    /// @details 初始化积分器，设置ODE的维度和步长等参数
    /// @param ode 常微分方程对象
    virtual err_t init(ODE& ode) = 0;

    /// @brief 积分ODE
    /// @param ode 常微分方程对象
    /// @param t0 初始时间
    /// @param tf 最终时间
    /// @param y0 初始状态向量
    /// @param yf 输出状态向量
    virtual err_t integrate(ODE& ode, double t0, double tf, const double* y0, double* yf) = 0;


    /// @brief 积分ODE一步
    /// @details 积分ODE一步，将积分结果存储在y中
    ///          如果是变步长积分器，步长会根据误差自动调整
    ///          如果是定步长积分器，步长会固定为初始步长
    /// @param ode 常微分方程对象
    /// @param[in,out] t 当前时间
    /// @param[in] tf 最终时间
    /// @param[in] y0 初始状态向量
    /// @param[out] y 输出状态向量
    virtual err_t integrateStep(ODE& ode, double& t, double tf, const double* y0, double* y) = 0;

    /// @brief 执行一步积分
    /// @param ode 常微分方程对象
    /// @param t0 当前时间
    /// @param step 时间步长
    /// @param y0 初始状态向量
    /// @param yf 输出状态向量
    virtual err_t singleStep(ODE& ode, double t0, double step, const double* y0, double* yf) = 0;
};

AST_NAMESPACE_END
