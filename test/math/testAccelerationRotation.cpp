///
/// @file      testAccelerationRotation.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-01
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

#include "ast/AccelerationRotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Rotation.hpp"
#include "ast/AttitudeConvert.hpp"
#include "ast/Matrix.hpp"
#include "ast/Vector.hpp"
#include "ast/Literals.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

/// @brief 按相对容差比较两个向量
static void expectVectorNear(const Vector3d& actual, const Vector3d& expect, double relTol, double absTol)
{
    const double tol = absTol + relTol * expect.norm();
    for (int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(actual[i], expect[i], tol);
    }
}

TEST(AccelerationRotationTest, Identity)
{
    AccelerationRotation rotation = AccelerationRotation::Identity();
    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d posOut{}, velOut{}, accOut{};
    rotation.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posOut[i], pos[i]);
        EXPECT_DOUBLE_EQ(velOut[i], vel[i]);
        EXPECT_DOUBLE_EQ(accOut[i], acc[i]);
    }

    Vector3d posInv{}, velInv{}, accInv{};
    rotation.transformPosVelAccInv(pos, vel, acc, posInv, velInv, accInv);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posInv[i], pos[i]);
        EXPECT_DOUBLE_EQ(velInv[i], vel[i]);
        EXPECT_DOUBLE_EQ(accInv[i], acc[i]);
    }
}

/*!
    与差分对照。
    取解析的旋转 M(t) = aRotationZMatrix(θ(t))，θ(t) = θ0 + a·t + b·t²/2，
    则按本库的旋转矩阵约定有 ω = a·ẑ、ω̇ = b·ẑ（aRotationZMatrix 给的是教科书 Rz 的转置，
    恰好满足 Ṁ = −M[ω×]）。
    源系下的轨迹取等加速度运动 r(t) = r0 + v0·t + a0·t²/2，其目标系下的轨迹为 M(t)·r(t)，
    对该轨迹做中心差分求一、二阶导数，应与 transformPosVelAcc 的输出一致。
    这样是拿"导数的定义"去校验变换公式，不会有循环论证。
*/
TEST(AccelerationRotationTest, TransformAgainstFiniteDifference)
{
    const double theta0 = 0.7;      // 初始转角 [rad]，取非零值使 M(0) 不是单位阵
    const double omega0 = 0.3;      // 角速度 [rad/s]
    const double alpha0 = -0.07;    // 角加速度 [rad/s^2]

    auto theta = [=](double t) { return theta0 + omega0 * t + 0.5 * alpha0 * t * t; };
    auto mat = [&](double t) { Matrix3d m; aRotationZMatrix(theta(t), m); return m; };

    // 位置、速度、加速度的量级取得使 ω×ω×ρ、ω̇×ρ、2ω×v 三项相当，交叉项都能被检验到
    Vector3d pos{1.0e4, -2.0e4, 3.0e3};
    Vector3d vel{1.0e3, 2.0e3, -3.0e3};
    Vector3d acc{-600.0, 500.0, -400.0};

    auto posInTarget = [&](double t) {
        Vector3d posInSource = pos + vel * t + acc * (0.5 * t * t);
        return mat(t) * posInSource;
    };

    AccelerationRotation rotation(mat(0.0), Vector3d{0, 0, omega0}, Vector3d{0, 0, alpha0});
    Vector3d posOut{}, velOut{}, accOut{};
    rotation.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);

    const double h = 1e-4;          // 中心差分的步长，兼顾截断误差与舍入误差
    Vector3d posFd = posInTarget(0.0);
    Vector3d velFd = (posInTarget(h) - posInTarget(-h)) * (1.0 / (2 * h));
    Vector3d accFd = (posInTarget(h) - posInTarget(0.0) * 2.0 + posInTarget(-h)) * (1.0 / (h * h));

    expectVectorNear(posOut, posFd, 1e-9, 1e-9);
    expectVectorNear(velOut, velFd, 1e-5, 1e-6);
    expectVectorNear(accOut, accFd, 1e-5, 1e-4);

    // 逆变换应与正向变换互为逆运算
    Vector3d posBack{}, velBack{}, accBack{};
    rotation.transformPosVelAccInv(posOut, velOut, accOut, posBack, velBack, accBack);
    expectVectorNear(posBack, pos, 1e-13, 1e-13);
    expectVectorNear(velBack, vel, 1e-13, 1e-13);
    expectVectorNear(accBack, acc, 1e-13, 1e-13);
}

