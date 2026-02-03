///
/// @file      ODEStepObserver.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-28
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

AST_NAMESPACE_BEGIN

class ODEIntegrator;

enum EODEAction
{
    eContinue = 0,
    eStop = 1,
};

/// @brief ODE状态量观察者
class ODEStateObserver
{
public:
    virtual ~ODEStateObserver() = default;

    /// @brief 状态更新处理函数
    /// @param y 当前状态向量
    /// @param x 当前积分变量
    virtual EODEAction onStateUpdate(double* y, double& x, ODEIntegrator* integrator) = 0;

};


/// @brief 泛型ODE状态量观察者助手类
class ODEStateObserverGenericHelper
{
public:
    // 版本1: 支持 func_(y, x, integrator)
    template<typename F>
    static A_ALWAYS_INLINE auto call_func(F& func, double* y, double& x, ODEIntegrator* integrator) 
        -> decltype(std::declval<F>()(y, x, integrator))
    {
        return func(y, x, integrator);
    }
    
    // 版本2: 支持 func_(y, x)
    template<typename F>
    static A_ALWAYS_INLINE auto call_func(F& func, double* y, double& x, ODEIntegrator* integrator) 
        -> decltype(std::declval<F>()(y, x))
    {
        return func(y, x);
    }
    
    // 版本3: 支持 func_(y)
    template<typename F>
    static A_ALWAYS_INLINE auto call_func(F& func, double* y, double& x, ODEIntegrator* integrator) 
        -> decltype(std::declval<F>()(y))
    {
        return func(y);
    }
};

/// @brief 泛型ODE状态量观察者
template<typename Func>
class ODEStateObserverGeneric: public ODEStateObserver
{
public:
    using Self = ODEStateObserverGeneric<Func>;
    using FuncType = Func;
    explicit ODEStateObserverGeneric(Func func) 
        : func_(std::move(func)) 
    {}
    
    EODEAction onStateUpdate(double* y, double& x, ODEIntegrator* integrator) override {
        return this->operator()(y, x, integrator);
    }
    
private:
    Func func_;
private:
    // 处理不同的返回类型
    template<typename F = FuncType>
    A_ALWAYS_INLINE
    typename std::enable_if<!std::is_void<decltype(
        ODEStateObserverGenericHelper::call_func
        (std::declval<F&>(), std::declval<double*>(), std::declval<double&>(), std::declval<ODEIntegrator*>()))>::value, EODEAction>::type
    operator()(double* y, double& x, ODEIntegrator* integrator) {
        return ODEStateObserverGenericHelper::call_func(func_, y, x, integrator);
    }

    template<typename F = FuncType>
    A_ALWAYS_INLINE
    typename std::enable_if<std::is_void<decltype(
        ODEStateObserverGenericHelper::call_func
        (std::declval<F&>(), std::declval<double*>(), std::declval<double&>(), std::declval<ODEIntegrator*>()))>::value, EODEAction>::type
    operator()(double* y, double& x, ODEIntegrator* integrator) {
        ODEStateObserverGenericHelper::call_func(func_, y, x, integrator);
        return EODEAction::eContinue;
    }
};

AST_NAMESPACE_END
