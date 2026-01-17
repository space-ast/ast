///
/// @file      testODE.cpp
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

#include "AstMath/RK4.hpp"
#include "AstMath/RK8.hpp"
#include "AstMath/RKV8.hpp"
#include "AstMath/RKF45.hpp"
#include "AstMath/RKF56.hpp"
#include "AstMath/RKF78.hpp"
#include "AstMath/RKCK.hpp"
#include "AstTest/Test.h"

using namespace ast;


// 定义一个简单的ODE类：dy/dt = -y
class SimpleODE : public ast::ODE {
public:
    int getDimension() const override {
        return 1;
    }
    
    ast::err_t evaluate(const double t, const double* y, double* dy) const override {
        (void)t; // 未使用的参数
        dy[0] = -y[0];
        return ast::eNoError;
    }
};

// 通用测试函数 - 可变步长积分器
template <typename IntegratorType>
void TestVarStepIntegrator(IntegratorType& integrator, double maxAbsErr, double tolerance) {
    // 创建ODE实例
    SimpleODE ode;
    
    // 设置积分器参数
    integrator.setMaxAbsErr(maxAbsErr);
    integrator.setMaxRelErr(1e-13);
    
    // 设置初始条件
    double t0 = 0.0;
    double y0[1] = {1.0};
    
    // 积分到t=1.0
    double tf = 1.0;
    double yf[1];
    
    // 执行积分
    err_t result = integrator.integrate(ode, t0, tf, y0, yf);
    
    // 检查是否成功
    EXPECT_EQ(result, eNoError);
    
    // 计算解析解
    double analyticalSolution = exp(-tf);
    
    // 检查数值解与解析解的误差
    EXPECT_NEAR(yf[0], analyticalSolution, tolerance);

    printf("largestStepSize_ = %f\n", integrator.getLargestStepSize());
    printf("smallestStepSize_ = %f\n", integrator.getSmallestStepSize());
    printf("numSteps_ = %d\n", integrator.getNumSteps());
}

// 通用测试函数 - 固定步长积分器
template <typename IntegratorType>
void TestFixedStepIntegrator(IntegratorType& integrator, double stepSize, double tolerance) {
    // 创建ODE实例
    SimpleODE ode;
    
    // 设置积分器参数
    integrator.setStepSize(stepSize);
    
    // 设置初始条件
    double t0 = 0.0;
    double y0[1] = {1.0};
    
    // 积分到t=1.0
    double tf = 1.0;
    double yf[1];
    
    // 执行积分
    err_t result = integrator.integrate(ode, t0, tf, y0, yf);
    
    // 检查是否成功
    EXPECT_EQ(result, eNoError);
    
    // 计算解析解
    double analyticalSolution = exp(-tf);
    
    // 检查数值解与解析解的误差
    EXPECT_NEAR(yf[0], analyticalSolution, tolerance);
}


TEST(ODE, RKF78)
{
    RKF78 integrator;
    TestVarStepIntegrator(integrator, 1e-14, 1e-12);
}

TEST(ODE, RKF45)
{
    RKF45 integrator;
    TestVarStepIntegrator(integrator, 1e-10, 1e-8);
}

TEST(ODE, RKF56)
{
    RKF56 integrator;
    TestVarStepIntegrator(integrator, 1e-12, 1e-10);
}

TEST(ODE, RKCK)
{
    RKCK integrator;
    TestVarStepIntegrator(integrator, 1e-12, 1e-10);
}

TEST(ODE, RK4)
{
    RK4 integrator;
    TestFixedStepIntegrator(integrator, 1e-5, 1e-12); 
}

TEST(ODE, RK8)
{
    RK8 integrator;
    TestFixedStepIntegrator(integrator, 1e-2, 1e-12); 
}

TEST(ODE, RKV8)
{
    RKV8 integrator;
    TestFixedStepIntegrator(integrator, 1e-2, 1e-12); 
}


GTEST_MAIN();