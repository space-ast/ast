///
/// @file      testVinti6.cpp
/// @brief     Vinti6 解析解（J2/J3）轨道预报用例：km 与 SI 单位制对比。
/// @details   用同一条近地倾斜圆轨道，分别以 [km, km/s] 与 [m, m/s] 调用
///            Vinti6()；校验输出状态有限、Vinti 平均根数合法、往返一致，
///            并验证 SI 结果恰好等于 km 结果的 1000 倍（即物理上完全一致）。
/// @author    axel
/// @date      2026-09-01
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

#include "ast/Vinti6.h"
#include "ast/Vinti.hpp"
#include "ast/Vector.hpp"
#include "ast/AstTestMacro.h"
#include <cmath>

TEST(VintiTest, Vinti6)
{
    const double j2 = 1.0826266835531513e-3;
    const double j3 = -2.5326564853313e-6;
    const double c45 = 0.7071067811865476;               // cos(45°)
    const double t0 = 0.0;
    const double t1 = 600.0;                              // 预报 600 s

    // ---------------- km（约定单位） ----------------
    // planet：{ 赤道半径 ae(km), 引力常数 gm(km^3/s^2), J2, J3 } —— WGS84
    double planetKm[4] = { 6378.137, 398600.4418, j2, j3 };
    // 初始状态 (km, km/s)：半径 7000 km、倾角约 45° 的近地圆轨道
    const double vKm = std::sqrt(planetKm[1] / 7000.0);   // 圆轨道速度
    double x0Km[6] = { 7000.0, 0.0, 0.0,  0.0, vKm * c45, vKm * c45 };
    double x1Km[6] = { 0.0 };
    double oeKm[6] = { 0.0 };

    Vinti6(planetKm, t0, x0Km, t1, x1Km, oeKm);

    // 1) 终态应为有限值
    for (int i = 0; i < 6; ++i)
        EXPECT_TRUE(std::isfinite(x1Km[i]));

    // 2) Vinti 平均根数应合法：椭圆轨道 0 <= e < 1，p > 0，0 <= sin^2(I) <= 1
    EXPECT_GT(oeKm[0], 0.0);                              // p  = 圆锥曲线参数 > 0
    EXPECT_GE(oeKm[1], 0.0);                              // e  >= 0
    EXPECT_LT(oeKm[1], 1.0);                              // e  < 1（椭圆）
    EXPECT_GE(oeKm[2], 0.0);                              // sin^2(I) >= 0
    EXPECT_LE(oeKm[2], 1.0);                              // sin^2(I) <= 1

    // 3) km 版往返一致性：由 t1 反推回 t0，应恢复初始状态
    double x2Km[6] = { 0.0 };
    Vinti6(planetKm, t1, x1Km, t0, x2Km, oeKm);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(x2Km[i],     x0Km[i],     1e-6);      // 位置 (km)
        EXPECT_NEAR(x2Km[i + 3], x0Km[i + 3], 1e-10);     // 速度 (km/s)
    }

    // ---------------- SI 单位制（m, m/s） ----------------
    // 与 km 版严格成 1000 倍：ae 6378.137 km → 6378137 m, gm 398600.4418 km^3/s^2 → 3.986004418e14 m^3/s^2
    double planetSi[4] = { 6378137.0, 3.986004418e14, j2, j3 };
    double x0Si[6] = { 7000000.0, 0.0, 0.0,  0.0, vKm * 1000.0 * c45, vKm * 1000.0 * c45 };
    double x1Si[6] = { 0.0 };
    double oeSi[6] = { 0.0 };

    Vinti6(planetSi, t0, x0Si, t1, x1Si, oeSi);

    for (int i = 0; i < 6; ++i)
        EXPECT_TRUE(std::isfinite(x1Si[i]));

    // SI 版往返一致性（m, m/s）
    double x2Si[6] = { 0.0 };
    Vinti6(planetSi, t1, x1Si, t0, x2Si, oeSi);
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(x2Si[i],     x0Si[i],     1e-3);      // 位置 (m)
        EXPECT_NEAR(x2Si[i + 3], x0Si[i + 3], 1e-6);      // 速度 (m/s)
    }

    // 打印对比（km）
    printf("km: %.15g %.15g %.15g %.15g %.15g %.15g\n",
           x1Km[0], x1Km[1], x1Km[2], x1Km[3], x1Km[4], x1Km[5]);
    // 打印对比（SI，需换算为 km/km/s 以便读数）
    printf("si: %.15g %.15g %.15g %.15g %.15g %.15g\n",
           x1Si[0] / 1000.0, x1Si[1] / 1000.0, x1Si[2] / 1000.0,
           x1Si[3] / 1000.0, x1Si[4] / 1000.0, x1Si[5] / 1000.0);

    // ---------------- km 与 SI 结果对比 ----------------
    // 物理上应为同一条轨道：SI 输出恰为 km 输出的 1000 倍
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_NEAR(x1Si[i],     x1Km[i] * 1000.0,     1e-6);   // 位置 [m]
        EXPECT_NEAR(x1Si[i + 3], x1Km[i + 3] * 1000.0, 1e-9);   // 速度 [m/s]
    }
    // 平均根数（无量纲 / 秒 / 弧度）应一致。
    // 注：km 与 SI 两套输入在浮点除法 (7000/6378.137 与 7000000/6378137) 上舍入
    //     略有差异，经数十次运算累积到 ~1e-12 相对量级；容差取 1e-8 即可覆盖。
    EXPECT_NEAR(oeSi[0], oeKm[0], 1e-8);              // p
    EXPECT_NEAR(oeSi[1], oeKm[1], 1e-8);              // e
    EXPECT_NEAR(oeSi[2], oeKm[2], 1e-8);              // sin^2(I)
    EXPECT_NEAR(oeSi[3], oeKm[3], 1e-8);              // -beta1 (s)
    EXPECT_NEAR(oeSi[4], oeKm[4], 1e-8);              // beta2
    EXPECT_NEAR(oeSi[5], oeKm[5], 1e-8);              // beta3
}


