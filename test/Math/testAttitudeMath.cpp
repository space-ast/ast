///
/// @file      testAttitudeMath.cpp
/// @brief     AstMath/AttitudeConvert 模块单元测试
/// @details   测试旋转矩阵、欧拉角、四元数、轴角之间的转换
/// @author    Aist
/// @date      2026-04-14
/// @copyright 版权所有 (C) 2026-present, ast项目.

#include "ast/AttitudeConvert.hpp"
#include "ast/Euler.hpp"
#include "ast/AngleAxis.hpp"
#include "ast/Quaternion.hpp"
#include "ast/Matrix.hpp"
#include "ast/AstTestMacro.h"
#include <cmath>
#include "ast/Constants.h"

AST_USING_NAMESPACE

static void expectMatrixNear(const Matrix3d& a, const Matrix3d& b, double tol = 1e-10)
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_NEAR(a(i, j), b(i, j), tol);
}

static void expectQuatNear(const Quaternion& a, const Quaternion& b, double tol = 1e-10)
{
    bool same = (std::abs(a.w() - b.w()) < tol && 
                 std::abs(a.x() - b.x()) < tol &&
                 std::abs(a.y() - b.y()) < tol && 
                 std::abs(a.z() - b.z()) < tol);
    bool opposite = (std::abs(a.w() + b.w()) < tol && 
                     std::abs(a.x() + b.x()) < tol &&
                     std::abs(a.y() + b.y()) < tol && 
                     std::abs(a.z() + b.z()) < tol);
    EXPECT_TRUE(same || opposite);
}

// ============================================================================
// 旋转矩阵测试（被动旋转）
// ============================================================================

TEST(RotationMatrix, XAxis)
{
    // 被动旋转：mtx(1,2) = sin(angle), mtx(2,1) = -sin(angle)
    Matrix3d mtx;
    double angle = kPI / 4;
    aRotationXMatrix(angle, mtx);
    
    EXPECT_NEAR(mtx(0, 0), 1.0, 1e-15);
    EXPECT_NEAR(mtx(0, 1), 0.0, 1e-15);
    EXPECT_NEAR(mtx(0, 2), 0.0, 1e-15);
    EXPECT_NEAR(mtx(1, 0), 0.0, 1e-15);
    EXPECT_NEAR(mtx(1, 1), std::cos(angle), 1e-15);
    EXPECT_NEAR(mtx(1, 2), std::sin(angle), 1e-15);   // 被动旋转：+sin
    EXPECT_NEAR(mtx(2, 0), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 1), -std::sin(angle), 1e-15);  // 被动旋转：-sin
    EXPECT_NEAR(mtx(2, 2), std::cos(angle), 1e-15);
}

TEST(RotationMatrix, YAxis)
{
    // 被动旋转：mtx(0,2) = -sin(angle), mtx(2,0) = sin(angle)
    Matrix3d mtx;
    double angle = kPI / 6;
    aRotationYMatrix(angle, mtx);
    
    EXPECT_NEAR(mtx(0, 0), std::cos(angle), 1e-15);
    EXPECT_NEAR(mtx(0, 1), 0.0, 1e-15);
    EXPECT_NEAR(mtx(0, 2), -std::sin(angle), 1e-15);  // 被动旋转：-sin
    EXPECT_NEAR(mtx(1, 0), 0.0, 1e-15);
    EXPECT_NEAR(mtx(1, 1), 1.0, 1e-15);
    EXPECT_NEAR(mtx(1, 2), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 0), std::sin(angle), 1e-15);   // 被动旋转：+sin
    EXPECT_NEAR(mtx(2, 1), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 2), std::cos(angle), 1e-15);
}

TEST(RotationMatrix, ZAxis)
{
    // 被动旋转：mtx(0,1) = sin(angle), mtx(1,0) = -sin(angle)
    Matrix3d mtx;
    double angle = kPI / 3;
    aRotationZMatrix(angle, mtx);
    
    EXPECT_NEAR(mtx(0, 0), std::cos(angle), 1e-15);
    EXPECT_NEAR(mtx(0, 1), std::sin(angle), 1e-15);   // 被动旋转：+sin
    EXPECT_NEAR(mtx(0, 2), 0.0, 1e-15);
    EXPECT_NEAR(mtx(1, 0), -std::sin(angle), 1e-15);  // 被动旋转：-sin
    EXPECT_NEAR(mtx(1, 1), std::cos(angle), 1e-15);
    EXPECT_NEAR(mtx(1, 2), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 0), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 1), 0.0, 1e-15);
    EXPECT_NEAR(mtx(2, 2), 1.0, 1e-15);
}

