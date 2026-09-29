///
/// @file      testAttitudeUtil.cpp
/// @brief     姿态计算工具函数测试
/// @details   覆盖双矢量定姿（Aligned and Constrained）的姿态与解析角速度
/// @author    axel
/// @date      2026-09-28
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/AttitudeUtil.hpp"
#include "ast/Rotation.hpp"
#include "ast/KinematicRotation.hpp"
#include "ast/Matrix.hpp"
#include "ast/Vector.hpp"
#include "ast/Test.h"

#include <cmath>

AST_USING_NAMESPACE

/// @brief 断言运动学旋转为单位旋转（不含旋转、不含角速度）
static void expectIdentity(const KinematicRotation& rotation)
{
    const Matrix3d& matrix = rotation.getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            EXPECT_NEAR(matrix(i, j), (i == j) ? 1.0 : 0.0, 1e-15);
        }
    }
    const Vector3d& angvel = rotation.getRotationRate();
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(angvel[i], 0.0, 1e-15);
    }
}


/// 圆轨道上"本体系 X 轴对齐位置方向、Y 轴约束到速度方向"等价于 LVLH 姿态，
/// 角速度应指向轨道法向且大小等于轨道角速度
TEST(AttitudeUtilTest, AlignConstrain_CircularOrbit)
{
    const double radius = 7000.0;      // 轨道半径
    const double rate = 1.0e-3;        // 轨道角速度
    const double theta = 0.7;          // 轨道面内的相位角，取非特殊值以免掩盖转置错误
    const double c = std::cos(theta);
    const double s = std::sin(theta);

    const Vector3d pos{radius * c, radius * s, 0.0};
    const Vector3d vel{-radius * rate * s, radius * rate * c, 0.0};
    const Vector3d acc{-radius * rate * rate * c, -radius * rate * rate * s, 0.0};

    const Vector3d axesVector1{1, 0, 0};        // 本体系 X 轴
    const Vector3d axesVector2{0, 1, 0};        // 本体系 Y 轴

    KinematicRotation rotation;
    errc_t rc = aAlignConstrainTransform(axesVector1, pos, vel,
                                         axesVector2, vel, acc,
                                         rotation);
    EXPECT_EQ(rc, eNoError);

    // 圆轨道上速度垂直于位置，两组三轴均为正交单位向量，故旋转矩阵的第 i 行即参考系第 i 轴
    //   f_1 = 位置方向, f_2 = 速度方向, f_3 = 轨道法向
    const Matrix3d& m = rotation.getMatrix();
    EXPECT_NEAR(m(0, 0), c, 1e-14);
    EXPECT_NEAR(m(0, 1), s, 1e-14);
    EXPECT_NEAR(m(0, 2), 0.0, 1e-14);
    EXPECT_NEAR(m(1, 0), -s, 1e-14);
    EXPECT_NEAR(m(1, 1), c, 1e-14);
    EXPECT_NEAR(m(1, 2), 0.0, 1e-14);
    EXPECT_NEAR(m(2, 0), 0.0, 1e-14);
    EXPECT_NEAR(m(2, 1), 0.0, 1e-14);
    EXPECT_NEAR(m(2, 2), 1.0, 1e-14);

    // 角速度指向轨道法向，大小等于轨道角速度，分量为参考系分量
    const Vector3d& angvel = rotation.getRotationRate();
    EXPECT_NEAR(angvel[0], 0.0, 1e-15);
    EXPECT_NEAR(angvel[1], 0.0, 1e-15);
    EXPECT_NEAR(angvel[2], rate, 1e-15);
}

/// 取非正交、非单位长度的向量，直接验证"对齐"与"约束"的定义，并检查旋转矩阵的正交性
TEST(AttitudeUtilTest, AlignConstrain_DefinitionHolds)
{
    const Vector3d axesVector1{2.0, 1.0, -0.5};     // 对齐向量(本体系分量)
    const Vector3d axesVector2{-1.0, 3.0, 1.0};     // 约束向量(本体系分量)
    const Vector3d refVector1{1.0, -2.0, 4.0};      // 对齐参考向量(参考系分量)
    const Vector3d refVector2{5.0, 1.0, -1.0};      // 约束参考向量(参考系分量)

    Rotation rotation;
    errc_t rc = aAlignConstrainTransform(axesVector1, refVector1,
                                         axesVector2, refVector2,
                                         rotation);
    EXPECT_EQ(rc, eNoError);

    // 对齐语义：本体系中的对齐向量方向变换到参考系后，应与参考向量平行且同向
    const Vector3d alignedInRef = rotation.transformVectorInv(axesVector1.normalized());
    EXPECT_NEAR(alignedInRef.cross(refVector1.normalized()).norm(), 0.0, 1e-14);
    EXPECT_NEAR(alignedInRef.dot(refVector1.normalized()), 1.0, 1e-14);

    // 约束语义：本体系中的约束向量方向变换到参考系后，应落在两参考向量张成的平面内，
    // 即与该平面的法向正交
    const Vector3d constrainedInRef = rotation.transformVectorInv(axesVector2.normalized());
    EXPECT_NEAR(constrainedInRef.dot(refVector1.cross(refVector2).normalized()), 0.0, 1e-14);

    // 旋转矩阵应为正交矩阵
    const Matrix3d& m = rotation.getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            double dotProduct = 0.0;
            for (int k = 0; k < 3; ++k)
            {
                dotProduct += m(i, k) * m(j, k);
            }
            EXPECT_NEAR(dotProduct, (i == j) ? 1.0 : 0.0, 1e-14);
        }
    }
}

/// 退化输入（零向量、两向量平行）应返回错误码，并输出单位旋转与零角速度
TEST(AttitudeUtilTest, AlignConstrain_Degenerate)
{
    const Vector3d zero{0, 0, 0};
    const Vector3d unitX{1, 0, 0};
    const Vector3d unitY{0, 1, 0};

    // 对齐参考向量为零向量
    {
        KinematicRotation rotation;
        errc_t rc = aAlignConstrainTransform(unitX, zero, zero,
                                             unitY, unitY, zero,
                                             rotation);
        EXPECT_EQ(rc, eErrorInvalidParam);
        expectIdentity(rotation);
    }

    // 约束参考向量与对齐参考向量平行
    {
        KinematicRotation rotation;
        errc_t rc = aAlignConstrainTransform(unitX, unitX, zero,
                                             unitY, unitX, zero,
                                             rotation);
        EXPECT_EQ(rc, eErrorInvalidParam);
        expectIdentity(rotation);
    }

    // 约束向量与对齐向量平行
    {
        KinematicRotation rotation;
        errc_t rc = aAlignConstrainTransform(unitX, unitX, zero,
                                             unitX, unitY, zero,
                                             rotation);
        EXPECT_EQ(rc, eErrorInvalidParam);
        expectIdentity(rotation);
    }

    // 对齐向量为零向量
    {
        KinematicRotation rotation;
        errc_t rc = aAlignConstrainTransform(zero, unitX, zero,
                                             unitY, unitY, zero,
                                             rotation);
        EXPECT_EQ(rc, eErrorInvalidParam);
        expectIdentity(rotation);
    }

    // 只求姿态的重载同样处理退化情形
    {
        Rotation rotation;
        errc_t rc = aAlignConstrainTransform(unitX, zero, unitY, unitY, rotation);
        EXPECT_EQ(rc, eErrorInvalidParam);
        const Matrix3d& m = rotation.getMatrix();
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                EXPECT_NEAR(m(i, j), (i == j) ? 1.0 : 0.0, 1e-15);
            }
        }
    }
}

GTEST_MAIN()
