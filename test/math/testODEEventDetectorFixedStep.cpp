///
/// @file      testODEEventDetectorFixedStep.cpp
/// @brief     固定步长积分模式下事件检测精确根查找测试
/// @details   验证 ODEIntegrator::integrateFixedStep 在固定步长下能对事件做精确根查找，
/// @author    axel
/// @date      2026-09-04
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
///
/// @details
///
/// 本测试用解析可解的指数衰减 ODE（dy/dt = -y，解 y(t) = e^-t）验证：
/// - 事件 y = 0.5 的解析时刻 t* = ln(2) ≈ 0.693147；
/// - 固定步长下事件停止时刻应精确等于 t*。

#include "ast/ODE.hpp"
#include "ast/RKF78.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include "ast/ODEEventDetector.hpp"
#include "ast/Constants.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.hpp"

AST_USING_NAMESPACE
using namespace _AST literals;

/// @brief 指数衰减 ODE：dy/dt = -y，解 y(t) = y0 * e^(-t)，便于解析校验
class DecayODE : public ODE
{
public:
    errc_t evaluate(const double* y, double* ydot, double t) override
    {
        ydot[0] = -y[0];
        return eNoError;
    }
    int getDimension() const override { return 1; }
};

/// @brief 检测状态 y[0] 等于目标值的事件检测器
class DecayEventDetector : public ODEEventDetector
{
public:
    explicit DecayEventDetector(double targetValue)
    {
        this->setDirection(eBoth);
        this->setThreshold(1e-10);
        this->setGoal(targetValue);
    }
    double getValue(const double* y, double x) const override { return y[0]; }
};

/// @brief 固定步长下事件精确根查找
/// @details
/// 用 RKF78 固定步长 0.1 s 积分 dy/dt = -y（初值 y(0)=1），检测 y = 0.5。
/// 解析事件时刻 t* = ln(2)。期望积分器在事件发生在 t* 处截断并给出精确状态。
TEST(ODEEventDetectorFixedStep, RootIsPreciseNotGridPoint)
{
    DecayODE ode;
    RKF78 integrator;

    // 切换至固定步长（RKF78 默认为变步长）
    integrator.setUseFixedStep(true);
    integrator.setStepSize(0.1_s);

    double y[1] = {1.0};   // y(0) = 1
    double t = 0.0;        // 从 0 积分到 tf
    double tf = 1.0;

    DecayEventDetector* detector = new DecayEventDetector(0.5);
    integrator.addEventDetector(detector);

    errc_t result = integrator.integrate(ode, y, t, tf);
    EXPECT_EQ(result, eNoError);
    printf("fixed-step event time = %.9f s (analytic ln2 = %.9f)\n", t, log(2.0));

    // 事件停止时刻应精确等于解析值 ln(2)，而非 0.6 s / 0.7 s 这类网格点
    EXPECT_NEAR(t, log(2.0), 1e-6);
    // 事件时刻状态 e^(-t*) = 0.5
    EXPECT_NEAR(y[0], 0.5, 1e-6);

    integrator.removeEventDetector(detector);
}

/// @brief 步长无关性
/// @details
/// 缩小固定步长（0.02 s）后事件时刻仍应精确等于 ln(2)。
TEST(ODEEventDetectorFixedStep, RootIndependentOfStepSize)
{
    DecayODE ode;
    RKF78 integrator;

    integrator.setUseFixedStep(true);
    integrator.setStepSize(0.02_s);

    double y[1] = {1.0};
    double t = 0.0;
    double tf = 1.0;

    DecayEventDetector* detector = new DecayEventDetector(0.5);
    integrator.addEventDetector(detector);

    errc_t result = integrator.integrate(ode, y, t, tf);
    EXPECT_EQ(result, eNoError);

    EXPECT_NEAR(t, log(2.0), 1e-6);
    EXPECT_NEAR(y[0], 0.5, 1e-6);

    integrator.removeEventDetector(detector);
}

/// @brief 多个事件检测器：取最早事件，且均应精确
/// @details
/// 同时挂两个事件检测器（y = 0.49 与 y = 0.48）。解析上先到 y=0.49，
/// 事件时刻 t* = ln(1/0.49)。验证固定步长下也能精确取到最早事件。
TEST(ODEEventDetectorFixedStep, EarliestOfMultipleDetectors)
{
    DecayODE ode;
    RKF78 integrator;

    integrator.setUseFixedStep(true);
    integrator.setStepSize(0.1_s);

    double y[1] = {1.0};
    double t = 0.0;
    double tf = 1.0;

    DecayEventDetector* detector49 = new DecayEventDetector(0.49);
    DecayEventDetector* detector48 = new DecayEventDetector(0.48);
    integrator.addEventDetector(detector49);
    integrator.addEventDetector(detector48);

    errc_t result = integrator.integrate(ode, y, t, tf);
    EXPECT_EQ(result, eNoError);

    // 最早事件为 y=0.49，t* = ln(1/0.49)
    EXPECT_NEAR(t, log(1.0 / 0.49), 1e-6);
    EXPECT_NEAR(y[0], 0.49, 1e-6);

    integrator.removeEventDetector(detector49);
    integrator.removeEventDetector(detector48);
}

GTEST_MAIN()
