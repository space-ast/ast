///
/// @file      GoldenSectionOptimizer.cpp
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

#include "GoldenSectionOptimizer.hpp"
#include <cmath>
#include <limits>

AST_NAMESPACE_BEGIN

namespace
{
    /// @brief 调用一元标量函数并返回函数值
    inline double evaluate(const UnaryScalarFunc& func, double x)
    {
        double value = 0.0;
        func.evaluate(x, &value);
        return value;
    }
}

errc_t GoldenSectionOptimizer::solve(UnaryScalarFunc& func, double min, double max, double& result, double* funcValue)
{
    stats_ = {};
    if (min >= max) {
        stats_.error_num = eErrorInvalidParam;
        result = std::numeric_limits<double>::quiet_NaN();
        if (funcValue != nullptr) { *funcValue = std::numeric_limits<double>::quiet_NaN(); }
        return eErrorInvalidParam;
    }

    // 黄金分割比例（共轭）：(√5 - 1) / 2
    const double phi = (std::sqrt(5.0) - 1.0) / 2.0;

    double a = min;
    double b = max;
    double c = b - phi * (b - a);
    double d = a + phi * (b - a);
    double fc = evaluate(func, c);
    double fd = evaluate(func, d);
    stats_.funcalls = 2;

    int iter = 0;
    double tol = 0.0;
    while (iter < maxIter_) {
        const double mid = 0.5 * (a + b);
        tol = absTol_ + relTol_ * fabs(mid);
        if (fabs(b - a) <= tol) {
            break;
        }
        if (fc < fd) {
            b = d;
            d = c;
            fd = fc;
            c = b - phi * (b - a);
            fc = evaluate(func, c);
        } else {
            a = c;
            c = d;
            fc = fd;
            d = a + phi * (b - a);
            fd = evaluate(func, d);
        }
        ++stats_.funcalls;
        ++iter;
    }

    stats_.iterations = iter;
    result = 0.5 * (a + b);
    if (funcValue != nullptr) {
        *funcValue = evaluate(func, result);
        ++stats_.funcalls;
    }

    stats_.error_num = (iter >= maxIter_ && fabs(b - a) > tol) ? eErrorMaxIter : eNoError;
    return stats_.error_num;
}

errc_t aGoldenSectionMinimize(UnaryScalarFunc& func, double min, double max, double& result,
                              double* funcValue, double absTol, double relTol, int maxIter)
{
    GoldenSectionOptimizer solver(relTol, absTol, maxIter);
    return solver.solve(func, min, max, result, funcValue);
}

AST_NAMESPACE_END
