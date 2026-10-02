///
/// @file      testEuclid.cpp
/// @brief     Euclid模块单元测试
/// @details   测试Axes、Frame、Point等几何基类的功能
/// @author    coze
/// @date      2026-04-12
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/Axes.hpp"
#include "ast/Frame.hpp"
#include "ast/Point.hpp"
#include "ast/AxesRoot.hpp"
#include "ast/AxesICRF.hpp"
#include "ast/FrameTransform.hpp"
#include "ast/RunTime.hpp"
#include "ast/EarthFrame.hpp"
#include "ast/CelestialBody.hpp"
#include "ast/Transform.hpp"
#include "ast/KinematicTransform.hpp"
#include "ast/Rotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Matrix.hpp"
#include "ast/Vector.hpp"
#include "ast/AstTestMacro.h"
#include <cmath>

AST_USING_NAMESPACE

// ============================================
// Test Fixture - 统一管理初始化
// ============================================
class EuclidTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        // 在所有测试开始前初始化运行时环境
        aInitialize();
    }
};

// ============================================
// Axes类测试
// ============================================

TEST_F(EuclidTest, AxesGetDepth)
{
    // 测试根轴系深度为1
    Axes* root = aAxesRoot();
    ASSERT_NE(root, nullptr);
    EXPECT_EQ(root->getDepth(), 1);
}

TEST_F(EuclidTest, AxesGetAncestor)
{
    Axes* root = aAxesRoot();
    ASSERT_NE(root, nullptr);
    
    // 根轴系的祖先应该是自己
    Axes* ancestor = root->getAncestor(0);
    EXPECT_EQ(ancestor, root);
}

TEST_F(EuclidTest, AxesSelfTransform)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    // 测试ICRF轴系的自变换（应该是单位旋转）
    Axes* icrf = aAxesICRF();
    ASSERT_NE(icrf, nullptr);
    
    Rotation rotation;
    errc_t rc = aAxesTransform(icrf, icrf, tp, rotation);
    EXPECT_EQ(rc, eNoError);
    
    // 自变换应该是单位旋转矩阵
    Matrix3d mat = rotation.getMatrix();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
            {
                EXPECT_NEAR(mat(i, j), 1.0, 1e-14);
            }
            else
            {
                EXPECT_NEAR(mat(i, j), 0.0, 1e-14);
            }
        }
    }
}

TEST_F(EuclidTest, AxesTransformKinematicRotation)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    Axes* icrf = aAxesICRF();
    ASSERT_NE(icrf, nullptr);
    
    // 测试动力学旋转版本
    KinematicRotation kr;
    errc_t rc = aAxesTransform(icrf, icrf, tp, kr);
    EXPECT_EQ(rc, eNoError);
}

TEST_F(EuclidTest, AxesTransformNullInput)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    Rotation rotation;
    
    // 空指针输入应该返回错误码
    errc_t rc1 = aAxesTransform(nullptr, nullptr, tp, rotation);
    EXPECT_NE(rc1, eNoError);
    
    Axes* icrf = aAxesICRF();
    errc_t rc2 = aAxesTransform(nullptr, icrf, tp, rotation);
    EXPECT_NE(rc2, eNoError);
    
    errc_t rc3 = aAxesTransform(icrf, nullptr, tp, rotation);
    EXPECT_NE(rc3, eNoError);
}

// ============================================
// Frame类测试
// ============================================

TEST_F(EuclidTest, FrameGetParent)
{
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    // 测试获取父坐标系
    Frame* parent = ecf->getParent();
    // ECF的父坐标系应该存在
    EXPECT_NE(parent, nullptr);
}

TEST_F(EuclidTest, FrameGetBody)
{
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    // 测试获取绑定的天体
    CelestialBody* body = ecf->getBody();
    EXPECT_EQ(body, earth);
}

TEST_F(EuclidTest, FrameGetGM)
{
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    // 测试获取GM
    double gm = ecf->getGM();
    EXPECT_GT(gm, 0.0);
}

TEST_F(EuclidTest, FrameSelfTransform)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    // 测试自变换（应该是单位变换）
    Transform transform;
    errc_t rc = aFrameTransform(ecf.get(), ecf.get(), tp, transform);
    EXPECT_EQ(rc, eNoError);
    
    // 自变换的位置偏移应该是零
    Vector3d translation = transform.getTranslation();
    EXPECT_NEAR(translation.norm(), 0.0, 1e-14);
}

TEST_F(EuclidTest, FrameTransformKinematic)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    // 测试动力学变换版本
    KinematicTransform kt;
    errc_t rc = aFrameTransform(ecf.get(), ecf.get(), tp, kt);
    EXPECT_EQ(rc, eNoError);
}

