///
/// @file      testAccelerationTransform.cpp
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

#include "ast/AccelerationTransform.hpp"
#include "ast/KinematicTransform.hpp"
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

TEST(AccelerationTransformTest, Identity)
{
    AccelerationTransform transform = AccelerationTransform::Identity();
    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d posOut{}, velOut{}, accOut{};
    transform.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posOut[i], pos[i]);
        EXPECT_DOUBLE_EQ(velOut[i], vel[i]);
        EXPECT_DOUBLE_EQ(accOut[i], acc[i]);
    }

    Vector3d posInv{}, velInv{}, accInv{};
    transform.transformPosVelAccInv(pos, vel, acc, posInv, velInv, accInv);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(posInv[i], pos[i]);
        EXPECT_DOUBLE_EQ(velInv[i], vel[i]);
        EXPECT_DOUBLE_EQ(accInv[i], acc[i]);
    }
}

/*!
    与差分对照。
    取解析的目标系运动：原点 t(τ) = t0 + V0·τ + A0·τ²/2，轴系 M(τ) = aRotationZMatrix(θ(τ))，
    θ(τ) = θ0 + ω·τ + α·τ²/2，则按本库的旋转矩阵约定有 ω = ω·ẑ、α = α·ẑ
    （aRotationZMatrix 给的是教科书 Rz 的转置，恰好满足 Ṁ = −M[ω×]）。
    源系下的点轨迹取等加速运动 p(τ) = p0 + v0·τ + a0·τ²/2，
    则在目标系下看到的轨迹为 p'(τ) = M(τ)·(p(τ) − t(τ))，
    对它做中心差分求一、二阶导数，应与 transformPosVelAcc 的输出一致。
    这样是拿"导数的定义"去校验变换公式，不会有循环论证。
*/
TEST(AccelerationTransformTest, TransformAgainstFiniteDifference)
{
    const double theta0 = 0.7;      // 初始转角 [rad]，取非零值使 M(0) 不是单位阵
    const double omega0 = 0.3;      // 角速度 [rad/s]
    const double alpha0 = -0.07;    // 角加速度 [rad/s^2]

    // 目标系原点在源系下的位置、速度、加速度
    Vector3d tran0{1.0e3, -2.0e3, 5.0e2};
    Vector3d tranVel0{10.0, -20.0, 5.0};
    Vector3d tranAcc0{0.1, 0.05, -0.2};

    // 点的位置、速度、加速度。取这个量级使 ω×(ω×ρ)≈135、α×ρ≈105、2ω×ρ̇≈32 三项相当，
    // 任何一项被漏掉或写错都会被下面的比较抓住；同时量级不大，往返还原的绝对容差才好定。
    Vector3d pos{2.0e3, -3.0e3, 1.0e3};
    Vector3d vel{20.0, 30.0, -10.0};
    Vector3d acc{-0.5, 0.3, 0.7};

    auto theta = [=](double t) { return theta0 + omega0 * t + 0.5 * alpha0 * t * t; };
    auto mat = [&](double t) { Matrix3d m; aRotationZMatrix(theta(t), m); return m; };
    auto translation = [&](double t) { return tran0 + tranVel0 * t + tranAcc0 * (0.5 * t * t); };

    auto posInTarget = [&](double t) {
        Vector3d posInSource = pos + vel * t + acc * (0.5 * t * t);
        return mat(t) * (posInSource - translation(t));
    };

    AccelerationTransform transform(tran0, tranVel0, tranAcc0,
                                    AccelerationRotation(mat(0.0), Vector3d{0, 0, omega0}, Vector3d{0, 0, alpha0}));
    Vector3d posOut{}, velOut{}, accOut{};
    transform.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);

    const double h = 1e-4;          // 中心差分的步长，兼顾截断误差与舍入误差
    Vector3d posFd = posInTarget(0.0);
    Vector3d velFd = (posInTarget(h) - posInTarget(-h)) * (1.0 / (2 * h));
    Vector3d accFd = (posInTarget(h) - posInTarget(0.0) * 2.0 + posInTarget(-h)) * (1.0 / (h * h));

    expectVectorNear(posOut, posFd, 1e-9, 1e-9);
    expectVectorNear(velOut, velFd, 1e-5, 1e-6);
    expectVectorNear(accOut, accFd, 1e-5, 1e-4);

    // 逆变换应与正向变换互为逆运算
    Vector3d posBack{}, velBack{}, accBack{};
    transform.transformPosVelAccInv(posOut, velOut, accOut, posBack, velBack, accBack);
    expectVectorNear(posBack, pos, 1e-12, 1e-11);
    expectVectorNear(velBack, vel, 1e-12, 1e-11);
    expectVectorNear(accBack, acc, 1e-12, 1e-11);
}

