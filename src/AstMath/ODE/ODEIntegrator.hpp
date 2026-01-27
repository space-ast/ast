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
#include <vector>

AST_NAMESPACE_BEGIN

class ODEStepHandler;

/// @brief ODE 积分器接口类
/// @details ~
class IODEIntegrator
{
public:
    virtual ~IODEIntegrator() {};

    /// @brief 初始化积分器
    /// @details 初始化积分器，设置ODE的维度和步长等参数
    /// @param ode 常微分方程对象
    virtual err_t initialize(ODE& ode) = 0;


    /// @brief 积分ODE
    /// @details 积分ODE，将积分结果存储在y中
    /// @param[in] ode 常微分方程对象
    /// @param[in,out] y 状态向量
    /// @param[in,out] t 当前时间
    /// @param[in] tf 最终时间
    virtual err_t integrate(ODE& ode, double* y,double& t, double tf) = 0;


    /// @brief 积分ODE一步
    /// @details 积分ODE一步，将积分结果存储在y中
    ///          如果是变步长积分器，步长会根据误差自动调整
    ///          如果是定步长积分器，步长会固定为初始步长
    /// @param[in] ode 常微分方程对象
    /// @param[in,out] y 状态向量
    /// @param[in,out] t 当前时间
    /// @param[in] tf 最终时间
    virtual err_t integrateStep(ODE& ode, double* y, double& t, double tf) = 0;

    /// @brief 执行一步积分
    /// @param ode 常微分方程对象
    /// @param[in,out] y 状态向量
    /// @param t0 当前时间
    /// @param step 时间步长
    virtual err_t singleStep(ODE& ode, double* y, double t0, double step) = 0;
};


/// @brief ODE 积分器
/// @details ~
class AST_MATH_API ODEIntegrator : public IODEIntegrator
{
public:
    ~ODEIntegrator() override;

    using IODEIntegrator::integrate;

    /// @brief 积分ODE
    /// @details 积分ODE，将积分结果存储在y中
    ///          同时将时间点和状态向量存储在xlist和ylist中
    /// @param[in] ode 常微分方程对象
    /// @param[in,out] y 状态向量
    /// @param[in,out] t 当前时间
    /// @param[in] tf 最终时间
    /// @param[out] xlist 时间点列表
    /// @param[out] ylist 状态向量列表
    err_t integrate(
        ODE& ode, double* y, double& t, double tf,
        std::vector<double>& xlist, std::vector<std::vector<double>>& ylist
    );


protected:
    ODEStepHandler* stepHandler_{nullptr};
};
AST_NAMESPACE_END