TEST_F(EuclidTest, FrameTransformNullInput)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    Transform transform;
    
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    auto ecf = earth->makeFrameFixed();
    
    // 空指针输入
    errc_t rc1 = aFrameTransform(nullptr, nullptr, tp, transform);
    EXPECT_NE(rc1, eNoError);
    
    errc_t rc2 = aFrameTransform(nullptr, ecf.get(), tp, transform);
    EXPECT_NE(rc2, eNoError);
    
    errc_t rc3 = aFrameTransform(ecf.get(), nullptr, tp, transform);
    EXPECT_NE(rc3, eNoError);
}

// ============================================
// Point类测试
// ============================================

TEST_F(EuclidTest, PointToBody)
{
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 测试获取绑定的天体
    CelestialBody* body = origin->toBody();
    EXPECT_EQ(body, earth);
}

TEST_F(EuclidTest, PointGetFrame)
{
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 测试获取所属坐标系
    Frame* frame = origin->getFrame();
    EXPECT_NE(frame, nullptr);
}

TEST_F(EuclidTest, PointGetPosVelIn)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 测试获取位置和速度
    Vector3d pos, vel;
    errc_t rc = origin->getPosVelIn(ecf.get(), tp, pos, vel);
    EXPECT_EQ(rc, eNoError);
    
    // 原点在ECF坐标系中的位置应为零向量
    EXPECT_NEAR(pos.norm(), 0.0, 1e-14);
    
    // 原点在ECF坐标系中的速度也应为零向量
    EXPECT_NEAR(vel.norm(), 0.0, 1e-14);
}

TEST_F(EuclidTest, PointGetPosInSameFrame)
{
    auto tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    
    auto earth = aGetEarth();
    ASSERT_NE(earth, nullptr);
    
    auto ecf = earth->makeFrameFixed();
    ASSERT_NE(ecf, nullptr);
    
    Point* origin = ecf->getOrigin();
    ASSERT_NE(origin, nullptr);
    
    // 在ECF中，原点位置应为零
    Vector3d posECF;
    errc_t rc = origin->getPosIn(ecf.get(), tp, posECF);
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(posECF.norm(), 0.0, 1e-14);
}

// ============================================
// Point::getPosVelAcc 默认实现测试
// ============================================

/// @brief 测试用点：在 xy 平面内做匀速圆周运动，位置/速度有解析解，
///        加速度为 -ω²·pos，用于校验默认实现的中心差分
class TestCircularPoint : public Point
{
public:
    Frame* getFrame() const override { return nullptr; }

    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override
    {
        Vector3d vel;
        return getPosVel(tp, pos, vel);
    }

    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override
    {
        const double t = tp - epoch_;
        const double c = std::cos(rate_ * t);
        const double s = std::sin(rate_ * t);
        pos = Vector3d{radius_ * c, radius_ * s, 0.0};
        vel = Vector3d{-radius_ * rate_ * s, radius_ * rate_ * c, 0.0};
        return eNoError;
    }

    TimePoint epoch_{};
    double radius_{7000e3};   ///< 半径 [m]
    double rate_{1.0e-3};     ///< 角速度 [rad/s]
};

/// @brief 测试用点：任何查询都失败，用于校验默认实现的错误传递
class TestFailingPoint : public Point
{
public:
    Frame* getFrame() const override { return nullptr; }
    errc_t getPos(const TimePoint&, Vector3d&) const override { return eErrorNotFound; }
    errc_t getPosVel(const TimePoint&, Vector3d&, Vector3d&) const override { return eErrorNotFound; }
};

TEST_F(EuclidTest, PointGetPosVelAccDefault)
{
    TestCircularPoint point;
    point.epoch_ = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);
    const TimePoint tp = point.epoch_ + 3600.0;

    Vector3d pos, vel, acc;
    errc_t rc = point.getPosVelAcc(tp, pos, vel, acc);
    EXPECT_EQ(rc, eNoError);

    // 位置、速度直接来自 getPosVel，应与其解析解一致
    const double t = tp - point.epoch_;
    const double c = std::cos(point.rate_ * t);
    const double s = std::sin(point.rate_ * t);
    EXPECT_NEAR(pos.x(), point.radius_ * c, 1e-6);
    EXPECT_NEAR(pos.y(), point.radius_ * s, 1e-6);
    EXPECT_NEAR(vel.x(), -point.radius_ * point.rate_ * s, 1e-9);
    EXPECT_NEAR(vel.y(), point.radius_ * point.rate_ * c, 1e-9);

    // 加速度默认为速度的中心差分，应与解析值 -ω²·pos 一致
    const Vector3d accExpected = pos * (-point.rate_ * point.rate_);
    EXPECT_NEAR((acc - accExpected).norm(), 0.0, 1e-9 * accExpected.norm());
}

TEST_F(EuclidTest, PointGetPosVelAccErrorPropagation)
{
    TestFailingPoint point;
    const TimePoint tp = TimePoint::FromUTC(2026, 3, 4, 0, 0, 0);

    Vector3d pos, vel, acc;
    errc_t rc = point.getPosVelAcc(tp, pos, vel, acc);
    EXPECT_EQ(rc, eErrorNotFound);
}

GTEST_MAIN()
