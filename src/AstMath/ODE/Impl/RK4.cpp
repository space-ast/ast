///
/// @file      RK4.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-17
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

#include "RK4.hpp"

AST_NAMESPACE_BEGIN


err_t RK4::singleStep(ODE &ode, double t0, double step, const double *y0, double *yf)
{
    int err;
    auto& wrk = this->getWorkspace();
    int ndim = wrk.dimension_;
    double t = t0;
    double* k1 = wrk.KArr_[0];
    double* k2 = wrk.KArr_[1];
    double* k3 = wrk.KArr_[2];
    double* k4 = wrk.KArr_[3];
    double* ymid = wrk.ymid_;

    double hh = step * 0.5;

    // 计算 k1
    err = ode.evaluate(t, y0, k1);
    
    // 计算 k2
    for(int i = 0; i < ndim; i++)
    {
        ymid[i] = y0[i] + hh * k1[i];
    }
    err |= ode.evaluate(t0 + hh, ymid, k2);

    // 计算 k3
    for(int i = 0; i < ndim; i++)
    {
        ymid[i] = y0[i] + hh * k2[i];
    }
    err |= ode.evaluate(t0 + hh, ymid, k3);

    // 计算 k4
    for(int i = 0; i < ndim; i++)
    {
        ymid[i] = y0[i] + step * k3[i];
    }
    err |= ode.evaluate(t0 + step, ymid, k4);

    // 计算 yf
    for(int i = 0; i < ndim; i++)
    {
        yf[i] = y0[i] + step * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]) / 6.0;
    }

    return err;
}

AST_NAMESPACE_END

