///
/// @file      testVisibility.cpp
/// @brief     可见性分析基础组件测试
/// @details   测试 FOV angularMargin、aLineOfSightClearance、AccessConstraint
/// @author    axel
/// @date      2026-07-21
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/FOVSimpleCone.hpp"
#include "ast/FOVConical.hpp"
#include "ast/FOVHalfPower.hpp"
#include "ast/FOVRectangular.hpp"
#include "ast/FOVSAR.hpp"
#include "ast/FOVCustom.hpp"
#include "ast/BodyObstruction.hpp"
#include "ast/AccessConstraint.hpp"
#include "ast/BodyObstructionConstraint.hpp"
#include "ast/FieldOfViewConstraint.hpp"
#include "ast/AccessStepper.hpp"
#include "ast/FixedStepStepper.hpp"
#include "ast/AccessEvaluator.hpp"
#include "ast/SphereShape.hpp"
#include "ast/SpheroidShape.hpp"
#include "ast/Constants.h"
#include "ast/Test.hpp"
#include "ast/Literals.hpp"
#include <cmath>

AST_USING_NAMESPACE

// ==================== FOV angularMargin 测试 ====================

// 简单圆锥视场：视轴方向、边界方向、超出方向
TEST(FOVSimpleConeTest, CenterAndEdge)
{
    FOVSimpleCone fov;
    fov.setConeAngle(30_deg);

    // 视轴方向，余量等于锥角
    EXPECT_NEAR(fov.angularMargin({0.0, 0.0, 1.0}), 30_deg, 1e-9);

    // 偏离视轴 30°，恰在边界
    double s30 = std::sin(30.0 * kPI / 180.0);
    double c30 = std::cos(30.0 * kPI / 180.0);
    EXPECT_NEAR(fov.angularMargin({0.0, s30, c30}), 0.0, 1e-6);

    // 偏离视轴 45°，在视场外
    double s45 = std::sin(45.0 * kPI / 180.0);
    double c45 = std::cos(45.0 * kPI / 180.0);
    EXPECT_LT(fov.angularMargin({0.0, s45, c45}), 0.0);

    // 便捷方法
    EXPECT_TRUE(fov.containsDirection({0.0, 0.0, 1.0}));
    EXPECT_FALSE(fov.containsDirection({0.0, s45, c45}));
}

// 半功率视场：等同于简单圆锥
TEST(FOVHalfPowerTest, CenterDirection)
{
    FOVHalfPower fov;
    fov.setHalfAngle(20_deg);
    EXPECT_NEAR(fov.angularMargin({0.0, 0.0, 1.0}), 20_deg, 1e-9);
}

// 矩形视场：水平/垂直半角边界
TEST(FOVRectangularTest, HorizontalAndVerticalEdges)
{
    FOVRectangular fov;
    fov.setHorizontalHalfAngle(10_deg);
    fov.setVerticalHalfAngle(5_deg);

    EXPECT_GT(fov.angularMargin({0.0, 0.0, 1.0}), 0.0);

    // 水平 10° = 边界
    double s10 = std::sin(10.0 * kPI / 180.0);
    double c10 = std::cos(10.0 * kPI / 180.0);
    EXPECT_NEAR(fov.angularMargin({s10, 0.0, c10}), 0.0, 1e-6);

    // 水平 15° = 超出
    double s15 = std::sin(15.0 * kPI / 180.0);
    double c15 = std::cos(15.0 * kPI / 180.0);
    EXPECT_LT(fov.angularMargin({s15, 0.0, c15}), 0.0);
}

// 环形圆锥视场：内外锥 + 时钟角
TEST(FOVConicalTest, RingRegion)
{
    FOVConical fov;
    fov.setInnerConeAngle(10_deg);
    fov.setOuterConeAngle(40_deg);
    fov.setMinClockAngle(0_deg);
    fov.setMaxClockAngle(180_deg);

    // 25° 在 [10°, 40°] 内，Y+ 方向（时钟角 ~90°）
    double s25 = std::sin(25.0 * kPI / 180.0);
    double c25 = std::cos(25.0 * kPI / 180.0);
    EXPECT_GT(fov.angularMargin({0.0, s25, c25}), 0.0);

    // 50° 超出外锥
    double s50 = std::sin(50.0 * kPI / 180.0);
    double c50 = std::cos(50.0 * kPI / 180.0);
    EXPECT_LT(fov.angularMargin({0.0, s50, c50}), 0.0);

    // 5° 小于内锥
    double s5 = std::sin(5.0 * kPI / 180.0);
    double c5 = std::cos(5.0 * kPI / 180.0);
    EXPECT_LT(fov.angularMargin({0.0, s5, c5}), 0.0);
}

