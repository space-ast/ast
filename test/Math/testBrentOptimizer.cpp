///
/// @file      testBrentOptimizer.cpp
/// @brief     测试 Brent 法一维函数极小值求解器
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

#include "ast/BrentOptimizer.hpp"
#include "ast/Function.hpp"
#include "ast/Test.hpp"
#include <cmath>

AST_USING_NAMESPACE

// 测试函数：f(x) = (x - 2)^2
class QuadraticFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = (x - 2.0) * (x - 2.0);
    }
};

// 测试函数：f(x) = -x
class MonotonicFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = -x;
    }
};

TEST(BrentOptimizer, MinimumQuadratic) {
    BrentOptimizer solver;
    QuadraticFunction func;
    double result = 0.0;
    double funcValue = 0.0;
    errc_t err = solver.solve(func, 0.0, 5.0, result, &funcValue);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-6);
    EXPECT_NEAR(funcValue, 0.0, 1e-6);
}

TEST(BrentOptimizer, QuarticMinimum) {
    BrentOptimizer solver;
    auto func = [](double x) -> double { return x * x * x * x - 2.0 * x * x + 1.0; };
    double result = 0.0;
    errc_t err = solver.solve(func, 0.0, 1.5, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 1.0, 1e-6);
}

TEST(BrentOptimizer, MonotonicBoundary) {
    BrentOptimizer solver;
    MonotonicFunction func;
    double result = 0.0;
    errc_t err = solver.solve(func, 0.0, 1.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 1.0, 1e-6);
}

TEST(BrentOptimizer, LambdaOverload) {
    BrentOptimizer solver;
    auto func = [](double x) -> double { return (x - 1.0) * (x - 1.0); };
    double result = 0.0;
    errc_t err = solver.solve(func, -2.0, 4.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 1.0, 1e-6);
}

TEST(BrentOptimizer, FreeFunction) {
    {
        auto func = [](double x) -> double { return (x - 3.0) * (x - 3.0); };
        double result = 0.0;
        errc_t err = aBrentMinimize(func, 0.0, 6.0, result);
        ASSERT_EQ(err, eNoError);
        EXPECT_NEAR(result, 3.0, 1e-6);
    }
    {
        QuadraticFunction func;
        double result = 0.0;
        errc_t err = aBrentMinimize(func, 0.0, 5.0, result);
        ASSERT_EQ(err, eNoError);
        EXPECT_NEAR(result, 2.0, 1e-6);
    }
}

TEST(BrentOptimizer, InvalidBracket) {
    BrentOptimizer solver;
    QuadraticFunction func;
    double result = 0.0;
    errc_t err = solver.solve(func, 5.0, 5.0, result);
    EXPECT_EQ(err, eErrorInvalidParam);
}

GTEST_MAIN()
