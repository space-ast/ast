/// @file      TwoBody.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 根据 Apache 许可证 2.0 版本（"许可证"）授权；
/// 除非符合许可证，否则不得使用此文件。
/// 您可以在以下网址获取许可证副本：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 除非适用法律要求或书面同意，按"原样"分发的软件
/// 没有任何明示或暗示的担保或条件。
/// 请参阅许可证了解特定语言的权限和限制。
 
 
#include "TwoBody.hpp"
#include "AstCore/MathOperator.hpp"
#include "AstCore/Vector.hpp"

#define ORBIT_TYPE_EPS 0  // 0.000001

 
AST_NAMESPACE_BEGIN


namespace details
{
    void c2c3(double psi, double& c2, double& c3)
    {
        if (psi > ORBIT_TYPE_EPS)        // 椭圆/圆轨道
        {
            c2 = (1 - cos(sqrt(psi))) / psi;
            c3 = (sqrt(psi) - sin(sqrt(psi))) / sqrt(psi * psi * psi);
        }
        else if (psi < -ORBIT_TYPE_EPS)  // 双曲线轨道
        {
            c2 = (1 - cosh(sqrt(-psi))) / psi;
            c3 = (sinh(sqrt(-psi)) - sqrt(-psi)) / sqrt(-psi * psi * psi);
        }
        else {
            c2 = 0.5;
            c3 = 1. / 6.;
        }
    }
}

enum ER
{
    eA
};


err_t aTwoBodyProp(double t, double mu, Vector3d& r0, Vector3d& v0)
{
    const double tol = 1e-10;              
    double r0mag = norm(r0);
    double v0mag = norm(v0);
    double alpha = -(v0mag * v0mag) / mu + 2 / r0mag;
    double X0;
    if (alpha > ORBIT_TYPE_EPS)          // 椭圆/圆轨道
    {
        X0 = sqrt(mu) * t * alpha;
    }
    else if (alpha < -ORBIT_TYPE_EPS) {  // 双曲线轨道
        double a = 1 / alpha;
        X0 = sign(t) * sqrt(-a) * log(-2 * mu * alpha * t / (dot(r0, v0) + sign(t) * sqrt(-mu * a) * (1 - r0mag * alpha)));
    }
    else {                               // 抛物线轨道
        auto hmag = norm(cross(r0, v0));
        auto p = (hmag * hmag) / mu;
        auto s = acot(3 * sqrt(mu / (p * p * p)) * t) / 2;
        auto w = atan(pow(tan(s), 1. / 3.));
        X0 = sqrt(p) * 2 * cot(2 * w);
    }
    // 开始迭代
    double err = 1e9;
    auto dr0v0Smu = dot(r0, v0) / sqrt(mu);
    auto Smut = sqrt(mu) * t;
    double Xn, c2, c3, r, psi;
    int niter = 0;
    do {
        auto X02 = X0 * X0;
        auto X03 = X02 * X0;
        psi = X02 * alpha;
        details::c2c3(psi, c2, c3);
        auto X0tOmPsiC3 = X0 * (1 - psi * c3);
        auto X02tC2 = X02 * c2;
        r = X02tC2 + dr0v0Smu * X0tOmPsiC3 + r0mag * (1 - psi * c2);
        Xn = X0 + (Smut - X03 * c3 - dr0v0Smu * X02tC2 - r0mag * X0tOmPsiC3) / r;
        err = Xn - X0;
        X0 = Xn;
        niter++;
    } while (abs(err) > tol && niter < 100);
    auto f = 1 - (Xn * Xn) * c2 / r0mag;
    auto g = t - (Xn * Xn * Xn) * c3 / sqrt(mu);
    auto gdot = 1 - c2 * (Xn * Xn) / r;
    auto fdot = Xn * (psi * c3 - 1) * sqrt(mu) / (r * r0mag);
    auto posnew = f * r0 + g * v0;    // 先不覆盖pos的数据，以免r0与pos地址相同，后面计算还需要用r0
    v0 = fdot * r0 + gdot * v0;
    r0 = posnew;
    return eNoError;
}


static_assert(eNoError == 0, "enum eNoError must eq 0");

AST_NAMESPACE_END
 
