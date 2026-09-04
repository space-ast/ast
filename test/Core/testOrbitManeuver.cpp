/// @file      testOrbitManeuver.cpp
/// @brief     拱点处切向机动 ΔV 计算测试
/// @details   测试 aApsisDeltaV：在拱点施加切向冲量使对向拱点达到指定半径所需的 ΔV。
/// @author    axel
/// @date      2026-08-29
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.

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

#include "ast/OrbitManeuver.hpp"
#include "ast/Constants.hpp"
#include "ast/AstTestMacro.h"
#include <cmath>
#include <cstdio>

AST_USING_NAMESPACE

// 测试常量
const double GM = 3.986004418e14;      // 地球引力参数 [m^3/s^2]（= kEarthGrav）
const double EPS = 1e-6;               // 速度增量容差 [m/s]

/// @brief 测试 GTO 场景：在近地点加速使远地点升高到目标半径
/// @details 当前轨道近地点 7000km、远地点 37000km，在近地点施加切向冲量使远地点
///          升到 42164km（同步高度）。期望值由独立 vis-viva 计算得到，硬编码以作交叉验证。
TEST(OrbitManeuver, GtoLiftApoapsis)
{
    printf("测试: GTO 近地点机动提升远地点\n");

    const double burnRadius      = 7.0e6;      // 近地点半径 [m]
    const double currentApoapsis = 3.7e7;      // 当前远地点半径 [m]
    const double targetApoapsis  = 4.2164e7;   // 目标远地点半径 [m]

    const double dv = aApsisDeltaV(burnRadius, currentApoapsis, targetApoapsis, GM);
    // 期望值（独立计算）：ΔV = sqrt(2·gm·T'/(R(R+T'))) − sqrt(2·gm·T/(R(R+T)))
    EXPECT_NEAR(dv, 96.75492752018909, EPS);
    EXPECT_TRUE(dv > 0.0);   // 加速提升远地点
}

/// @brief 测试往返可逆性：把对向拱点从 T 升到 T' 再降回，ΔV 应互为相反数
TEST(OrbitManeuver, RoundTripReversible)
{
    printf("测试: 往返可逆性\n");

    const double R  = 7.0e6;    // 机动点半径 [m]
    const double T  = 3.7e7;    // 起始对向拱点半径 [m]
    const double Tp = 4.2164e7; // 目标对向拱点半径 [m]

    const double dvUp   = aApsisDeltaV(R, T, Tp, GM);
    const double dvDown = aApsisDeltaV(R, Tp, T, GM);
    EXPECT_NEAR(dvUp, -dvDown, EPS);
}

/// @brief 测试圆轨道基准：从圆轨道（R=T）向偏圆过渡
/// @details R=T 时当前拱点速度即圆轨道速度 sqrt(gm/R)。期望值独立计算并硬编码。
TEST(OrbitManeuver, CircularReference)
{
    printf("测试: 圆轨道基准\n");

    const double R  = 7.0e6;    // 圆轨道半径 [m]
    const double Tp = 1.0e7;    // 目标对向拱点半径 [m]

    const double dv = aApsisDeltaV(R, R, Tp, GM);
    EXPECT_NEAR(dv, 638.7906846274864, EPS);

    // 当前拱点速度 = 圆轨道速度 sqrt(gm/R)，验证目标速度 v = v_circ + ΔV 满足 vis-viva
    const double vToday = std::sqrt(GM / R) + dv;
    EXPECT_NEAR(vToday, std::sqrt(2.0 * GM * Tp / (R * (R + Tp))), EPS);
}

/// @brief 测试远点机动降低近点（减速），ΔV 应为负
TEST(OrbitManeuver, ApoapsisLowerPeriapsis)
{
    printf("测试: 远点机动降低近点\n");

    const double burnRadius     = 4.2164e7;   // 远地点半径 [m]
    const double currentPeri    = 7.0e6;      // 当前近地点半径 [m]
    const double targetPeri     = 6.5e6;      // 目标近地点半径 [m]

    const double dv = aApsisDeltaV(burnRadius, currentPeri, targetPeri, GM);
    EXPECT_NEAR(dv, -51.581658066694445, EPS);
    EXPECT_TRUE(dv < 0.0);   // 减速降低对向拱点
}

/// @brief 测试 ModOrbElem 重载：在近拱点机动提升远拱点，应等于四参数基础函数结果
TEST(OrbitManeuver, ModElemOverload)
{
    printf("测试: ModOrbElem 重载\n");

    const double rp = 7.0e6;          // 近拱点半径 [m]
    const double apo = 3.7e7;         // 远拱点半径 [m]
    const double target = 4.2164e7;   // 目标远拱点半径 [m]
    const double ecc = (apo - rp) / (apo + rp);

    ModOrbElem moe;
    moe.rp_ = rp; moe.e_ = ecc;
    moe.i_ = 0.0; moe.raan_ = 0.0; moe.argper_ = 0.0; moe.trueA_ = 0.0;
    EXPECT_NEAR(aApsisDeltaV(moe, target, GM), 96.75492752018909, EPS);
}

/// @brief 测试 CartState 重载：以当前位置（近拱点）为机动点，应等于基础函数结果
TEST(OrbitManeuver, CartStateOverload)
{
    printf("测试: CartState 重载\n");

    const double rp = 7.0e6;          // 近拱点半径 [m]
    const double apo = 3.7e7;         // 远拱点半径 [m]
    const double target = 4.2164e7;   // 目标远拱点半径 [m]
    const double vPeri = std::sqrt(2.0 * GM * apo / (rp * (rp + apo)));   // 近拱点切向速度 [m/s]

    CartState cs;
    cs.pos_ = Vector3d{rp, 0.0, 0.0};    // 近拱点位置（沿 x 轴）
    cs.vel_ = Vector3d{0.0, vPeri, 0.0}; // 近拱点切向速度（沿 y 轴）
    EXPECT_NEAR(aApsisDeltaV(cs, target, GM), 96.75492752018909, EPS);
}

GTEST_MAIN()