TEST(VintiTest, VintiWrap)
{
    AST_USING_NAMESPACE
    {
        // ---- SI 输入
        const double re = 6378137.0;                         // 赤道半径 [m]
        const double gm = 3.986004418e14;                    // 引力常数 [m^3/s^2]
        const double j2 = 1.0826266835531513e-3;
        const double j3 = -2.5326564853313e-6;
        const double duration = 600.0;                       // 预报时长 [s]

        //  45° 近地圆轨道：r = 7000 km，v = sqrt(gm/r)
        Vector3d r  {7000000.0, 0.0, 0.0};
        const double v = std::sqrt(gm / 7000000.0);
        Vector3d vel{0.0, v * 0.7071067811865476, v * 0.7071067811865476};

        // 基准值：
        //   pos (km) = 5584.369559661  2982.65721046936  2982.0754195079
        //   vel (km/s) = -4.55255513343154  4.25694369250608  4.2541246163785
        const Vector3d rExp{
            5584369.559661,
            2982657.21046936,
            2982075.4195079 };
        const Vector3d vExp{
            -4552.55513343154,
            4256.94369250608,
            4254.1246163785 };

        // 调用封装
        errc_t err = aVinti(duration, gm, j2, j3, re, r, vel);

        EXPECT_EQ(err, eNoError);
        EXPECT_TRUE(std::isfinite(r.norm()));
        EXPECT_TRUE(std::isfinite(vel.norm()));

        printf("aVinti r: %.15g %.15g %.15g\n", r.x(), r.y(), r.z());
        printf("aVinti v: %.15g %.15g %.15g\n", vel.x(), vel.y(), vel.z());

        // 与基准值对比
        EXPECT_NEAR(r.x(),   rExp.x(), 1e-8);                // 位置 [m]
        EXPECT_NEAR(r.y(),   rExp.y(), 1e-8);
        EXPECT_NEAR(r.z(),   rExp.z(), 1e-8);
        EXPECT_NEAR(vel.x(), vExp.x(), 1e-10);                // 速度 [m/s]
        EXPECT_NEAR(vel.y(), vExp.y(), 1e-10);
        EXPECT_NEAR(vel.z(), vExp.z(), 1e-10);
    }
}

GTEST_MAIN()
