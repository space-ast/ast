///
/// @file      testAxesAccelerationRotation.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-02
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

#include "ast/Axes.hpp"
#include "ast/AccelerationRotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Rotation.hpp"
#include "ast/AttitudeConvert.hpp"
#include "ast/Matrix.hpp"
#include "ast/Vector.hpp"
#include "ast/TimePoint.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

static TimePoint testEpoch()
{
    return TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
}

/// @brief 只给出旋转变换和角速度、不给出角加速度的测试轴系
/// @details 绕 Z 轴以 θ(t) = θ0 + ω0·t + α·t²/2 转动，其角加速度为常数 α。
///          不重载 AccelerationRotation 版本，用来检验 Axes 的默认差分实现。
class TestSpinningAxes : public Axes
{
public:
    /// @param omega0 初始角速度 [rad/s]
    /// @param alpha  角加速度 [rad/s^2]
    /// @param theta0 初始转角 [rad]
    TestSpinningAxes(double omega0, double alpha, double theta0 = 0.7)
        : tp0_(testEpoch())
        , theta0_(theta0)
        , omega0_(omega0)
        , alpha_(alpha)
    {}

    /// @note 本类重载了 getTransform 的其它版本，会遮蔽基类的 AccelerationRotation 版本，
    ///       需要显式引入基类的重载集合
    using Axes::getTransform;

    Axes* getParent() const override { return nullptr; }    // 根轴系

    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override
    {
        aRotationZMatrix(angle(tp), rotation.matrix());
        return eNoError;
    }

    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override
    {
        aRotationZMatrix(angle(tp), rotation.matrix());
        rotation.setRotationRate(Vector3d{0.0, 0.0, rate(tp)});
        return eNoError;
    }

private:
    double t(const TimePoint& tp) const { return tp - tp0_; }
    double angle(const TimePoint& tp) const { double tt = t(tp); return theta0_ + omega0_ * tt + 0.5 * alpha_ * tt * tt; }
    double rate(const TimePoint& tp) const { return omega0_ + alpha_ * t(tp); }

    TimePoint tp0_;
    double theta0_;
    double omega0_;
    double alpha_;
};

class AxesAccelerationRotationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        aInitialize();
    }

    void TearDown() override
    {
        aUninitialize();
    }
};

/*!
    默认实现应给出与运动学版本一致的旋转矩阵和角速度，并用中心差分求出角加速度 α。
*/
TEST_F(AxesAccelerationRotationTest, DefaultFromDifference)
{
    TestSpinningAxes axes(0.3, -0.07);
    const TimePoint tp = testEpoch() + 100.0;   // 距历元 100s，使 θ、ω 均不为零

    AccelerationRotation accelRot;
    errc_t rc = axes.getTransform(tp, accelRot);
    EXPECT_EQ(rc, eNoError);

    KinematicRotation kinRot;
    EXPECT_EQ(axes.getTransform(tp, kinRot), eNoError);

    // 旋转矩阵、角速度应与运动学版本完全一致
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            EXPECT_NEAR(accelRot.getMatrix()(i, j), kinRot.getMatrix()(i, j), 1e-14);
        EXPECT_NEAR(accelRot.getRotationRate()[i], kinRot.getRotationRate()[i], 1e-14);
    }

    // θ(t) 是二次的，中心差分对线性角速度精确，因此角加速度应严格等于 α。
    // 容差取 1e-9:
    EXPECT_NEAR(accelRot.getRotationRateDot()[0], 0.0, 1e-10);
    EXPECT_NEAR(accelRot.getRotationRateDot()[1], 0.0, 1e-10);
    EXPECT_NEAR(accelRot.getRotationRateDot()[2], -0.07, 1e-9);
}

/*!
    绕定轴匀速转动时角加速度应为零——差分实现不应对匀速转动引入虚假的角加速度。
*/
TEST_F(AxesAccelerationRotationTest, ConstantRateHasZeroAcceleration)
{
    TestSpinningAxes axes(0.3, 0.0);
    const TimePoint tp = testEpoch() + 100.0;

    AccelerationRotation accelRot;
    EXPECT_EQ(axes.getTransform(tp, accelRot), eNoError);

    for (int i = 0; i < 3; i++)
        EXPECT_NEAR(accelRot.getRotationRateDot()[i], 0.0, 1e-12);
}

GTEST_MAIN()
