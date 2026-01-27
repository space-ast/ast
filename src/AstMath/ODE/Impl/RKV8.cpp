///
/// @file      RKV8.cpp
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

#include "RKV8.hpp"
#include <cmath>


AST_NAMESPACE_BEGIN

err_t RKV8::initialize(ODE &ode)
{
    // 重置工作空间
    this->getWorkspace().reset(ode.getDimension(), 11);
    return eNoError;
}


err_t RKV8::singleStep(ODE &ode, double* y, double t0, double h)
{
    const double sqrt21 = sqrt(21.0);

    const double c1 = 1.0 / 2.0;
    const double c2 = (7.0 - sqrt21) / 14.0;
    const double c3 = (7.0 + sqrt21) / 14.0;

    const double a21 = 1.0 / 2.0;
    const double a31 = 1.0 / 4.0;
    const double a32 = 1.0 / 4.0;
    const double a41 = 1.0 / 7.0;
    const double a42 = (-7.0 + 3.0 * sqrt21) / 98.0;
    const double a43 = (21.0 - 5.0 * sqrt21) / 49.0;
    const double a51 = (11.0 - sqrt21) / 84.0;
    const double a53 = (18.0 - 4.0 * sqrt21) / 63.0;
    const double a54 = (21.0 + sqrt21) / 252.0;
    const double a61 = (5.0 - sqrt21) / 48.0;
    const double a63 = (9.0 - sqrt21) / 36.0;
    const double a64 = (-231.0 - 14.0 * sqrt21) / 360.0;
    const double a65 = (63.0 + 7.0 * sqrt21) / 80.0;
    const double a71 = (10.0 + sqrt21) / 42.0;
    const double a73 = (-432.0 - 92.0 * sqrt21) / 315.0;
    const double a74 = (633.0 + 145.0 * sqrt21) / 90.0;
    const double a75 = (-504.0 - 115.0 * sqrt21) / 70.0;
    const double a76 = (63.0 + 13.0 * sqrt21) / 35.0;
    const double a81 = 1.0 / 14.0;
    const double a85 = (14.0 + 3.0 * sqrt21) / 126.0;
    const double a86 = (13.0 + 3.0 * sqrt21) / 63.0;
    const double a87 = 1.0 / 9.0;
    const double a91 = 1.0 / 32.0;
    const double a95 = (91.0 + 21.0 * sqrt21) / 576.0;
    const double a96 = 11.0 / 72.0;
    const double a97 = (-385.0 + 75.0 * sqrt21) / 1152.0;
    const double a98 = (63.0 - 13.0 * sqrt21) / 128.0;
    const double a10_1 = 1.0 / 14.0;
    const double a10_5 = 1.0 / 9.0;
    const double a10_6 = (-733.0 + 147.0 * sqrt21) / 2205.0;
    const double a10_7 = (515.0 - 111.0 * sqrt21) / 504.0;
    const double a10_8 = (-51.0 + 11.0 * sqrt21) / 56.0;
    const double a10_9 = (132.0 - 28.0 * sqrt21) / 245.0;
    const double a11_5 = (-42.0 - 7.0 * sqrt21) / 18.0;
    const double a11_6 = (-18.0 - 28.0 * sqrt21) / 45.0;
    const double a11_7 = (-273.0 + 53.0 * sqrt21) / 72.0;
    const double a11_8 = (301.0 - 53.0 * sqrt21) / 72.0;
    const double a11_9 = (28.0 + 28.0 * sqrt21) / 45.0;
    const double a11_10 = (49.0 + 7.0 * sqrt21) / 18.0;

    const double  b1 = 9.0 / 180.0;
    const double  b8 = 49.0 / 180.0;
    const double  b9 = 64.0 / 180.0;


#if 0
#define EPS 1e-10
    assert(c1 == a21);
    assert(c1 == a31 + a32);
    assert(abs(c2 - (a41 + a42 + a43)) < EPS);
    assert(abs(c2 - (a51 + a53 + a54)) < EPS);
    assert(c1 == a61 + a63 + a64 + a65);
    assert(abs(c3 - (a71 + a73 + a74 + a75 + a76)) < EPS);
    assert(abs(c3 - (a81 + a85 + a86 + a87)) < EPS);
    assert(abs(c1 - (a91 + a95 + a96 + a97 + a98)) < EPS);
    assert(abs(c2 - (a10_1 + a10_5 + a10_6 + a10_7 + a10_8 + a10_9)) < EPS);
    assert(abs(1 - (a11_5 + a11_6 + a11_7 + a11_8 + a11_9 + a11_10)) < EPS);
    assert(abs(1 - (b1 + b8 + b9 + b8 + b1)) < EPS);
#undef EPS
#endif
    auto& wrk = this->getWorkspace();
    int ndim = wrk.dimension_;
    double* k1 = wrk.KArr_[0];
    double* k2 = wrk.KArr_[1];
    double* k3 = wrk.KArr_[2];
    double* k4 = wrk.KArr_[3];
    double* k5 = wrk.KArr_[4];
    double* k6 = wrk.KArr_[5];
    double* k7 = wrk.KArr_[6];
    double* k8 = wrk.KArr_[7];
    double* k9 = wrk.KArr_[8];
    double* k10 = wrk.KArr_[9];
    double* k11 = wrk.KArr_[10];
    double* ymid = wrk.ymid_;

    double c1h = c1 * h, c2h = c2 * h, c3h = c3 * h;

    const double* y0 = y;
    double* yf = y;

    err_t err;
    // 计算k1
    err = ode.evaluate(t0, y0, k1);
    // 计算k2
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a21 * k1[i]);
    err |= ode.evaluate(t0 + c1h, ymid, k2);
    // 计算k3
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a31 * k1[i] + a32 * k2[i]);
    err |= ode.evaluate(t0 + c1h, ymid, k3);
    // 计算k4
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a41 * k1[i] + a42 * k2[i] + a43 * k3[i]);
    err |= ode.evaluate(t0 + c2h, ymid, k4);
    // 计算k5
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a51 * k1[i] + a53 * k3[i] + a54 * k4[i]);
    err |= ode.evaluate(t0 + c2h, ymid, k5);
    // 计算k6
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a61 * k1[i] + a63 * k3[i] + a64 * k4[i] + a65 * k5[i]);
    err |= ode.evaluate(t0 + c1h, ymid, k6);
    // 计算k7
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a71 * k1[i] + a73 * k3[i] + a74 * k4[i] + a75 * k5[i] + a76 * k6[i]);
    err |= ode.evaluate(t0 + c3h, ymid, k7);
    // 计算k8
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a81 * k1[i] + a85 * k5[i] + a86 * k6[i] + a87 * k7[i]);
    err |= ode.evaluate(t0 + c3h, ymid, k8);
    // 计算k9
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a91 * k1[i] + a95 * k5[i] + a96 * k6[i] + a97 * k7[i] + a98 * k8[i]);
    err |= ode.evaluate(t0 + c1h, ymid, k9);
    // 计算k10
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a10_1 * k1[i] + a10_5 * k5[i] + a10_6 * k6[i] + a10_7 * k7[i] + a10_8 * k8[i] + a10_9 * k9[i]);
    err |= ode.evaluate(t0 + c2h, ymid, k10);
    // 计算k11
    for(int i=0; i<ndim; i++)
        ymid[i] = y0[i] + h * (a11_5 * k5[i] + a11_6 * k6[i] + a11_7 * k7[i] + a11_8 * k8[i] + a11_9 * k9[i] + a11_10 * k10[i]);
    err |= ode.evaluate(t0 + h, ymid, k11);

    // 计算yf
    for(int i=0; i<ndim; i++)
        yf[i] = y0[i] + (b1 * k1[i] + b8 * k8[i] + b9 * k9[i] + b8 * k10[i] + b1 * k11[i]) * h;

    return err;
    /*
    double c1h = c1 * h, c2h = c2 * h, c3h = c3 * h;
    func(t0, y0, k1);
    func(t0 + c1h, y0 + h * (a21 * k1), k2);
    func(t0 + c1h, y0 + h * (a31 * k1 + a32 * k2), k3);
    func(t0 + c2h, y0 + h * (a41 * k1 + a42 * k2 + a43 * k3), k4);
    func(t0 + c2h, y0 + h * (a51 * k1 + a53 * k3 + a54 * k4), k5);
    func(t0 + c1h, y0 + h * (a61 * k1 + a63 * k3 + a64 * k4 + a65 * k5), k6);
    func(t0 + c3h, y0 + h * (a71 * k1 + a73 * k3 + a74 * k4 + a75 * k5 + a76 * k6), k7);
    func(t0 + c3h, y0 + h * (a81 * k1 + a85 * k5 + a86 * k6 + a87 * k7), k8);
    func(t0 + c1h, y0 + h * (a91 * k1 + a95 * k5 + a96 * k6 + a97 * k7 + a98 * k8), k9);
    func(t0 + c2h, y0 + h * (a10_1 * k1 + a10_5 * k5 + a10_6 * k6 + a10_7 * k7 + a10_8 * k8 + a10_9 * k9), k10);
    func(t0 + h, y0 + h * (a11_5 * k5 + a11_6 * k6 + a11_7 * k7 + a11_8 * k8 + a11_9 * k9 + a11_10 * k10), k11);
    y0 += (b1 * k1 + b8 * k8 + b9 * k9 + b8 * k10 + b1 * k11) * h;
    */
}


AST_NAMESPACE_END