TEST(AccelerationTransformTest, Composed)
{
    AccelerationTransform transform1(
        Vector3d{4, -1, 2}, Vector3d{-3, 5, 1}, Vector3d{0.4, -0.2, 0.7},
        AccelerationRotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d{3, 5, -2}));
    AccelerationTransform transform2(
        Vector3d{3, 2, -5}, Vector3d{7, -2, 4}, Vector3d{-0.3, 0.6, 0.1},
        AccelerationRotation(Rotation(50_deg, Vector3d{3, 2, -5}), Vector3d{11, 3, -4}, Vector3d{-6, 8, 1}));

    AccelerationTransform composed1 = transform1.composed(transform2);
    AccelerationTransform composed2(transform1);
    composed2.compose(transform2);

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d pos1{}, vel1{}, acc1{};
    Vector3d pos2{}, vel2{}, acc2{};
    Vector3d pos3{}, vel3{}, acc3{};
    Vector3d posTemp{}, velTemp{}, accTemp{};

    composed1.transformPosVelAcc(pos, vel, acc, pos1, vel1, acc1);
    composed2.transformPosVelAcc(pos, vel, acc, pos2, vel2, acc2);
    // 组合变换应等价于依次应用两个变换（先transform1，后transform2）
    transform1.transformPosVelAcc(pos, vel, acc, posTemp, velTemp, accTemp);
    transform2.transformPosVelAcc(posTemp, velTemp, accTemp, pos3, vel3, acc3);

    expectVectorNear(pos1, pos2, 1e-15, 1e-15);
    expectVectorNear(vel1, vel2, 1e-15, 1e-15);
    expectVectorNear(acc1, acc2, 1e-15, 1e-15);

    expectVectorNear(pos1, pos3, 1e-12, 1e-12);
    expectVectorNear(vel1, vel3, 1e-12, 1e-12);
    expectVectorNear(acc1, acc3, 1e-12, 1e-12);
}

// 测试结合律
TEST(AccelerationTransformTest, ComposedAssociative)
{
    AccelerationTransform transformA(
        Vector3d{4, -1, 2}, Vector3d{-3, 5, 1}, Vector3d{0.4, -0.2, 0.7},
        AccelerationRotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d{3, 5, -2}));
    AccelerationTransform transformB(
        Vector3d{3, 2, -5}, Vector3d{7, -2, 4}, Vector3d{-0.3, 0.6, 0.1},
        AccelerationRotation(Rotation(50_deg, Vector3d{3, 2, -5}), Vector3d{11, 3, -4}, Vector3d{-6, 8, 1}));
    AccelerationTransform transformC(
        Vector3d{-2, 6, 1}, Vector3d{2, 1, -6}, Vector3d{0.9, -0.5, -0.2},
        AccelerationRotation(Rotation(70_deg, Vector3d{-2, 6, 1}), Vector3d{4, -7, 2}, Vector3d{1, -3, 5}));

    AccelerationTransform left = transformA.composed(transformB).composed(transformC);
    AccelerationTransform right = transformA.composed(transformB.composed(transformC));

    for (int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(left.getTranslation()[i], right.getTranslation()[i], 1e-12);
        EXPECT_NEAR(left.getVelocity()[i], right.getVelocity()[i], 1e-12);
        EXPECT_NEAR(left.getAcceleration()[i], right.getAcceleration()[i], 1e-12);
        EXPECT_NEAR(left.getRotationRate()[i], right.getRotationRate()[i], 1e-12);
        EXPECT_NEAR(left.getRotationRateDot()[i], right.getRotationRateDot()[i], 1e-12);
        for (int j = 0; j < 3; j++)
        {
            EXPECT_NEAR(left.getRotation().getMatrix()(i, j), right.getRotation().getMatrix()(i, j), 1e-12);
        }
    }
}