TEST(RotationMatrix, GeneralAPI)
{
    Matrix3d mtx1, mtx2;
    double angle = kPI / 4;
    
    EXPECT_EQ(aRotationMatrix(angle, 1, mtx1), 0);
    aRotationXMatrix(angle, mtx2);
    expectMatrixNear(mtx1, mtx2);
    
    EXPECT_EQ(aRotationMatrix(angle, 2, mtx1), 0);
    aRotationYMatrix(angle, mtx2);
    expectMatrixNear(mtx1, mtx2);
    
    EXPECT_EQ(aRotationMatrix(angle, 3, mtx1), 0);
    aRotationZMatrix(angle, mtx2);
    expectMatrixNear(mtx1, mtx2);
}

TEST(RotationMatrix, Orthogonality)
{
    // 验证旋转矩阵的正交性
    Matrix3d mtx;
    aRotationXMatrix(kPI / 5, mtx);
    
    double det = mtx(0,0)*(mtx(1,1)*mtx(2,2) - mtx(1,2)*mtx(2,1))
               - mtx(0,1)*(mtx(1,0)*mtx(2,2) - mtx(1,2)*mtx(2,0))
               + mtx(0,2)*(mtx(1,0)*mtx(2,1) - mtx(1,1)*mtx(2,0));
    EXPECT_NEAR(det, 1.0, 1e-14);
}

// ============================================================================
// 四元数与矩阵转换测试
// ============================================================================

TEST(QuatMatrixConversion, Identity)
{
    Quaternion quat;
    quat.w() = 1.0; quat.x() = 0.0; quat.y() = 0.0; quat.z() = 0.0;
    
    Matrix3d mtx;
    aQuatToMatrix(quat, mtx);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_NEAR(mtx(i, j), (i == j) ? 1.0 : 0.0, 1e-15);
        }
    }
}

TEST(QuatMatrixConversion, RoundTrip)
{
    double angle = kPI / 3;
    Quaternion quat;
    quat.w() = std::cos(angle / 2);
    quat.x() = std::sin(angle / 2);
    quat.y() = 0.0;
    quat.z() = 0.0;
    
    Matrix3d mtx;
    aQuatToMatrix(quat, mtx);
    
    Quaternion quat2;
    aMatrixToQuat(mtx, quat2);
    
    expectQuatNear(quat, quat2);
}

TEST(QuatMatrixConversion, RotationX90)
{
    double angle = kPI / 2;
    Quaternion quat;
    quat.w() = std::cos(angle / 2);
    quat.x() = std::sin(angle / 2);
    quat.y() = 0.0;
    quat.z() = 0.0;
    
    Matrix3d mtx;
    aQuatToMatrix(quat, mtx);
    
    Matrix3d expected;
    aRotationXMatrix(angle, expected);
    expectMatrixNear(mtx, expected);
}

// ============================================================================
// 轴角转换测试
// ============================================================================

TEST(AngleAxisConversion, Basic)
{
    double angle = kPI / 2;
    Vector3d axis; axis(0) = 0; axis(1) = 0; axis(2) = 1;
    
    AngleAxis aa;
    aa.angle() = angle;
    aa.axis() = axis;
    
    Quaternion quat;
    aAngleAxisToQuat(aa, quat);
    
    EXPECT_NEAR(quat.w(), std::cos(angle / 2), 1e-10);
    EXPECT_NEAR(quat.z(), std::sin(angle / 2), 1e-10);
}

TEST(AngleAxisConversion, MatrixRoundTrip)
{
    double angle = kPI / 3;
    Vector3d axis; axis(0) = 1; axis(1) = 1; axis(2) = 1;
    double norm = std::sqrt(3.0);
    axis(0) /= norm; axis(1) /= norm; axis(2) /= norm;
    
    AngleAxis aa1;
    aa1.angle() = angle;
    aa1.axis() = axis;
    
    Matrix3d mtx;
    aAngleAxisToMatrix(aa1, mtx);
    
    AngleAxis aa2;
    aMatrixToAngleAxis(mtx, aa2);
    
    // 验证角度和旋转矩阵
    EXPECT_NEAR(std::abs(aa1.angle()), std::abs(aa2.angle()), 1e-10);
    
    Matrix3d mtx2;
    aAngleAxisToMatrix(aa2, mtx2);
    expectMatrixNear(mtx, mtx2);
}

TEST(AngleAxis, Inverse)
{
    double angle = kPI / 4;
    Vector3d axis; axis(0) = 0; axis(1) = 1; axis(2) = 0;
    
    AngleAxis aa;
    aa.angle() = angle;
    aa.axis() = axis;
    
    AngleAxis aa_inv = aa.inverse();
    EXPECT_NEAR(aa_inv.angle(), -angle, 1e-15);
    EXPECT_NEAR(aa_inv.axis()(0), axis(0), 1e-15);
    EXPECT_NEAR(aa_inv.axis()(1), axis(1), 1e-15);
    EXPECT_NEAR(aa_inv.axis()(2), axis(2), 1e-15);
}

