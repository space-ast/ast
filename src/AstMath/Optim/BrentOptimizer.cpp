///
/// @file      BrentOptimizer.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-08-17
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

#include "BrentOptimizer.hpp"
#include "GoldenSectionOptimizer.hpp"
#include <cmath>
#include <limits>

AST_NAMESPACE_BEGIN

namespace
{
    /// @brief 调用一元标量函数并返回函数值
    inline double evaluateBrent(const UnaryScalarFunc& func, double x)
    {
        double value = 0.0;
        func.evaluate(x, &value);
        return value;
    }
}

errc_t BrentOptimizer::solve(UnaryScalarFunc& func, double min, double max, double& result, double* funcValue)
{
    stats_ = {};
    if (min >= max) {
        stats_.error_num = eErrorInvalidParam;
        result = std::numeric_limits<double>::quiet_NaN();
        if (funcValue != nullptr) { *funcValue = std::numeric_limits<double>::quiet_NaN(); }
        return eErrorInvalidParam;
    }

    // 黄金分割共轭比例：用于构造初始内点
    const double cgold = (3.0 - std::sqrt(5.0)) * 0.5;

    double a = min;
    double b = max;
    double x = a + cgold * (b - a);
    double fa = evaluateBrent(func, a);
    double fx = evaluateBrent(func, x);
    double fb = evaluateBrent(func, b);
    stats_.funcalls = 3;

    // 内点并非最低：最小值靠近区间端点（单调或近端），退回黄金分割收缩
    if (fx > fa || fx > fb) {
        GoldenSectionOptimizer golden(relTol_, absTol_, maxIter_);
        errc_t err = golden.solve(func, a, b, result, funcValue);
        const SolverStats& gs = golden.getStats();
        stats_.funcalls += gs.funcalls;   // 保留初始 3 次采样
        stats_.iterations = gs.iterations;
        stats_.error_num = gs.error_num;
        return err;
    }

    // Brent 迭代：反抛物线插值 + 黄金分割兜底
    double w = x;
    double v = x;
    double fw = fx;
    double fv = fx;
    double e = 0.0;   ///< 上一步移动距离
    double d = 0.0;   ///< 当前步长

    int iter = 0;
    bool converged = false;
    while (iter < maxIter_) {
        const double xm = 0.5 * (a + b);
        const double tol1 = absTol_ + relTol_ * std::fabs(x);
        const double tol2 = 2.0 * tol1;
        if (std::fabs(x - xm) <= tol2 - 0.5 * (b - a)) {
            converged = true;
            break;
        }

        if (std::fabs(e) > tol1) {
            // 反抛物线插值：过 (x, w, v) 三点拟合抛物线求极小点
            const double r = (x - w) * (fx - fv);
            const double q1 = (x - v) * (fx - fw);
            double p = (x - v) * q1 - (x - w) * r;
            double q2 = 2.0 * (q1 - r);
            if (q2 > 0.0) {
                p = -p;
            }
            q2 = std::fabs(q2);
            const double etemp = e;
            e = d;
            if (std::fabs(p) >= std::fabs(0.5 * q2 * etemp) || p <= q2 * (a - x) || p >= q2 * (b - x)) {
                // 抛物线步长不合格，改用黄金分割
                e = (x >= xm) ? a - x : b - x;
                d = cgold * e;
            } else {
                d = p / q2;
                const double u = x + d;
                if (u - a < tol2 || b - u < tol2) {
                    d = (xm >= x) ? tol1 : -tol1;
                }
            }
        } else {
            e = (x >= xm) ? a - x : b - x;
            d = cgold * e;
        }

        const double u = (std::fabs(d) >= tol1) ? (x + d) : (x + ((d >= 0.0) ? tol1 : -tol1));
        const double fu = evaluateBrent(func, u);
        ++stats_.funcalls;

        if (fu <= fx) {
            if (u >= x) {
                a = x;
            } else {
                b = x;
            }
            v = w; fv = fw;
            w = x; fw = fx;
            x = u; fx = fu;
        } else {
            if (u < x) {
                a = u;
            } else {
                b = u;
            }
            if (fu <= fw || w == x) {
                v = w; fv = fw;
                w = u; fw = fu;
            } else if (fu <= fv || v == x || v == w) {
                v = u; fv = fu;
            }
        }
        ++iter;
    }

    stats_.iterations = iter;
    result = x;
    if (funcValue != nullptr) {
        *funcValue = evaluateBrent(func, result);
        ++stats_.funcalls;
    }

    stats_.error_num = converged ? eNoError : eErrorMaxIter;
    return stats_.error_num;
}

errc_t aBrentMinimize(UnaryScalarFunc& func, double min, double max, double& result,
                      double* funcValue, double absTol, double relTol, int maxIter)
{
    BrentOptimizer solver(relTol, absTol, maxIter);
    return solver.solve(func, min, max, result, funcValue);
}

AST_NAMESPACE_END