TEST(AccelerationTransformTest, Inverse)
{
    AccelerationTransform transform(
        Vector3d{4, -1, 2}, Vector3d{-3, 5, 1}, Vector3d{0.4, -0.2, 0.7},
        AccelerationRotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d{3, 5, -2}));
    AccelerationTransform inversed = transform.inverse();

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d posOut{}, velOut{}, accOut{};
    Vector3d posBack{}, velBack{}, accBack{};
    transform.transformPosVelAcc(pos, vel, acc, posOut, velOut, accOut);
    inversed.transformPosVelAcc(posOut, velOut, accOut, posBack, velBack, accBack);

    expectVectorNear(posBack, pos, 1e-12, 1e-12);
    expectVectorNear(velBack, vel, 1e-12, 1e-12);
    expectVectorNear(accBack, acc, 1e-12, 1e-12);

    // 与逆变换组合应得到单位元
    AccelerationTransform identity = transform.composed(inversed);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i == j)
                EXPECT_NEAR(identity.getRotation().getMatrix()(i, j), 1.0, 1e-12);
            else
                EXPECT_NEAR(identity.getRotation().getMatrix()(i, j), 0.0, 1e-12);
        }
        EXPECT_NEAR(identity.getTranslation()[i], 0.0, 1e-12);
        EXPECT_NEAR(identity.getVelocity()[i], 0.0, 1e-12);
        EXPECT_NEAR(identity.getAcceleration()[i], 0.0, 1e-12);
        EXPECT_NEAR(identity.getRotationRate()[i], 0.0, 1e-12);
        EXPECT_NEAR(identity.getRotationRateDot()[i], 0.0, 1e-12);
    }
}

/*!
    退化一致性：平移加速度与角加速度均为零时，应与 KinematicTransform 完全一致。
*/
TEST(AccelerationTransformTest, DegenerateToKinematicTransform)
{
    Vector3d translation{4, -1, 2};
    Vector3d velocity{-3, 5, 1};
    KinematicRotation rotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7});

    AccelerationTransform accelTransform(translation, velocity, Vector3d::Zero(),
                                         AccelerationRotation(rotation, Vector3d::Zero()));
    KinematicTransform kinematicTransform(translation, velocity, rotation);

    Vector3d pos{1, -2, 3}, vel{4, -5, 6}, acc{7, 8, -9};
    Vector3d pos1{}, vel1{}, acc1{};
    Vector3d pos2{}, vel2{};
    accelTransform.transformPosVelAcc(pos, vel, acc, pos1, vel1, acc1);
    kinematicTransform.transformPositionVelocity(pos, vel, pos2, vel2);
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(pos1[i], pos2[i]);
        EXPECT_DOUBLE_EQ(vel1[i], vel2[i]);
    }

    // 求逆也应退化成一样的结果
    AccelerationTransform accelInversed = accelTransform.inverse();
    KinematicTransform kinematicInversed = kinematicTransform.inverse();
    for (int i = 0; i < 3; i++)
    {
        EXPECT_DOUBLE_EQ(accelInversed.getTranslation()[i], kinematicInversed.getTranslation()[i]);
        EXPECT_DOUBLE_EQ(accelInversed.getVelocity()[i], kinematicInversed.getVelocity()[i]);
        EXPECT_DOUBLE_EQ(accelInversed.getRotationRate()[i], kinematicInversed.getRotationRate()[i]);
    }

    // 逆变换（transformPosVelAccInv）与 inverse().transformPosVelAcc() 等价
    Vector3d pos3{}, vel3{}, acc3{};
    Vector3d pos4{}, vel4{}, acc4{};
    accelTransform.transformPosVelAccInv(pos, vel, acc, pos3, vel3, acc3);
    accelTransform.inverse().transformPosVelAcc(pos, vel, acc, pos4, vel4, acc4);
    expectVectorNear(pos3, pos4, 1e-12, 1e-12);
    expectVectorNear(vel3, vel4, 1e-12, 1e-12);
    expectVectorNear(acc3, acc4, 1e-12, 1e-12);
}

