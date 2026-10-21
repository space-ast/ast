///
/// @file      testGibbs.cpp
/// @brief     测试 Gibbs 初轨确定
/// @details   用解析二体轨道点检验 aGibbs 求解的中间时刻速度
/// @author    axel
/// @date      2026-10-21
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

#include "AstEstimation/Gibbs.hpp"
#include "AstMath/Vector.hpp"
#include "ast/Test.hpp"

#include <cmath>

AST_USING_NAMESPACE

namespace
{

/// @brief 地球引力常数 [m³/s²]
constexpr double kGM = 3.986004418e14;

/// @brief 二体轨道状态（位置、速度）
struct ConicState
{
    Vector3d r;  ///< 位置 [m]
    Vector3d v;  ///< 速度 [m/s]
};

/// @brief 真近点角 nu 处的二体轨道状态（近焦点坐标系，p 为半通径）
ConicState aStateAtTrueAnomaly(double nu, double p, double e)
{
    const double radius = p / (1.0 + e * std::cos(nu));
    const double k = std::sqrt(kGM / p);
    ConicState state;
    state.r = Vector3d{radius * std::cos(nu), radius * std::sin(nu), 0.0};
    state.v = Vector3d{-k * std::sin(nu), k * (e + std::cos(nu)), 0.0};
    return state;
}

/// @brief 把近焦点系矢量旋转到任意惯性系（先绕 z 轴 0.7 rad，再绕 x 轴 0.4 rad）
Vector3d aToInertial(const Vector3d& v)
{
    const double cz = std::cos(0.7);
    const double sz = std::sin(0.7);
    const double cx = std::cos(0.4);
    const double sx = std::sin(0.4);
    const Vector3d v1{cz * v.x() - sz * v.y(), sz * v.x() + cz * v.y(), v.z()};
    return Vector3d{v1.x(), cx * v1.y() - sx * v1.z(), sx * v1.y() + cx * v1.z()};
}

}

TEST(Gibbs, EllipticOrbit)
{
    const double e = 0.3;
    const double p = 7.0e6 * (1.0 - e * e);
    const ConicState s1 = aStateAtTrueAnomaly(0.1, p, e);
    const ConicState s2 = aStateAtTrueAnomaly(0.9, p, e);
    const ConicState s3 = aStateAtTrueAnomaly(1.8, p, e);

    Vector3d v2;
    ASSERT_EQ(aGibbs(s1.r, s2.r, s3.r, kGM, v2), eNoError);
    EXPECT_NEAR((v2 - s2.v).norm(), 0.0, 1e-6);
}

TEST(Gibbs, CircularOrbit)
{
    // 圆轨道时 S→0，验证实现不依赖 S/|S| 的归一化
    const double e = 0.0;
    const double p = 7.0e6;
    const ConicState s1 = aStateAtTrueAnomaly(0.1, p, e);
    const ConicState s2 = aStateAtTrueAnomaly(1.0, p, e);
    const ConicState s3 = aStateAtTrueAnomaly(2.2, p, e);

    Vector3d v2;
    ASSERT_EQ(aGibbs(s1.r, s2.r, s3.r, kGM, v2), eNoError);
    EXPECT_NEAR((v2 - s2.v).norm(), 0.0, 1e-6);
}

TEST(Gibbs, HyperbolicOrbit)
{
    const double e = 1.5;
    const double a = -20.0e6;
    const double p = -a * (e * e - 1.0);  // 双曲线半通径
    const ConicState s1 = aStateAtTrueAnomaly(0.1, p, e);
    const ConicState s2 = aStateAtTrueAnomaly(0.8, p, e);
    const ConicState s3 = aStateAtTrueAnomaly(1.5, p, e);

    Vector3d v2;
    ASSERT_EQ(aGibbs(s1.r, s2.r, s3.r, kGM, v2), eNoError);
    EXPECT_NEAR((v2 - s2.v).norm(), 0.0, 1e-6);
}

TEST(Gibbs, ArbitraryOrientation)
{
    // 同一轨道整体旋转到任意惯性系后，解应随之旋转
    const double e = 0.3;
    const double p = 7.0e6 * (1.0 - e * e);
    const ConicState s1 = aStateAtTrueAnomaly(0.1, p, e);
    const ConicState s2 = aStateAtTrueAnomaly(0.9, p, e);
    const ConicState s3 = aStateAtTrueAnomaly(1.8, p, e);

    Vector3d v2;
    ASSERT_EQ(aGibbs(aToInertial(s1.r), aToInertial(s2.r), aToInertial(s3.r), kGM, v2), eNoError);
    EXPECT_NEAR((v2 - aToInertial(s2.v)).norm(), 0.0, 1e-6);
}

TEST(Gibbs, ReversedOrder)
{
    // 位置按时间倒序输入等价于反向运动，结果应为 -v2
    const double e = 0.3;
    const double p = 7.0e6 * (1.0 - e * e);
    const ConicState s1 = aStateAtTrueAnomaly(0.1, p, e);
    const ConicState s2 = aStateAtTrueAnomaly(0.9, p, e);
    const ConicState s3 = aStateAtTrueAnomaly(1.8, p, e);

    Vector3d vForward, vBackward;
    ASSERT_EQ(aGibbs(s1.r, s2.r, s3.r, kGM, vForward), eNoError);
    ASSERT_EQ(aGibbs(s3.r, s2.r, s1.r, kGM, vBackward), eNoError);
    EXPECT_NEAR((vForward + vBackward).norm(), 0.0, 1e-6);
}

TEST(Gibbs, InvalidArguments)
{
    Vector3d v2{1.0, 2.0, 3.0};
    const Vector3d r1{1.0e6, 0.0, 0.0};
    const Vector3d r2{0.0, 1.0e6, 0.0};
    const Vector3d r3{-1.0e6, 0.0, 0.0};

    // 位置矢量为零
    EXPECT_EQ(aGibbs(Vector3d::Zero(), r2, r3, kGM, v2), eErrorInvalidParam);
}

TEST(Gibbs, DegenerateGeometry)
{
    Vector3d v2;
    // 三点与中心天体共线：轨道面不唯一
    EXPECT_EQ(aGibbs({1.0e6, 0.0, 0.0}, {2.0e6, 0.0, 0.0}, {3.0e6, 0.0, 0.0}, kGM, v2),
              eErrorInvalidValue);
    // 三点重合
    EXPECT_EQ(aGibbs({1.0e6, 0.0, 0.0}, {1.0e6, 0.0, 0.0}, {1.0e6, 0.0, 0.0}, kGM, v2),
              eErrorInvalidValue);
}

GTEST_MAIN()
