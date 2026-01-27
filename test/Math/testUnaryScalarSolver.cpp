///
/// @file      testUnaryScalarSolver.cpp
/// @brief     测试一元标量值方程求解器
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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

#include "AstMath/SecantSolver.hpp"
#include "AstMath/BrentSolver.hpp"
#include "AstMath/BisectionSolver.hpp"
#include "AstMath/RidderSolver.hpp"
#include "AstTest/Test.hpp"
#include "AstMath/Function.hpp"
#include <cmath>

// 定义π的值，因为C++11不支持std::numbers::pi
#define PI 3.14159265358979323846

using namespace ast;

// 测试函数：f(x) = x - 2
class LinearFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = x - 2.0;
    }
};

// 测试函数：f(x) = x^2 - 4
class QuadraticFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = x * x - 4.0;
    }
};

// 测试函数：f(x) = sin(x)
class SinFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = std::sin(x);
    }
};

// 测试函数：f(x) = exp(x) - 2
class ExpFunction : public UnaryScalarFunc {
public:
    void evaluate(double x, double* out) const override {
        *out = std::exp(x) - 2.0;
    }
};

// 二分法求解器测试
TEST(BisectionSolverTest, LinearFunction) {
    BisectionSolver solver;
    LinearFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(BisectionSolverTest, QuadraticFunction) {
    BisectionSolver solver;
    QuadraticFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(BisectionSolverTest, SinFunction) {
    BisectionSolver solver;
    SinFunction func;
    double result;
    err_t err = solver.solve(func, 0.5, 3.5, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, PI, 1e-10);
}

TEST(BisectionSolverTest, ExpFunction) {
    BisectionSolver solver;
    ExpFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 2.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, std::log(2.0), 1e-10);
}

// Brent法求解器测试
TEST(BrentSolverTest, LinearFunction) {
    BrentSolver solver;
    LinearFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(BrentSolverTest, QuadraticFunction) {
    BrentSolver solver;
    QuadraticFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(BrentSolverTest, SinFunction) {
    BrentSolver solver;
    SinFunction func;
    double result;
    err_t err = solver.solve(func, 0.5, 3.5, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, PI, 1e-10);
}

TEST(BrentSolverTest, ExpFunction) {
    BrentSolver solver;
    ExpFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 2.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, std::log(2.0), 1e-10);
}

// 割线法求解器测试
TEST(SecantSolverTest, LinearFunction) {
    SecantSolver solver;
    LinearFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(SecantSolverTest, QuadraticFunction) {
    SecantSolver solver;
    QuadraticFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(SecantSolverTest, SinFunction) {
    SecantSolver solver;
    SinFunction func;
    double result;
    err_t err = solver.solve(func, 0.5, 3.5, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, PI, 1e-10);
}

TEST(SecantSolverTest, ExpFunction) {
    SecantSolver solver;
    ExpFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 2.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, std::log(2.0), 1e-10);
}

// Ridder法求解器测试
TEST(RidderSolverTest, LinearFunction) {
    RidderSolver solver;
    LinearFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(RidderSolverTest, QuadraticFunction) {
    RidderSolver solver;
    QuadraticFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 5.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, 2.0, 1e-10);
}

TEST(RidderSolverTest, SinFunction) {
    RidderSolver solver;
    SinFunction func;
    double result;
    err_t err = solver.solve(func, 0.5, 3.5, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, PI, 1e-10);
}

TEST(RidderSolverTest, ExpFunction) {
    RidderSolver solver;
    ExpFunction func;
    double result;
    err_t err = solver.solve(func, 0.0, 2.0, result);
    ASSERT_EQ(err, eNoError);
    EXPECT_NEAR(result, std::log(2.0), 1e-10);
}

GTEST_MAIN()