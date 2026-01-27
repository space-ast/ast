///
/// @file      RKCK.cpp
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

#include "RKCK.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

err_t RKCK::initialize(ODE &ode)
{
    // 重置工作空间
    this->getWorkspace().reset(ode.getDimension(), 6);
    return eNoError;
}



err_t RKCK::singleStep(ODE &ode, double* y, double t0, double h)
{
    /* Cash-Karp 方法系数 */
    static const double ah[] = { 1.0 / 5.0, 0.3, 3.0 / 5.0, 1.0, 7.0 / 8.0 };
    static const double b21  = 1.0 / 5.0;
    static const double b3[] = { 3.0 / 40.0, 9.0 / 40.0 };
    static const double b4[] = { 0.3, -0.9, 1.2 };
    static const double b5[] = { -11.0 / 54.0, 2.5, -70.0 / 27.0, 35.0 / 27.0 };
    static const double b6[] =
    { 1631.0 / 55296.0, 175.0 / 512.0, 575.0 / 13824.0, 44275.0 / 110592.0,
      253.0 / 4096.0 };
    static const double c1 = 37.0 / 378.0;
    static const double c3 = 250.0 / 621.0;
    static const double c4 = 125.0 / 594.0;
    static const double c6 = 512.0 / 1771.0;

    /* 五阶和四阶系数差，给误差估计使用 */

    static const double ec[] = { 0.0,
      37.0 / 378.0 - 2825.0 / 27648.0,
      0.0,
      250.0 / 621.0 - 18575.0 / 48384.0,
      125.0 / 594.0 - 13525.0 / 55296.0,
      -277.0 / 14336.0,
      512.0 / 1771.0 - 0.25
    };
    
    
    auto& wrk = this->getWorkspace();
    const int ndim = wrk.dimension_;

    auto KArr = wrk.KArr_;
    double
        * f1 = KArr[0],
        * f2 = KArr[1],
        * f3 = KArr[2],
        * f4 = KArr[3],
        * f5 = KArr[4],
        * f6 = KArr[5];
    double* ymid = wrk.ymid_;

    const double* y0 = y;
    double* yf = y;

    err_t err;

    // f1
    err = ode.evaluate(t0, y0, f1);
    
    // f2
    for (int i = 0; i < ndim; i++) {
        ymid[i] = y0[i] + h * ( b21*f1[i] );
    }
    err |= ode.evaluate(t0 + ah[0] * h, ymid, f2);
    
    // f3
    for (int i = 0; i < ndim; i++) {
        ymid[i] = y0[i] + h * (b3[0] * f1[i] + b3[1] * f2[i]);
    }
    err |= ode.evaluate(t0 + ah[1] * h, ymid, f3);

    // f4
    for (int i = 0; i < ndim; i++) {
        ymid[i] = y0[i] + h * (b4[0] * f1[i] + b4[1] * f2[i] + b4[2] * f3[i]);
    }
    err |= ode.evaluate(t0 + ah[2] * h, ymid, f4);
    
    // f5
    for (int i = 0; i < ndim; i++) {
        ymid[i] = y0[i] + h * (b5[0] * f1[i] + b5[1] * f2[i] + b5[2] * f3[i] + b5[3] * f4[i]);
    }
    err |= ode.evaluate(t0 + ah[3] * h, ymid, f5);

    // f6
    for (int i = 0; i < ndim; i++) {
        ymid[i] = y0[i] + h * (b6[0] * f1[i] + b6[1] * f2[i] + b6[2] * f3[i] + b6[3] * f4[i] + b6[4] * f5[i]);
    }
    err |= ode.evaluate(t0 + ah[4] * h, ymid, f6);

    for (int i = 0; i < ndim; i++)
    {
        yf[i] = y0[i] + h * (c1 * f1[i] + c3 * f3[i] + c4 * f4[i] + c6 * f6[i]);
        wrk.absErrPerLen_[i] = (                            // 每单位长度截断误差估计（4阶相对于5阶）
            + ec[1] * KArr[0][i]
            + ec[3] * KArr[2][i]
            + ec[4] * KArr[3][i]
            + ec[5] * KArr[4][i]
            + ec[6] * KArr[5][i]
        );
    }
    return err;
}


AST_NAMESPACE_END
