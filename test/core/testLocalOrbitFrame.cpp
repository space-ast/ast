///
/// @file      testLocalOrbitFrame.cpp
/// @brief     局部轨道坐标系测试
/// @details   测试VVLH、LVLH、VNC等局部轨道坐标系的转换矩阵计算
/// @author    axel
/// @date      2026-04-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见： 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/LocalOrbitFrame.hpp"
#include "ast/RunTime.hpp"
#include "ast/Matrix.hpp"
#include "ast/Rotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/AstTestMacro.h"

AST_USING_NAMESPACE

// 辅助函数：获取矩阵的某行（每行是一个轴向量）
static Vector3d getMatrixRow(const Matrix3d& m, int row) {
    return Vector3d{m(row, 0), m(row, 1), m(row, 2)};
}

// 辅助函数：检查是否为单位矩阵
static bool isIdentityMatrix(const Matrix3d& m, double tol = 1e-10) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (std::abs(m(i, j) - expected) > tol) return false;
        }
    }
    return true;
}

// 辅助函数：检查是否为正交矩阵（M * M^T = I）
static bool isOrthogonalMatrix(const Matrix3d& m, double tol = 1e-10) {
    Matrix3d product = m * m.transpose();
    return isIdentityMatrix(product, tol);
}

TEST(LocalOrbitFrame, FrameToVVLH)
{
    Vector3d pos{6778.0, 0.0, 0.0};
    Vector3d vel{0.0, 7.5, 0.0};
    
    Matrix3d matrix;
    errc_t rc = aFrameToVVLHMatrix(pos, vel, matrix);
    
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
    
    Vector3d zAxis = getMatrixRow(matrix, 2);
    Vector3d expectedZ = -pos.normalized();
    EXPECT_NEAR(zAxis.dot(expectedZ), 1.0, 1e-10);
    
    // 测试逆变换
    Matrix3d fromVVLH;
    aVVLHToFrameMatrix(pos, vel, fromVVLH);
    Matrix3d product = matrix * fromVVLH;
    EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
}

TEST(LocalOrbitFrame, FrameToLVLH)
{
    Vector3d pos{6778.0, 0.0, 0.0};
    Vector3d vel{0.0, 7.5, 0.0};
    
    Matrix3d matrix;
    errc_t rc = aFrameToLVLHMatrix(pos, vel, matrix);
    
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
    
    Vector3d xAxis = getMatrixRow(matrix, 0);
    Vector3d expectedX = pos.normalized();
    EXPECT_NEAR(xAxis.dot(expectedX), 1.0, 1e-10);
    
    Vector3d zAxis = getMatrixRow(matrix, 2);
    Vector3d expectedZ = pos.cross(vel).normalized();
    EXPECT_NEAR(zAxis.dot(expectedZ), 1.0, 1e-10);
    
    // 测试逆变换
    Matrix3d fromLVLH;
    aLVLHToFrameMatrix(pos, vel, fromLVLH);
    Matrix3d product = matrix * fromLVLH;
    EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
}

TEST(LocalOrbitFrame, FrameToVNC)
{
    Vector3d pos{6778.0, 0.0, 0.0};
    Vector3d vel{0.0, 7.5, 0.0};
    
    Matrix3d matrix;
    errc_t rc = aFrameToVNCMatrix(pos, vel, matrix);
    
    EXPECT_EQ(rc, eNoError);
    EXPECT_TRUE(isOrthogonalMatrix(matrix, 1e-10));
    
    Vector3d xAxis = getMatrixRow(matrix, 0);
    Vector3d expectedX = vel.normalized();
    EXPECT_NEAR(xAxis.dot(expectedX), 1.0, 1e-10);
    
    // 测试逆变换
    Matrix3d fromVNC;
    aVNCToFrameMatrix(pos, vel, fromVNC);
    Matrix3d product = matrix * fromVNC;
    EXPECT_TRUE(isIdentityMatrix(product, 1e-10));
}

TEST(LocalOrbitFrame, FrameToLVLHTransform)
{
    Vector3d pos{6778.0, 1234.0, 4321.0};
    Vector3d vel{-1.2, 7.5, 2.3};

    Matrix3d matrix;
    EXPECT_EQ(aFrameToLVLHMatrix(pos, vel, matrix), eNoError);

    // 旋转变换与矩阵版本一致
    Rotation rotation;
    EXPECT_EQ(aFrameToLVLHTransform(pos, vel, rotation), eNoError);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_NEAR(rotation.getMatrix()(i, j), matrix(i, j), 1e-12);

    // 角速度为轨道角速度 ω = (r×v)/|r|²
    KinematicRotation kinematic;
    EXPECT_EQ(aFrameToLVLHTransform(pos, vel, kinematic), eNoError);
    Vector3d expectedRate = pos.cross(vel) / pos.squaredNorm();
    EXPECT_NEAR((kinematic.getRotationRate() - expectedRate).norm(), 0.0, 1e-12);

    // 变换后位置沿X轴，速度只剩径向分量，横向和法向分量为零
    Vector3d posOut, velOut;
    kinematic.transformVectorVelocity(pos, vel, posOut, velOut);
    EXPECT_NEAR(posOut[0], pos.norm(), 1e-10);
    EXPECT_NEAR(posOut[1], 0.0, 1e-10);
    EXPECT_NEAR(posOut[2], 0.0, 1e-10);
    EXPECT_NEAR(velOut[0], pos.dot(vel) / pos.norm(), 1e-10);
    EXPECT_NEAR(velOut[1], 0.0, 1e-10);
    EXPECT_NEAR(velOut[2], 0.0, 1e-10);

    // 退化情形：输出单位运动学旋转
    {
        Vector3d posDegenerate{1.0, 0.0, 0.0};
        Vector3d velDegenerate{2.0, 0.0, 0.0};
        Rotation rotationDegenerate;
        EXPECT_EQ(aFrameToLVLHTransform(posDegenerate, velDegenerate, rotationDegenerate), eErrorInvalidParam);
        EXPECT_TRUE(isIdentityMatrix(rotationDegenerate.getMatrix()));

        KinematicRotation kinematicDegenerate;
        EXPECT_EQ(aFrameToLVLHTransform(posDegenerate, velDegenerate, kinematicDegenerate), eErrorInvalidParam);
        EXPECT_TRUE(isIdentityMatrix(kinematicDegenerate.getMatrix()));
        EXPECT_NEAR(kinematicDegenerate.getRotationRate().norm(), 0.0, 0.0);
    }
}

