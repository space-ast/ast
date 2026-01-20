///
/// @file      RKF78.cpp
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

#include "RKF78.hpp"

AST_NAMESPACE_BEGIN


static const double
    ch78_8[13]   { 0.      ,  0.    , 0.     ,	0.   , 0.    , 34./105. , 9./35., 9./35.,  9./280. ,  9./280., 0.      , 41./840., 41./840. },
    ch78_7[13]   { 41./840.,  0.    , 0.     ,	0.   , 0.    , 34./105. , 9./35., 9./35.,  9./280. ,  9./280., 41./840., 0.      , 0.       },
    alph78[13]   { 0.      ,  2./27., 1./9.  ,	1./6., 5./12., 1.0/2.0  , 5./6. , 1./ 6.,  2.0/3.0 , 1.0/3.0 , 1.      , 0.      , 1.       },
    beta78[13][12]{
        {},
        { 2.0 / 27.0	 , 0.       , 0.       , 0.         , 0.	       , 0.        , 0.	          , 0.      , 0.       , 0.      , 0. ,    0. },
        { 1. / 36		 , 1./12.   , 0.       , 0.         , 0.	       , 0.        , 0.	          , 0.      , 0.       , 0.      , 0. ,    0. },
        { 1. / 24.	     , 0.	    , 1./8.    , 0.         , 0.	       , 0.        , 0.	          , 0.      , 0.       , 0.      , 0. ,    0. },
        { 5. / 12.	     , 0.	    , -25./16. , 25./16.    , 0.           , 0.        , 0.	          , 0.      , 0.       , 0.      , 0. ,    0. },
        { 1. / 20.	     , 0.	    , 0.       , 1./4.      , 1. / 5.      , 0.        , 0.           , 0.      , 0.       , 0.      , 0. ,    0. },
        { -25. / 108.	 , 0.	    , 0.       , 125./108.  , -65./ 27.    , 125./54.  , 0.	          , 0.      , 0.       , 0.      , 0. ,    0. },
        { 31. / 300.	 , 0.	    , 0.       , 0.         , 61. / 225.   , -2./9.    , 13. / 900.   , 0.      , 0.       , 0.      , 0. ,    0. },
        { 2.		     , 0.	    , 0.       , -53./6.    , 704. /45.    , -107./9.  , 67. / 90.    , 3.      , 0.       , 0.      , 0. ,    0. },
        { -91. / 108.	 , 0.	    , 0.       , 23./108.   , -976./135.   , 311./54.  , -19./60.     , 17./6.  , -1./12.  , 0.      , 0. ,    0. },
        { 2383.0 / 4100. , 0.	    , 0.       , -341./164. , 4496./1025.  , -301./82. , 2133./4100.  , 45./82. , 45./164. , 18./41. , 0. ,    0. },
        { 3. / 205.	     , 0.	    , 0.       ,0.          , 0.           , -6./41.   , -3./205.     , -3./41. ,  3./41.  , 6.0/41. , 0. ,    0. },
        { -1777. / 4100. , 0.	    , 0.       , -341./164. , 4496./1025.  , -289./82. , 2193./4100.  , 51./82. , 33./164. , 12./41. , 0. ,    1. } };

const double err_factor = -41.0 / 840.0;
const int num_stage_rkf78 = 13;

err_t RKF78::initialize(ODE &ode)
{
    // 重置工作空间
    this->getWorkspace().reset(ode.getDimension(), num_stage_rkf78);
    return eNoError;
}

err_t RKF78::singleStep(ODE &ode, double t0, double h, const double *y0, double *yf)
{
    auto& wrk = getWorkspace();
    auto KArr = wrk.KArr_;
    auto absErrPerLen = wrk.absErrPerLen_;
    double* ymid = wrk.ymid_;
    int ndim = wrk.dimension_;
    
    for (int k = 0; k < num_stage_rkf78; k++) {
        for (int i = 0; i < ndim; i++) {
            double sumtemp = 0.0;
            for (int j = 0; j < k; j++) {
                sumtemp += beta78[k][j] * KArr[j][i];
            }
            ymid[i] = y0[i] + sumtemp * h;
        }
        //func(, ymidvec, kvec);
        //rightFunc(ndim, t + alph78[k] * h, ymid, &KArr[k * ndim], rkfFuncParam);
        if (err_t err = ode.evaluate(t0 + alph78[k] * h, ymid, KArr[k])) {
            return err;
        }
    }
    for (int i = 0; i < ndim; i++)
    {
        double sumtemp = 0.0;
        for (int k = 0; k < num_stage_rkf78; k++) {
            // ssumtemp += ch78_7[k] * wrk.KArr_[k][i];    // 7阶公式
            sumtemp += ch78_8[k] * KArr[k][i];    // 8阶公式
        }
        yf[i] = y0[i] + sumtemp * h;
        absErrPerLen[i] = err_factor * (                            // 每单位长度截断误差估计（7阶相对于8阶）
            + KArr[0][i]
            + KArr[10][i]
            - KArr[11][i]
            - KArr[12][i]
        );
    }
    return eNoError;
}


AST_NAMESPACE_END

