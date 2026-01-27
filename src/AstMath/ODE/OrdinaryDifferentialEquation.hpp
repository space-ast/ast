///
/// @file      OrdinaryDifferentialEquation.hpp
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

AST_NAMESPACE_BEGIN

/// @class     OrdinaryDifferentialEquation
/// @brief     常微分方程基接口
/// @details   该类定义了常微分方程的接口，包括获取维度和计算导数。
class OrdinaryDifferentialEquation
{
public:
    virtual ~OrdinaryDifferentialEquation() = default;
    
    /// @brief 获取微分方程的维度。
    /// @return 微分方程的维度。
    virtual int getDimension() const = 0;

    /// @brief 计算常微分方程在给定时间点和状态下的导数。
    /// @details 
    /// 为什么将时间作为最后一个参数
    /// 1. 在很多情况下，ODE 系统是自洽系统（时间无关）
    /// 2. 与 Boost.Odeint 库的接口设计保持一致
    /// 3. 与 Julia 语言的ODE求解器设计保持一致
    /// @param y 状态向量。
    /// @param dy 输出导数向量。
    /// @param t 时间点。
    /// @return 错误码。
    virtual err_t evaluate(const double* y, double* dy, const double t) = 0;

    /// 兼容性处理
    A_ALWAYS_INLINE 
    err_t evaluate(const double t,const double* y, double* dy){return evaluate(y, dy, t);}
};

using ODE = OrdinaryDifferentialEquation;       ///< 常微分方程别名

AST_NAMESPACE_END
