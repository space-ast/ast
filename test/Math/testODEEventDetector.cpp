///
/// @file      testODEEventDetector.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-28
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
#include "AstMath/ODEEventDetector.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

// 简单的指数衰减ODE: dy/dt = -y
class SimpleODE : public ODE {
public:
    err_t evaluate(const double* y, double* ydot, double t) override {
        ydot[0] = -y[0];
        return eNoError;
    }

    int getDimension() const override {
        return 1;
    }
};

// 简单的简谐振动ODE: d²x/dt² = -x
class HarmonicOscillatorODE : public ODE {
public:
    err_t evaluate(const double* y, double* ydot, double t) override {
        // y[0] = x, y[1] = dx/dt
        ydot[0] = y[1];
        ydot[1] = -y[0];
        return eNoError;
    }

    int getDimension() const override {
        return 2;
    }
};

// 检测y值等于特定值的事件检测器
class SimpleEventDetector : public ODEEventDetector {
public:
    SimpleEventDetector(double targetValue, EDirection direction = eBoth)
    {
        this->setDirection(direction);
        this->setThreshold(1e-10);
        this->setGoal(targetValue);
    }

    double getValue(const double* y, double x) const override {
        return y[0];
    }

private:
};

// 检测简谐振动达到最大值的事件检测器
class HarmonicMaxEventDetector : public ODEEventDetector {
public:
    HarmonicMaxEventDetector() {
        this->setDirection(eDecrease); // 当速度从正变负时触发（最大值）
        this->setThreshold(1e-10);
        this->setGoal(0.0); // 检测速度为0的点
    }

    double getValue(const double* y, double x) const override {
        return y[1]; // 返回速度
    }
};

// 测试事件检测器的基本功能
TEST(ODEEventDetector, BasicFunctionality) {
    SimpleODE ode;
    SimpleEventDetector detector(0.5); // 检测y=0.5的事件

    // 测试getValue方法
    double y[1] = {0.5};
    EXPECT_DOUBLE_EQ(detector.getValue(y, 0.0), 0.5);

    // 测试getDifference方法
    EXPECT_DOUBLE_EQ(detector.getDifference(y, 0.0), 0.0);

    y[0] = 0.6;
    EXPECT_DOUBLE_EQ(detector.getDifference(y, 0.0), 0.1);

    y[0] = 0.4;
    EXPECT_DOUBLE_EQ(detector.getDifference(y, 0.0), -0.1);
}

// 测试事件检测器的方向设置
TEST(ODEEventDetector, DirectionSetting) {
    SimpleODE ode;
    SimpleEventDetector detector(0.5, ODEEventDetector::eIncrease);
    EXPECT_EQ(detector.getDirection(), ODEEventDetector::eIncrease);

    SimpleEventDetector detector2(0.5, ODEEventDetector::eDecrease);
    EXPECT_EQ(detector2.getDirection(), ODEEventDetector::eDecrease);

    SimpleEventDetector detector3(0.5, ODEEventDetector::eBoth);
    EXPECT_EQ(detector3.getDirection(), ODEEventDetector::eBoth);
}

// 测试将事件检测器添加到积分器
TEST(ODEEventDetector, AddToIntegrator) {
    RKF45 integrator;
    // 使用堆上创建的事件检测器对象，因为ODEEventObserver会管理其生命周期
    SimpleEventDetector* detector = new SimpleEventDetector(0.5);

    // 测试添加事件检测器
    integrator.addEventDetector(detector);

    // 测试移除事件检测器
    // 注意：当调用removeEventDetector时，ODEEventObserver会自动删除detector指针
    integrator.removeEventDetector(detector);
}

// 测试简谐振动的事件检测
TEST(ODEEventDetector, HarmonicOscillatorEvent) {
    HarmonicOscillatorODE ode;
    HarmonicMaxEventDetector detector;

    // 初始条件：x=0, v=1（速度最大，位置为0）
    double y[2] = {0.0, 1.0};
    double t = 0.0;

    // 测试在初始位置的事件值
    EXPECT_DOUBLE_EQ(detector.getValue(y, t), 1.0);
    EXPECT_DOUBLE_EQ(detector.getDifference(y, t), 1.0);

    // 当达到最高点时，速度应该为0
    // 解析解：x = sin(t), v = cos(t)
    // 在t=π/2时，x=1, v=0
    t = 1.5707963267948966; // π/2
    y[0] = 1.0; // sin(π/2)
    y[1] = 0.0; // cos(π/2)
    EXPECT_DOUBLE_EQ(detector.getValue(y, t), 0.0);
    EXPECT_DOUBLE_EQ(detector.getDifference(y, t), 0.0);
}

// 测试带事件检测器的积分过程
TEST(ODEEventDetector, IntegrationWithEventDetector) {
    SimpleODE ode;
    RKF45 integrator;
    
    // 初始条件：y(0) = 1.0
    double y[1] = {1.0};
    double t = 0.0;
    double tf = 1.0; // 积分到t=1.0
    
    // 创建事件检测器，检测y=0.5的事件
    // 由于SimpleODE的解是y(t) = e^(-t)，所以在t=ln(2)≈0.693时，y值将达到0.5
    SimpleEventDetector* detector = new SimpleEventDetector(0.5);
    
    // 将事件检测器添加到积分器
    integrator.addEventDetector(detector);
    
    // 执行积分
    err_t result = integrator.integrate(ode, y, t, tf);
    
    // 验证积分成功
    EXPECT_EQ(result, eNoError);
    
    // 验证事件检测时间
    EXPECT_NEAR(t, log(2.0), 1e-6);
    
    // 移除事件检测器
    integrator.removeEventDetector(detector);
}

GTEST_MAIN()