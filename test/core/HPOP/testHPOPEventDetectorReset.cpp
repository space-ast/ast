///
/// @file      testHPOPEventDetectorReset.cpp
/// @brief     HPOP 多次重复预报内部状态重置测试
/// @details   验证事件检测器内部状态（ODEEventObserver 的 repeatCount/lastTime 等）
///            在每次预报开始时经由 ODEIntegrator::initialize -> eventDetectorList_.reset()
///            得到正确重置，从而保证"同一 HPOP 实例对同一初始状态做多次预报，结果完全一致"。
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
/// 背景：
/// - HPOP 会缓存 equation_，配置不变时重复预报不会重建方程。
///   但 ODE 积分器（ODEIntegrator，如 RKF78）每次进行数值积分都会复用同一实例。
/// - ODEIntegrator 内部持有一个有状态的 ODEEventDetectorList（其中的 ODEEventObserver
///   会记录 repeatCount_、lastTime_、lastDifference_、eventTime_）。
/// - ODEIntegrator 在每次 integrate() 开始时调用 initialize(ode)，后者调用 eventDetectorList_.reset()
///   （见 src/AstMath/ODE/ODEIntegrator.cpp），把每个事件观察器恢复为初始状态。
///
/// 本测试用同一个 HPOP 实例 + 同一事件检测器，分两次独立预报完全相同的初始状态：

#include "ast/HPOP.hpp"
#include "ast/HPOPEquation.hpp"
#include "ast/EventDetector.hpp"
#include "ast/EOP.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.hpp"
#include "ast/Vector.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/SpacecraftState.hpp"
#include "ast/ODEVarStepIntegrator.hpp"
#include "ast/Literals.hpp"

AST_USING_NAMESPACE

using namespace _AST literals;

class HPOPEventResetTest : public ::testing::Test
{
    void SetUp() override
    {
        aInitialize();
        aDataContext_GetEOP()->unload();  // 卸载EOP数据，确保测试不受EOP修正数据的影响
    }

    void TearDown() override
    {
        aUninitialize();
    }
};

/// @brief 验证 HPOP 多次重复预报时事件检测器内部状态被重置
/// @details
/// 力模型：纯二体（JGM3 degree=0/order=0），轨道路径确定。
/// 事件检测：位置 x 分量穿越 2000 km（初始 x≈6678 km，近圆 LEO，x 分量随轨道在约
///           ±6678 km 间摆动，事件会在约 1/4 周期处触发）。
///
/// 两种情形：
///   1. 只调用一次预报 —— 验证事件确实触发（endTime 被提前截断，而非跑满 24 h）。
///   2. 复位初始状态后再调用第二次预报 —— 验证与第一次结果一致。
///      若 odEEventDetectorList_.reset() 未生效，第二次预报会在第 1 步就停，停止时间
///      会与第一次相差几十到几十分钟，从而暴露问题。
TEST_F(HPOPEventResetTest, RepeatedPropagationResetsInternalState)
{
    // ── 力模型：纯二体（点质量） ──
    HPOPForceModel forcemodel;
    forcemodel.gravity().model_ = "JGM3";
    forcemodel.gravity().maxDegree_ = 0;
    forcemodel.gravity().maxOrder_ = 0;
    forcemodel.useMoonGravity(false);

    HPOP propagator;
    errc_t err = propagator.setForceModel(forcemodel);
    ASSERT_EQ(err, eNoError);

    // 固定步长 60 s：两次预报的积分轨迹完全确定；同时也覆盖了
    // ODEIntegrator::integrateFixedStep 内对 stateAtStepStart_/timeAtStepStart_ 的维护，
    // 使事件检测器在固定步长下也能精确求根（此前会退化为停在步进网格点并打印告警）。
    auto integrator = propagator.getIntegrator();
    auto varstep = dynamic_cast<ODEVarStepIntegrator*>(integrator);
    ASSERT_NE(varstep, nullptr);
    varstep->setUseFixedStep(true);
    varstep->setStepSize(60_s);

    // ── 事件检测器：位置 x 分量穿越 2000 km ──
    // addEventDetector 模板重载直接包装 lambda 并返回 EventDetector*；
    // 所有权随 addEventDetector 交给内部 ODEEventDetectorWrap（SharedPtr 管理），勿手动 delete。
    auto detector = propagator.addEventDetector(
        [](const SpacecraftState& s, double /*t*/) {
            CartState c;
            s.getState(c);
            return c.x();
        });
    detector->setGoal(2000_km);

    // ── 初始状态（与其它 HPOP 测试一致的近圆 LEO） ──
    auto start  = TimePoint::FromUTC(2026, 1, 20, 0, 0, 0);
    auto target = TimePoint::FromUTC(2026, 1, 21, 0, 0, 0);  // 24 h 预报
    Vector3d pos{6678137, 0, 0};
    Vector3d vel{0, 6789.53029, 3686.414173};

    // ── 第 1 次预报 ──
    TimePoint end1 = target;
    err = propagator.propagate(start, end1, pos, vel);
    EXPECT_EQ(err, eNoError);
    double dur1 = end1 - start;
    printf("1st propagate: event time = %.6f s\n", dur1);

    // 事件检测器应该触发，使预报提前停止（dur1 应显著小于 24 h 且明显大于步长 60 s）
    EXPECT_GT(dur1, 1e-6);
    EXPECT_LT(dur1, 24_hour);

    // ── 复位初始状态，第 2 次预报 ──
    Vector3d pos2{6678137, 0, 0};
    Vector3d vel2{0, 6789.53029, 3686.414173};
    TimePoint end2 = target;
    err = propagator.propagate(start, end2, pos2, vel2);
    EXPECT_EQ(err, eNoError);
    double dur2 = end2 - start;
    printf("2nd propagate: event time = %.6f s\n", dur2);

    // 两次预报的事件触发时刻必须一致 —— 这就是"内部状态被重置"的直接证据。
    // 若 eventDetectorList_.reset() 失效，第二次预报将由残留的 repeatCount_ 在第 1 步就触发，
    // dur2 将明显小于 dur1，下一条断言失败。
    EXPECT_NEAR(dur2, dur1, 1e-6);

    // 最终状态也应一致
    EXPECT_NEAR(pos2[0], pos[0], 1e-3);
    EXPECT_NEAR(pos2[1], pos[1], 1e-3);
    EXPECT_NEAR(pos2[2], pos[2], 1e-3);
    EXPECT_NEAR(vel2[0], vel[0], 1e-6);
    EXPECT_NEAR(vel2[1], vel[1], 1e-6);
    EXPECT_NEAR(vel2[2], vel[2], 1e-6);
}

GTEST_MAIN()