/*!
    零加速度不封闭于组合：两个平移加速度、角加速度均为零的变换组合后，加速度一般并不为零。
    因为 τ = M1ᵀt₂ 与 M1ᵀV₂ 的分量是在转动的中间系下分解的，其方向随时间变化，求二阶导时不消去：
        A = 2ω₁×(M₁ᵀV₂) + ω₁×(ω₁×τ)
        α = ω₁×(M₁ᵀω₂)
    只有 ω₁ ≡ 0（从而 α₁ = 0）且 A₁ = A₂ = 0 时才封闭。
*/
TEST(AccelerationTransformTest, ZeroAccelerationNotClosedUnderComposition)
{
    AccelerationTransform transform1(
        Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(),
        AccelerationRotation(Rotation(30_deg, Vector3d{4, -1, 2}), Vector3d{-1, -2, 7}, Vector3d::Zero()));
    AccelerationTransform transform2(
        Vector3d{3, 2, -5}, Vector3d{7, -2, 4}, Vector3d::Zero(),
        AccelerationRotation(Rotation(50_deg, Vector3d{3, 2, -5}), Vector3d{11, 3, -4}, Vector3d::Zero()));

    AccelerationTransform composed = transform1.composed(transform2);

    const Matrix3d& mat1 = transform1.getRotation().getMatrix();
    Vector3d angvel1 = transform1.getRotationRate();
    Vector3d tau = transform2.getTranslation() * mat1;      // 即 M₁ᵀ t₂
    Vector3d velocity2 = transform2.getVelocity() * mat1;   // 即 M₁ᵀ V₂
    Vector3d angvel2 = transform2.getRotationRate() * mat1; // 即 M₁ᵀ ω₂

    Vector3d accExpect = angvel1.cross(velocity2) * 2.0 + angvel1.cross(angvel1.cross(tau));
    Vector3d angvelDotExpect = angvel1.cross(angvel2);

    for (int i = 0; i < 3; i++)
    {
        EXPECT_NEAR(composed.getAcceleration()[i], accExpect[i], 1e-14);
        EXPECT_NEAR(composed.getRotationRateDot()[i], angvelDotExpect[i], 1e-14);
    }
    // 断言其确实非平凡，否则这条用例就没有意义了
    EXPECT_GT(composed.getAcceleration().norm(), 1.0);
    EXPECT_GT(composed.getRotationRateDot().norm(), 1.0);
}

/*!
    纯转动（绕自身原点，t₁ = V₁ = A₁ = 0）+ 中间系下分量为常数的纯平移（V₂ = A₂ = 0）：
    结果应恰好是向心项加欧拉项，A = ω₁×(ω₁×τ) + α₁×τ。
*/
TEST(AccelerationTransformTest, RotatingFrameCentripetal)
{
    const double omega0 = 0.3;
    const double alpha0 = -0.07;
    Vector3d angvel1{0, 0, omega0};
    Vector3d angvelDot1{0, 0, alpha0};
    Vector3d arm{3.0e4, -1.0e4, 2.0e3};

    AccelerationTransform rotation(
        Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(),
        AccelerationRotation(Rotation(0.4, Vector3d{0, 0, 1}), angvel1, angvelDot1));
    AccelerationTransform translation(
        arm, Vector3d::Zero(), Vector3d::Zero(), AccelerationRotation::Identity());

    AccelerationTransform composed = rotation.composed(translation);

    Vector3d tau = arm * rotation.getRotation().getMatrix();   // 即 M₁ᵀ t₂
    Vector3d translationExpect = tau;
    Vector3d velocityExpect = angvel1.cross(tau);
    Vector3d accelerationExpect = angvel1.cross(angvel1.cross(tau)) + angvelDot1.cross(tau);

    expectVectorNear(composed.getTranslation(), translationExpect, 1e-14, 1e-14);
    expectVectorNear(composed.getVelocity(), velocityExpect, 1e-14, 1e-14);
    expectVectorNear(composed.getAcceleration(), accelerationExpect, 1e-14, 1e-14);
    // 向心项必须非零，否则这条用例没有覆盖到它
    EXPECT_GT(accelerationExpect.norm(), 1.0);
}

/*!
    只有角加速度（ω₁ = 0，α₁ ≠ 0）时，ω₁×(ω₁×τ) 项应消失，A 恰好退化为 α₁×τ。
*/
TEST(AccelerationTransformTest, PureAngularAcceleration)
{
    Vector3d angvelDot1{0, 0, -0.07};
    Vector3d arm{3.0e4, -1.0e4, 2.0e3};

    AccelerationTransform rotation(
        Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(),
        AccelerationRotation(Rotation(0.4, Vector3d{0, 0, 1}), Vector3d::Zero(), angvelDot1));
    AccelerationTransform translation(
        arm, Vector3d::Zero(), Vector3d::Zero(), AccelerationRotation::Identity());

    AccelerationTransform composed = rotation.composed(translation);

    Vector3d tau = arm * rotation.getRotation().getMatrix();
    Vector3d accelerationExpect = angvelDot1.cross(tau);

    expectVectorNear(composed.getTranslation(), tau, 1e-14, 1e-14);
    expectVectorNear(composed.getVelocity(), Vector3d::Zero(), 1e-14, 1e-14);
    expectVectorNear(composed.getAcceleration(), accelerationExpect, 1e-14, 1e-14);
    EXPECT_GT(accelerationExpect.norm(), 1.0);
}

GTEST_MAIN()