TEST(LocalOrbitFrame, FrameToVVLHTransform)
{
    Vector3d pos{6778.0, 1234.0, 4321.0};
    Vector3d vel{-1.2, 7.5, 2.3};

    Matrix3d matrix;
    EXPECT_EQ(aFrameToVVLHMatrix(pos, vel, matrix), eNoError);

    // 旋转变换与矩阵版本一致
    Rotation rotation;
    EXPECT_EQ(aFrameToVVLHTransform(pos, vel, rotation), eNoError);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_NEAR(rotation.getMatrix()(i, j), matrix(i, j), 1e-12);

    // 角速度为轨道角速度 ω = (r×v)/|r|²，与 LVLH 相同
    KinematicRotation kinematic;
    EXPECT_EQ(aFrameToVVLHTransform(pos, vel, kinematic), eNoError);
    Vector3d expectedRate = pos.cross(vel) / pos.squaredNorm();
    EXPECT_NEAR((kinematic.getRotationRate() - expectedRate).norm(), 0.0, 1e-12);

    // 变换后位置沿-Z轴，速度只剩径向分量，横向和法向分量为零
    Vector3d posOut, velOut;
    kinematic.transformVectorVelocity(pos, vel, posOut, velOut);
    EXPECT_NEAR(posOut[0], 0.0, 1e-10);
    EXPECT_NEAR(posOut[1], 0.0, 1e-10);
    EXPECT_NEAR(posOut[2], -pos.norm(), 1e-10);
    EXPECT_NEAR(velOut[0], 0.0, 1e-10);
    EXPECT_NEAR(velOut[1], 0.0, 1e-10);
    EXPECT_NEAR(velOut[2], -pos.dot(vel) / pos.norm(), 1e-10);

    // 与 LVLH 只差一组固定的轴重排
    Matrix3d lvlh;
    EXPECT_EQ(aFrameToLVLHMatrix(pos, vel, lvlh), eNoError);
    const Matrix3d permutation = {
        0.0, 1.0, 0.0,
        0.0, 0.0, -1.0,
        -1.0, 0.0, 0.0,
    };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            EXPECT_NEAR(matrix(i, j), (permutation * lvlh)(i, j), 1e-12);

    // 退化情形：输出单位运动学旋转
    {
        Vector3d posDegenerate{1.0, 0.0, 0.0};
        Vector3d velDegenerate{2.0, 0.0, 0.0};
        Rotation rotationDegenerate;
        EXPECT_EQ(aFrameToVVLHTransform(posDegenerate, velDegenerate, rotationDegenerate), eErrorInvalidParam);
        EXPECT_TRUE(isIdentityMatrix(rotationDegenerate.getMatrix()));

        KinematicRotation kinematicDegenerate;
        EXPECT_EQ(aFrameToVVLHTransform(posDegenerate, velDegenerate, kinematicDegenerate), eErrorInvalidParam);
        EXPECT_TRUE(isIdentityMatrix(kinematicDegenerate.getMatrix()));
        EXPECT_NEAR(kinematicDegenerate.getRotationRate().norm(), 0.0, 0.0);
    }
}

TEST(LocalOrbitFrame, ErrorCases)
{
    // pos和vel同方向
    {
        Vector3d pos{1.0, 0.0, 0.0};
        Vector3d vel{1.0, 0.0, 0.0};
        Matrix3d matrix;
        EXPECT_EQ(aFrameToVVLHMatrix(pos, vel, matrix), eErrorInvalidParam);
    }
    // pos为零
    {
        Vector3d pos{0.0, 0.0, 0.0};
        Vector3d vel{1.0, 0.0, 0.0};
        Matrix3d matrix;
        EXPECT_EQ(aFrameToLVLHMatrix(pos, vel, matrix), eErrorInvalidParam);
    }
    // vel为零
    {
        Vector3d pos{1.0, 0.0, 0.0};
        Vector3d vel{0.0, 0.0, 0.0};
        Matrix3d matrix;
        EXPECT_EQ(aFrameToVNCMatrix(pos, vel, matrix), eErrorInvalidParam);
    }
}

GTEST_MAIN()