TEST(AccelerationRotationTest, Composed)
{
    AccelerationRotation rotation1(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d{3, 5, -2});
    AccelerationRotation rotation2(Rotation(50_deg, Vector3d{3, 2, -5}), Vector3d{11, 3, -4}, Vector3d{-6, 8, 1});

    AccelerationRotation composed1 = rotation1.composed(rotation2);
    AccelerationRotation composed2(rotation1);
    composed2.compose(rotation2);

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d pos1{}, vel1{}, acc1{};
    Vector3d pos2{}, vel2{}, acc2{};
    Vector3d pos3{}, vel3{}, acc3{};
    Vector3d posTemp{}, velTemp{}, accTemp{};

    composed1.transformPosVelAcc(pos, vel, acc, pos1, vel1, acc1);
    composed2.transformPosVelAcc(pos, vel, acc, pos2, vel2, acc2);
    // 组合变换应等价于依次应用两个变换（先rotation1，后rotation2）
    rotation1.transformPosVelAcc(pos, vel, acc, posTemp, velTemp, accTemp);
    rotation2.transformPosVelAcc(posTemp, velTemp, accTemp, pos3, vel3, acc3);

    expectVectorNear(pos1, pos2, 1e-15, 1e-15);
    expectVectorNear(vel1, vel2, 1e-15, 1e-15);
    expectVectorNear(acc1, acc2, 1e-15, 1e-15);

    expectVectorNear(pos1, pos3, 1e-12, 1e-12);
    expectVectorNear(vel1, vel3, 1e-12, 1e-12);
    expectVectorNear(acc1, acc3, 1e-12, 1e-12);
}

TEST(AccelerationRotationTest, Inverse)
{
    AccelerationRotation rotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d{3, 5, -2});
    AccelerationRotation inversed = rotation.inverse();

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d posOut{}, velOut{}, accOut{};
    Vector3d posBack{}, velBack{}, accBack{};
    rotation.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);
    inversed.transformPosVelAcc(posOut, velOut, accOut, posBack, velBack, accBack);

    expectVectorNear(posBack, pos, 1e-13, 1e-13);
    expectVectorNear(velBack, vel, 1e-13, 1e-13);
    expectVectorNear(accBack, acc, 1e-13, 1e-13);

    // 与逆旋转组合应得到单位元
    AccelerationRotation identity = rotation.composed(inversed);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
                EXPECT_NEAR(identity.getMatrix()(i, j), 1.0, 1e-13);
            else
                EXPECT_NEAR(identity.getMatrix()(i, j), 0.0, 1e-13);
        }
        EXPECT_NEAR(identity.getRotationRate()[i], 0.0, 1e-13);
        EXPECT_NEAR(identity.getRotationRateDot()[i], 0.0, 1e-13);
    }
}

/*!
    退化一致性：角加速度为零时，结果应与 KinematicRotation 完全一致。
*/
TEST(AccelerationRotationTest, DegenerateToKinematicRotation)
{
    KinematicRotation kinematic1(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7});
    KinematicRotation kinematic2(Rotation(50_deg, Vector3d{3, 2, -5}), Vector3d{11, 3, -4});
    AccelerationRotation rotation1(kinematic1, Vector3d::Zero());
    AccelerationRotation rotation2(kinematic2, Vector3d::Zero());

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d pos1{}, vel1{}, acc1{};
    Vector3d pos2{}, vel2{};
    rotation1.transformPosVelAcc(pos, vel, acc, pos1, vel1, acc1);
    kinematic1.transformVectorVelocity(pos, vel, pos2, vel2);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_EQ(pos1[i], pos2[i]);
        EXPECT_EQ(vel1[i], vel2[i]);
    }

    Vector3d posInv1{}, velInv1{}, accInv1{};
    Vector3d posInv2{}, velInv2{};
    rotation1.transformPosVelAccInv(pos, vel, acc, posInv1, velInv1, accInv1);
    kinematic1.transformVectorVelocityInv(pos, vel, posInv2, velInv2);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_EQ(posInv1[i], posInv2[i]);
        EXPECT_EQ(velInv1[i], velInv2[i]);
    }
}

GTEST_MAIN()
