///
/// @file      RKF56.cpp
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

#include "RKF56.hpp"
#include <cmath>
#include <assert.h>

AST_NAMESPACE_BEGIN

static const double
    ch56_6[8]    { 7./1408., 0.   , 1125./2816., 9./32., 125./768., 0.    , 5./66., 5./66. },
    ch56_5[8]    {31./384. , 0.   , 1125./2816., 9./32., 125./768., 5./66., 0.    , 0.     },
    alph56[8]    {0.       , 1./6., 4./15.     , 2./3. , 4./5.    , 1.    , 0.    , 1.     },
    beta56[8][7]{
        {},
        {1./6.     , 0.      ,0.       , 0.       , 0.      , 0. , 0.},
        {4./75     ,16./75.  ,0.       , 0.       , 0.      , 0. , 0.},
        {5./6.     ,-8./3.   ,5./2     , 0.       , 0.      , 0. , 0.},
        {-8./5.    ,144./25. ,-4       , 16./25.  , 0.      , 0. , 0.},
        {361./320. ,-18./5   ,407./128 ,-11./80.  , 55./128., 0. , 0.},
        {-11./640. , 0.      ,11./256. ,-11./160. , 11./256., 0. , 0.},
        {93./640.  , -18./5. ,803./256.,-11./160. , 99./256., 0. , 1.}};

err_t RKF56::init(ODE &ode)
{
    // 重置工作空间
    this->getWorkspace().reset(ode.getDimension(), 8);
    return eNoError;
}

err_t RKF56::singleStep(ODE &ode, double t0, double h, const double *y0, double *yf)
{
    const double err_factor = -5.0 / 66.0;

    auto& wrk = this->getWorkspace();
    int ndim = wrk.dimension_;
    auto KArr = wrk.KArr_;
    double* ymid = wrk.ymid_;

    for (int k = 0; k < 8; k++) {
        for (int i = 0; i < ndim; i++) {
            double sumtemp = 0.0;
            for (int j = 0; j < k; j++) {
                sumtemp += beta56[k][j] * KArr[j][i];
            }
            ymid[i] = y0[i] + sumtemp * h;
        }
        // kvec.SetBuffer(ndim, KArr + k * ndim);
        // func(t + alph56[k] * h, ymidvec, kvec);
        //rightFunc(ndim, t + alph78[k] * h, ymid, &KArr[k * ndim], rkfFuncParam);
        if(err_t err = ode.evaluate(t0 + alph56[k] * h, ymid, KArr[k]))
        {
            return err;
        }
    }

    for (int i = 0; i < ndim; i++)
    {
        double sumtemp = 0.0;
        for (int k = 0; k < 8; k++) {
            sumtemp += ch56_6[k] * KArr[k][i];    // 5阶公式
        }
        yf[i] = y0[i] + sumtemp * h;
        wrk.absErrPerLen_[i] = err_factor * (        // 每单位长度截断误差估计（5阶相对于6阶）
            + KArr[0][i]
            + KArr[5][i]
            - KArr[6][i]
            - KArr[7][i]
        );
    }
#if 0
#define ASSERT_EQ(x, y) assert(abs((x) - (y)) < 1e-10);
#ifndef NDEBUG
        for (int i = 0; i < 8; i++) {
            double s;
            vector_op::vector_sum(7, s, beta56[i]);
            ASSERT_EQ(alph56[i], s);
        }
        double s;
        vector_op::vector_sum(8, s, ch56_6);
        ASSERT_EQ(1, s);
        vector_op::vector_sum(8, s, ch56_6);
        ASSERT_EQ(1, s);
#endif // !NDEBUG
#endif

    return eNoError;
}

AST_NAMESPACE_END