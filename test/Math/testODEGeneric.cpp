///
/// @file      testODEGeneric.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-29
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

#include "AstMath/ODE.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(ODEGeneric, integrate) {
    auto ode = make_ode(1, [](const double* y, double* dy, const double t) -> err_t {
        dy[0] = -y[0];
        return eNoError;
    });

    EXPECT_EQ(ode.getDimension(), 1);
    RKF78 integrator;
    // 设置初始条件
    double t0 = 0.0;
    double y0[1] = {1.0};
    
    // 积分到t=1.0
    double tf = 1.0;
    double t = t0;
    double y[1]{y0[0]};
    
    // 执行积分
    err_t result = integrator.integrate(
        1, [](const double* y, double* dy, const double t) -> err_t {
            dy[0] = -y[0];
            return eNoError;
        }, 
        y, t, tf
    );
    
    // 检查是否成功
    EXPECT_EQ(result, eNoError);
    
    // 计算解析解
    double analyticalSolution = exp(-tf);
    
    // 检查数值解与解析解的误差
    EXPECT_NEAR(y[0], analyticalSolution, 1e-8);
    EXPECT_EQ(t, tf);
}



TEST(ODEGeneric, EventDetector) {
    RKF78 integrator;
    // 设置初始条件
    double t0 = 0.0;
    double y0[1] = {1.0};
    
    double tf = 10.0;
    double t = t0;
    double y[1]{y0[0]};
    
    double value = 0.456;

    // 添加事件检测器
    integrator.addEventDetector([=](const double* y, const double t) -> double {
        return y[0] - value;
    });

    auto ode = make_ode(1, [](const double* y, double* dy, const double t) -> err_t {
        dy[0] = -y[0];
        return eNoError;
    });

    // 执行积分
    err_t result = integrator.integrate(ode, y, t, tf);

    // 检查是否成功
    EXPECT_EQ(result, eNoError);
    
    // 计算解析解
    // double analyticalSolution = exp(-tf);
    
    // 检查数值解与解析解的误差
    EXPECT_NEAR(y[0], value, 1e-10);
    EXPECT_NEAR(t, log(1./value), 1e-10);
}

GTEST_MAIN();