// SAR 视场：仰角范围 + 前后排除区
TEST(FOVSARTest, SideLooking)
{
    FOVSAR fov;
    fov.setMinElevAngle(10_deg);
    fov.setMaxElevAngle(50_deg);
    fov.setForwardExcludeAngle(5_deg);
    fov.setBackwardExcludeAngle(5_deg);

    // 仰角 30°[10,50]内、方位 30° 避开 ±5° 排除区
    double s30 = std::sin(30.0 * kPI / 180.0);
    double c30 = std::cos(30.0 * kPI / 180.0);
    double x = s30 * c30;
    double y = s30;
    double z = c30 * c30;
    EXPECT_GT(fov.angularMargin({x, y, z}), 0.0);
}

// 自定义视场：默认始终返回负值
TEST(FOVCustomTest, DefaultNegative)
{
    FOVCustom fov;
    EXPECT_LT(fov.angularMargin({0.0, 0.0, 1.0}), 0.0);
}


// ==================== 中心天体遮挡测试 ====================

// 两点在太空同一侧，视线不穿入天体
TEST(CentralBodyObstructionTest, NoObstruction)
{
    SharedPtr<SphereShape> earth = new SphereShape(6371_km);

    Vector3d obs{6371_km + 400_km, 0.0, 100_km};
    Vector3d tgt{6371_km + 400_km, 0.0, 200_km};

    EXPECT_GT(aLineOfSightClearance(obs, tgt, earth.get()), 0.0);
    EXPECT_FALSE(aLineOfSightObstructed(obs, tgt, earth.get()));
}

// 两点在地球两侧，直线穿过球心
TEST(CentralBodyObstructionTest, ThroughCenter)
{
    SharedPtr<SphereShape> earth = new SphereShape(6371_km);

    Vector3d obs{6371_km + 400_km, 0.0, 0.0};
    Vector3d tgt{-(6371_km + 400_km), 0.0, 0.0};

    EXPECT_LT(aLineOfSightClearance(obs, tgt, earth.get()), 0.0);
    EXPECT_TRUE(aLineOfSightObstructed(obs, tgt, earth.get()));
}

// 视线恰好沿水平切线方向
TEST(CentralBodyObstructionTest, TangentToHorizon)
{
    SharedPtr<SphereShape> earth = new SphereShape(6371_km);

    Vector3d obs{6371_km, 0.0, 0.0};
    Vector3d tgt{6371_km, 0.0, 100_km};

    EXPECT_NEAR(aLineOfSightClearance(obs, tgt, earth.get()), 0.0, 1e-3);
}

// 空天体指针：永无遮挡
TEST(CentralBodyObstructionTest, NullBody)
{
    EXPECT_GT(aLineOfSightClearance({0, 0, 0}, {1, 0, 0}, nullptr), 1e11);
}


// ==================== AccessConstraint 基类测试 ====================

namespace
{

// 测试用：始终满足的约束
class ConstraintAlwaysSatisfied : public AccessConstraint
{
public:
    double evaluate(const TimePoint&) const override { return 1.0; }
};

// 测试用：始终违反的约束
class ConstraintAlwaysViolated : public AccessConstraint
{
public:
    double evaluate(const TimePoint&) const override { return -1.0; }
};

// 测试用：固定余量的约束
class ConstraintFixedMargin : public AccessConstraint
{
public:
    ConstraintFixedMargin(double m) : margin_(m) {}
    double evaluate(const TimePoint&) const override { return margin_; }
private:
    double margin_;
};

} // anonymous namespace

// 正、负、零边界的 isSatisfied 判断（>=0 满足，<0 违反）
TEST(AccessConstraintTest, SignedBoundaryCheck)
{
    ConstraintAlwaysSatisfied ok;
    ConstraintAlwaysViolated bad;
    ConstraintFixedMargin zero(0.0);
    ConstraintFixedMargin pos(1e-6);
    ConstraintFixedMargin neg(-1e-6);

    auto t0 = TimePoint::Default();
    EXPECT_TRUE(ok.isSatisfied(t0));
    EXPECT_FALSE(bad.isSatisfied(t0));
    EXPECT_TRUE(zero.isSatisfied(t0));  // >=0 边界值视为满足
    EXPECT_TRUE(pos.isSatisfied(t0));
    EXPECT_FALSE(neg.isSatisfied(t0));
}