TEST(AngleAxis, FromRotationMatrix)
{
    double angle = kPI / 4;
    Matrix3d mtx;
    aRotationYMatrix(angle, mtx);
    
    AngleAxis aa;
    aa.fromRotationMatrix(mtx);
    
    // 验证旋转矩阵是否一致
    Matrix3d mtx2 = aa.toRotationMatrix();
    expectMatrixNear(mtx, mtx2);
}

TEST(AngleAxis, ToRotationMatrix)
{
    double angle = kPI / 3;
    Vector3d axis; axis(0) = 0; axis(1) = 0; axis(2) = 1;
    
    AngleAxis aa;
    aa.angle() = angle;
    aa.axis() = axis;
    
    Matrix3d mtx = aa.toRotationMatrix();
    
    Matrix3d expected;
    aRotationZMatrix(angle, expected);
    expectMatrixNear(mtx, expected);
}

// ============================================================================
// 欧拉角转换测试
// ============================================================================

TEST(EulerConversion, XYZOrder)
{
    Euler euler;
    euler.angle1() = kPI / 6;
    euler.angle2() = kPI / 4;
    euler.angle3() = kPI / 3;
    
    Matrix3d mtx;
    EXPECT_EQ(euler.toMatrix(Euler::eXYZ, mtx), 0);
    
    Euler euler2;
    EXPECT_EQ(euler2.fromMatrix(mtx, Euler::eXYZ), 0);
    
    // 校验所有三个角度
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
}

TEST(EulerConversion, ZYXOrder)
{
    Euler euler;
    euler.angle1() = kPI / 8;
    euler.angle2() = kPI / 6;
    euler.angle3() = kPI / 4;
    
    Matrix3d mtx;
    EXPECT_EQ(euler.toMatrix(Euler::eZYX, mtx), 0);
    
    Euler euler2;
    EXPECT_EQ(euler2.fromMatrix(mtx, Euler::eZYX), 0);
    
    // 校验所有三个角度
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
}

TEST(EulerConversion, QuatRoundTrip)
{
    Euler euler1;
    euler1.angle1() = kPI / 4;
    euler1.angle2() = kPI / 6;
    euler1.angle3() = kPI / 3;
    
    Quaternion quat;
    EXPECT_EQ(euler1.toQuat(Euler::eXYZ, quat), 0);
    
    Euler euler2;
    EXPECT_EQ(euler2.fromQuat(quat, Euler::eXYZ), 0);
    
    // 校验所有三个角度
    EXPECT_NEAR(euler1.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler1.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler1.angle3(), euler2.angle3(), 1e-10);
}

TEST(EulerConversion, AllABCTypes)
{
    Euler euler;
    euler.angle1() = 0.2;
    euler.angle2() = 0.3;
    euler.angle3() = 0.4;
    
    Matrix3d mtx;
    Euler euler2;
    
    // XYZ (123)
    aEuler123ToMatrix(euler, mtx);
    aMatrixToEuler123(mtx, euler2);
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
    
    // ZYX (321)
    aEuler321ToMatrix(euler, mtx);
    aMatrixToEuler321(mtx, euler2);
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
}

TEST(EulerConversion, AllABATypes)
{
    Euler euler;
    euler.angle1() = 0.3;
    euler.angle2() = kPI / 4;
    euler.angle3() = 0.5;
    
    Matrix3d mtx;
    Euler euler2;
    
    // ZXZ (313)
    aEuler313ToMatrix(euler, mtx);
    aMatrixToEuler313(mtx, euler2);
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
    
    // ZYZ (323)
    aEuler323ToMatrix(euler, mtx);
    aMatrixToEuler323(mtx, euler2);
    EXPECT_NEAR(euler.angle1(), euler2.angle1(), 1e-10);
    EXPECT_NEAR(euler.angle2(), euler2.angle2(), 1e-10);
    EXPECT_NEAR(euler.angle3(), euler2.angle3(), 1e-10);
}

// ============================================================================
// 四元数转轴角测试
// ============================================================================

TEST(QuatAngleAxisConversion, RoundTrip)
{
    double angle = kPI / 3;
    Quaternion quat;
    double s = std::sin(angle / 2) / std::sqrt(3.0);
    quat.w() = std::cos(angle / 2);
    quat.x() = s; quat.y() = s; quat.z() = s;
    
    AngleAxis aa;
    aQuatToAngleAxis(quat, aa);
    
    Quaternion quat2;
    aAngleAxisToQuat(aa, quat2);
    
    expectQuatNear(quat, quat2);
}

GTEST_MAIN()
