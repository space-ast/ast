///
/// @file      RKF45.cpp
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

#include "RKF45.hpp"

AST_NAMESPACE_BEGIN

static const double
    ch45_5[6]    { 16./135., 0.   , 6656./12825., 28561./56430.,  -9./50., 2./55.},
    ch45_4[6]    { 25./216., 0.   , 1408./2565. , 2197./4104.  ,  -1./5. , 0.    },
    alph45[6]    {0.       , 1./4., 3./8.       , 12./13.      ,  1.     , 1./2. },
    beta45[6][5] {
        {},
        {1./4.      , 0.          ,0.         , 0.         , 0.      },
        {3./32.     ,9./32.       ,0.         , 0.         , 0.      },
        {1932./2197.,-7200./2197. ,7296./2197 , 0.         , 0.      },
        {439./216.  ,-8.          ,3680./513. , -845./4104 , 0.      },
        {-8./27.    ,2.           ,-3544./2565, 1859./4104., -11/40.}};

        
err_t RKF45::initialize(ODE &ode)
{
    this->ODEIntegrator::initialize(ode);
    // 重置工作空间
    this->resetWorkspace(ode.getDimension(), 6);
    return eNoError;
}

err_t RKF45::singleStep(ODE &ode, double* y, double t0, double h)
{
    auto& wrk = this->getWorkspace();
    int ndim = wrk.dimension_;
    auto KArr = wrk.KArr_;
    double* ymid = wrk.ymid_;
    const double* y0 = y;
    double* yf = y;

    for (int k = 0; k < 6; k++) {
        for (int i = 0; i < ndim; i++) {
            double sumtemp = 0.0;
            for (int j = 0; j < k; j++) {
                sumtemp += beta45[k][j] * KArr[j][i];
            }
            ymid[i] = y0[i] + sumtemp * h;
        }
        // rightFunc(ndim, t + alph78[k] * h, ymid, &KArr[k * ndim], rkfFuncParam);
        // func(t + alph45[k] * h, ymidvec, kvec);
        if(err_t err = ode.evaluate(t0 + alph45[k] * h, ymid, KArr[k]))
        {
            return err;
        }
    }

    for (int i = 0; i < ndim; i++)
    {
        double sumtemp = 0.0;
        for (int k = 0; k < 5; k++) {
            sumtemp += ch45_4[k] * KArr[k][i];    // 4阶公式
        }
        yf[i] = y0[i] + sumtemp * h;
        wrk.absErrPerLen_[i] = (                            // 每单位长度截断误差估计（4阶相对于5阶）
            - 1./360.      * KArr[0][i]
            + 128./4275.   * KArr[2][i]
            + 2197./75240. * KArr[3][i]
            - 1./50.       * KArr[4][i]
            - 2./55.       * KArr[5][i]
        );
    }
    return eNoError;
}


AST_NAMESPACE_END