// ==================== BodyObstructionConstraint 测试 ====================

TEST(BodyObstructionConstraintTest, NoCentralBody)
{
    BodyObstructionConstraint c;
    // 未设置对象，应返回负值
    auto t0 = TimePoint::Default();
    EXPECT_LT(c.evaluate(t0), 0.0);
    EXPECT_FALSE(c.isSatisfied(t0));
}

TEST(BodyObstructionConstraintTest, SetMembers)
{
    BodyObstructionConstraint c;
    c.setFromObject(nullptr);
    c.setToObject(nullptr);
    c.setCentralBody(nullptr);
    EXPECT_EQ(c.fromObject(), nullptr);
    EXPECT_EQ(c.toObject(), nullptr);
    EXPECT_EQ(c.centralBody(), nullptr);
}


// ==================== FieldOfViewConstraint 测试 ====================

TEST(FieldOfViewConstraintTest, NoFOV)
{
    FieldOfViewConstraint c;
    // 未设置对象，应返回负值
    auto t0 = TimePoint::Default();
    EXPECT_LT(c.evaluate(t0), 0.0);
    EXPECT_FALSE(c.isSatisfied(t0));
}

TEST(FieldOfViewConstraintTest, SetMembers)
{
    FOVSimpleCone fov;
    fov.setConeAngle(30_deg);
    FieldOfViewConstraint c(nullptr, nullptr, &fov);
    EXPECT_EQ(c.fieldOfView(), &fov);
    EXPECT_EQ(c.fromObject(), nullptr);
    EXPECT_EQ(c.toObject(), nullptr);
}

// ==================== AccessEvaluator 测试 ====================

namespace
{

// 模拟约束：evaluate 返回固定值，方便测试正负
class MockValueConstraint : public AccessConstraint
{
public:
    explicit MockValueConstraint(double val) : val_(val) {}
    void setValue(double val) { val_ = val; }
    double evaluate(const TimePoint&) const override { return val_; }
private:
    double val_{};
};

} // anonymous namespace

// 全部采样点为正：整个区间都应满足
TEST(AccessEvaluatorTest, AllPositive)
{
    MockValueConstraint constraint(1.0);
    FixedStepStepper stepper(10.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    evaluator.setStepper(&stepper);

    auto t0 = TimePoint::Default();
    TimeIntervalList intervals;
    evaluator.evaluate({t0, t0 + 100.0}, intervals);
    EXPECT_EQ(intervals.size(), 1u);
}

// 全部采样点为负：无满足区间
TEST(AccessEvaluatorTest, AllNegative)
{
    MockValueConstraint constraint(-1.0);
    FixedStepStepper stepper(10.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    evaluator.setStepper(&stepper);

    auto t0 = TimePoint::Default();
    TimeIntervalList intervals;
    evaluator.evaluate({t0, t0 + 100.0}, intervals);
    EXPECT_TRUE(intervals.empty());
}

// 无约束时返回错误
TEST(AccessEvaluatorTest, NoConstraint)
{
    FixedStepStepper stepper(30.0);
    AccessEvaluator evaluator;
    evaluator.setStepper(&stepper);

    auto t0 = TimePoint::Default();
    TimeIntervalList intervals;
    errc_t rc = evaluator.evaluate({t0, t0 + 100.0}, intervals);
    EXPECT_NE(rc, eNoError);
}

// check 单点检查
TEST(AccessEvaluatorTest, HasAccess)
{
    MockValueConstraint constraint(1.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);
    auto t0 = TimePoint::Default();
    EXPECT_TRUE(evaluator.check(t0));

    constraint.setValue(-1.0);
    EXPECT_FALSE(evaluator.check(t0));
}

// 无步进器时返回空
TEST(AccessEvaluatorTest, NoStepper)
{
    MockValueConstraint constraint(1.0);
    AccessEvaluator evaluator;
    evaluator.setConstraint(&constraint);

    auto t0 = TimePoint::Default();
    TimeIntervalList intervals;
    errc_t rc = evaluator.evaluate({t0, t0 + 100.0}, intervals);
    EXPECT_NE(rc, eNoError);
    EXPECT_TRUE(intervals.empty());
}

GTEST_MAIN()
