///
/// @file      testOrbitGeometry.cpp
/// @brief     轨道几何单元测试
/// @details   验证公共交线法求两轨道路径最短距离，以及共面退化返回错误码。
/// @author    axel
/// @date      2026-08-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "ast/AstTestMacro.h"
#include "ast/OrbitGeometry.hpp"
#include "ast/OrbitElement.hpp"
#include "ast/TimeIntervalList.hpp"
#include "ast/Constants.h"
#include "ast/Literals.hpp"
#include <cmath>
#include <string>
#include <vector>


AST_USING_NAMESPACE
using namespace literals;

// ==================== 轨道面法向量 ====================

TEST(OrbitGeometry, PlaneNormal)
{
    Vector3d n;

    // 赤道面 (i=0)：法向量指向 +Z
    aOrbitPlaneNormal(0.0, 0.0, n);
    EXPECT_NEAR(n.x(), 0.0, 1e-12);
    EXPECT_NEAR(n.y(), 0.0, 1e-12);
    EXPECT_NEAR(n.z(), 1.0, 1e-12);

    // 极地轨道 (i=90°, raan=0)：法向量指向 -Y
    aOrbitPlaneNormal(kHalfPI, 0.0, n);
    EXPECT_NEAR(n.x(), 0.0, 1e-12);
    EXPECT_NEAR(n.y(), -1.0, 1e-12);
    EXPECT_NEAR(n.z(), 0.0, 1e-12);

    // 任意倾角下法向量仍为单位向量
    aOrbitPlaneNormal(0.9, 2.1, n);
    EXPECT_NEAR(n.norm(), 1.0, 1e-12);
}

// ==================== 真近点角处半径 ====================

TEST(OrbitGeometry, OrbitRadius)
{
    // 圆轨道：任意 ν 处半径恒等于长半轴
    EXPECT_NEAR(aOrbitRadius(7000e3, 0.0, 0.0), 7000e3, 1e-9);
    EXPECT_NEAR(aOrbitRadius(7000e3, 0.0, kPI), 7000e3, 1e-9);

    // 椭圆轨道 (a=8000km, e=0.5)：近拱点 4000km，远拱点 12000km
    EXPECT_NEAR(aOrbitRadius(8000e3, 0.5, 0.0), 4000e3, 1e-9);
    EXPECT_NEAR(aOrbitRadius(8000e3, 0.5, kPI), 12000e3, 1e-9);
}

// ==================== perifocal 基向量 ====================

TEST(OrbitGeometry, PerifocalBasis)
{
    Vector3d p, q;

    // 赤道面、近拱点在 +X：P=(1,0,0)，Q=(0,1,0)
    OrbElem e1{7000e3, 0.0, 0.0, 0.0, 0.0, 0.0};
    aOrbitPerifocalBasis(e1, p, q);
    EXPECT_NEAR(p.x(), 1.0, 1e-12);
    EXPECT_NEAR(p.y(), 0.0, 1e-12);
    EXPECT_NEAR(p.z(), 0.0, 1e-12);
    EXPECT_NEAR(q.x(), 0.0, 1e-12);
    EXPECT_NEAR(q.y(), 1.0, 1e-12);
    EXPECT_NEAR(q.z(), 0.0, 1e-12);

    // 一般情况：P、Q 为单位向量且正交，P×Q 等于轨道面法向
    OrbElem e2{8000e3, 0.3, 0.9, 2.1, 1.3, 0.0};
    aOrbitPerifocalBasis(e2, p, q);
    EXPECT_NEAR(p.norm(), 1.0, 1e-12);
    EXPECT_NEAR(q.norm(), 1.0, 1e-12);
    EXPECT_NEAR(p.dot(q), 0.0, 1e-12);

    Vector3d n, w;
    aOrbitPlaneNormal(e2.i_, e2.raan_, n);
    w = p.cross(q);
    EXPECT_NEAR(w.x(), n.x(), 1e-12);
    EXPECT_NEAR(w.y(), n.y(), 1e-12);
    EXPECT_NEAR(w.z(), n.z(), 1e-12);
}

// ==================== 公共交线法 ====================

// 赤道圆轨道 (a=7000km) 与极地圆轨道 (a=8000km)：两轨道均与 x 轴交于 ±a，
// 最短路径距离 = |7000km - 8000km| = 1000km。
TEST(OrbitGeometry, MinDistanceEquatorialPolar)
{
    OrbElem orb1{7000e3, 0.0, 0.0,     0.0, 0.0, 0.0}; // 赤道圆轨道
    OrbElem orb2{8000e3, 0.0, kHalfPI, 0.0, 0.0, 0.0}; // 极地圆轨道

    double dist = 0.0;
    EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
    EXPECT_NEAR(dist, 1000e3, 1.0);
}

// 交线在两个方向上给出的两个距离值应各自等于 1000km。
TEST(OrbitGeometry, NodeLineDistancesSymmetric)
{
    OrbElem orb1{7000e3, 0.0, 0.0,     0.0, 0.0, 0.0};
    OrbElem orb2{8000e3, 0.0, kHalfPI, 0.0, 0.0, 0.0};

    double distPlus = 0.0, distMinus = 0.0;
    EXPECT_EQ(aOrbitNodeLineDistances(orb1, orb2, distPlus, distMinus), eNoError);
    EXPECT_NEAR(distPlus, 1000e3, 1.0);
    EXPECT_NEAR(distMinus, 1000e3, 1.0);
}

// 偏心极地轨道 (a=8000km, e=0.5, argper=0)：近拱点在 +x（r=4000km），远拱点在 -x（r=12000km）。
// 与赤道圆轨道 (a=7000km) 相比：
//   distPlus  = |7000 - 4000| = 3000km
//   distMinus = |7000 - 12000| = 5000km
//   min = 3000km
TEST(OrbitGeometry, NodeLineDistancesEccentric)
{
    OrbElem orb1{7000e3, 0.0, 0.0,     0.0, 0.0, 0.0}; // 赤道圆轨道
    OrbElem orb2{8000e3, 0.5, kHalfPI, 0.0, 0.0, 0.0}; // 偏心极地轨道

    double distPlus = 0.0, distMinus = 0.0;
    EXPECT_EQ(aOrbitNodeLineDistances(orb1, orb2, distPlus, distMinus), eNoError);
    EXPECT_NEAR(distPlus, 3000e3, 1.0);
    EXPECT_NEAR(distMinus, 5000e3, 1.0);

    double dist = 0.0;
    EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
    EXPECT_NEAR(dist, 3000e3, 1.0);
}

// ==================== 共面退化 ====================

// 两轨道同平面（相同 i/raan）→ 公共交线退化，返回错误码；一维回退给出最短距离。
TEST(OrbitGeometry, CoplanarConcentric)
{
    OrbElem orb1{7000e3, 0.0, 0.5, 0.0, 0.0, 0.0};
    OrbElem orb2{8000e3, 0.0, 0.5, 0.0, 0.0, 0.0};

    double distPlus = 0.0, distMinus = 0.0;
    EXPECT_NE(aOrbitNodeLineDistances(orb1, orb2, distPlus, distMinus), eNoError);

    double dist = 0.0;
    EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
    EXPECT_NEAR(dist, 1000e3, 1.0);
}

// 内圆 (a=3000km) 与外椭圆 (a=8000km, e=0.5, 近拱 4000km) 同平面无交点：
// 最近点在外椭圆近拱方向，距离 = 4000 - 3000 = 1000km。
TEST(OrbitGeometry, CoplanarOneInsideAnother)
{
    OrbElem orb1{3000e3, 0.0, 0.0, 0.0, 0.0, 0.0};  // 内圆
    OrbElem orb2{8000e3, 0.5, 0.0, 0.0, 0.0, 0.0};  // 外椭圆，近拱 +x

    double dist = 0.0;
    EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
    EXPECT_NEAR(dist, 1000e3, 1.0);
}

// 圆 (a=7000km) 与椭圆 (a=8000km, e=0.5, r∈[4000,12000]km) 相交 → 距离 0。
TEST(OrbitGeometry, CoplanarIntersecting)
{
    OrbElem orb1{7000e3, 0.0, 0.0, 0.0, 0.0, 0.0};
    OrbElem orb2{8000e3, 0.5, 0.0, 0.0, 0.0, 0.0};

    double dist = 0.0;
    EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
    EXPECT_NEAR(dist, 0.0, 1.0);
}

// 逆行共面（n2 = -n1）：外椭圆完全包裹内椭圆，两近拱点均指向 +x。
// 最近点在外椭圆近拱方向，距离 = 5000 - 1000 = 4000km。
TEST(OrbitGeometry, CoplanarRetrograde)
{
    {
        OrbElem orb1{10000e3, 0.5, 0.5,       0.0, 0.0, 0.0};   // 顺行，近拱 +x (r=5000)
        OrbElem orb2{2000e3,  0.5, kPI - 0.5, kPI, kPI, 0.0};   // 逆行共面，近拱 +x (r=1000)

        double dist = 0.0;
        EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
        EXPECT_NEAR(dist, 4000e3, 1.0);
    }
    {
        OrbElem orb1{10000e3, 0.5, 0.5,       0.0, 0.0, 0.0};   // 顺行，近拱 +x (r=5000)
        OrbElem orb2{10000e3, 0.5, kPI - 0.5, kPI, 0.0, 0.0};   // 逆行共面，近拱 +x (r=1000)

        double dist = 0.0;
        EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
        EXPECT_NEAR(dist, 0, 1.0);
    }
    {
        OrbElem orb1{10000e3, 0.5, 0.5,       0.2,       0.3,       0.0};   // 顺行，近拱 +x (r=5000)
        OrbElem orb2{ 2000e3, 0.5, kPI - 0.5, kPI + 0.2, kPI - 0.3, 0.0};   // 逆行共面，近拱 +x (r=1000)

        double dist = 0.0;
        EXPECT_EQ(aOrbitPathMinDistance(orb1, orb2, dist), eNoError);
        EXPECT_NEAR(dist, 4000e3, 1.0);
    }
}

// ==================== 轨道面邻近时段 ====================

// 共面（相同 i/raan）→ d ≡ 0，整个窗口均为一个区间
TEST(OrbitGeometry, PlaneProximityCoplanar)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    OrbElem orbit{7000e3, 0.0, 0.5, 0.0, 0.0, 0.0};
    OrbElem other{8000e3, 0.0, 0.5, 0.0, 0.0, 0.0};   // 同 i/raan → 共面

    TimeInterval window(epoch, epoch + 1000.0);
    TimeIntervalList result;
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, 100e3, window, result), eNoError);

    ASSERT_EQ(result.size(), 1u);
    const TimeInterval ti = result[0];
    EXPECT_NEAR(ti.start() - window.start(), 0.0, 1e-6);
    EXPECT_NEAR(ti.stop() - window.stop(), 0.0, 1e-6);
}

// 极地圆轨道 vs 赤道面：每圈两个节点（ν=0、ν=π）附近各有一段邻近区间。
// window = 1 周期（自升交点起）：升交点区间跨 0/2π 分离成首段 + 末段，
// 加上降交点区间共 3 段，总时长 = 4·asin(threshold/a)/n。
TEST(OrbitGeometry, PlaneProximityPolarVsEquatorial)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    const double threshold = 700e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, 0.0};   // 极地圆轨道
    OrbElem other{a, 0.0, 0.0, 0.0, 0.0, 0.0};       // 赤道面

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;

    TimeInterval window(epoch, epoch + T);
    TimeIntervalList result;
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, threshold, window, result), eNoError);

    const double deltaNu = std::asin(threshold / a);
    const double expectedTotal = 4.0 * deltaNu / n;

    ASSERT_EQ(result.size(), 3u);
    EXPECT_NEAR(result.totalDuration(), expectedTotal, 1e-6 * expectedTotal + 1e-6);
}

// 阈值超过最大面距（极轨最大 |z| = a）→ 整圈均在阈值内，窗口为一个区间
TEST(OrbitGeometry, PlaneProximityThresholdCoversOrbit)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, 0.0};
    OrbElem other{a, 0.0, 0.0, 0.0, 0.0, 0.0};

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;

    TimeInterval window(epoch, epoch + T);
    TimeIntervalList result;
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, 8000e3 /* > a */, window, result), eNoError);

    ASSERT_EQ(result.size(), 1u);
    const TimeInterval ti = result[0];
    EXPECT_NEAR(ti.start() - window.start(), 0.0, 1e-6);
    EXPECT_NEAR(ti.stop() - window.stop(), 0.0, 1e-6);
}

// 非法参数：threshold ≤ 0、e ≥ 1、无限窗口 → 均返回 eErrorInvalidParam
TEST(OrbitGeometry, PlaneProximityInvalidParams)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    OrbElem orbit{7000e3, 0.0, 0.5, 0.0, 0.0, 0.0};
    OrbElem other{8000e3, 0.0, 0.5, 0.0, 0.0, 0.0};
    TimeInterval window(epoch, epoch + 1000.0);
    TimeIntervalList result;

    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, 0.0, window, result), eErrorInvalidParam);
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, -1.0, window, result), eErrorInvalidParam);

    OrbElem hyper{7000e3, 1.0, 0.5, 0.0, 0.0, 0.0};
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, hyper, other, kEarthGrav, 100e3, window, result), eErrorInvalidParam);

    TimeInterval infWin;
    infWin.setWhole();
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, 100e3, infWin, result), eErrorInvalidParam);
}

// 窗口裁剪：窄窗口套住降交点（ν=π → t=T/2），仅覆盖中心一半时长
TEST(OrbitGeometry, PlaneProximityWindowClipping)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    const double threshold = 700e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, 0.0};
    OrbElem other{a, 0.0, 0.0, 0.0, 0.0, 0.0};

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;
    const double deltaT = std::asin(threshold / a) / n;

    TimeInterval window(epoch + T / 2.0 - 0.5 * deltaT, epoch + T / 2.0 + 0.5 * deltaT);
    TimeIntervalList result;
    EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, threshold, window, result), eNoError);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_NEAR(result.totalDuration(), deltaT, 1e-6 * deltaT + 1e-6);
}

// 用 aOrbitPlaneDistance 独立复核 aOrbitPlaneProximityIntervals 返回的区间：
// 区间内 |d| ≤ threshold；区间外（窗口首尾及相邻区间空隙）|d| > threshold；
// 区间端点（非窗口裁剪处）|d| 恰为 threshold。轨道两两配对 × 多组阈值反复验证。
TEST(OrbitGeometry, PlaneProximityIntervalsVerifyByDistance)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);

    // 一组轨道根数 {a, e, i, raan, argper, trueA}，覆盖顺行/逆行、圆/椭圆、近赤道/极地、
    // 高中低偏心率等特殊情况。任意两两配对（含反向）互为 orbit/other 验证。
    const OrbElem orbits[] = {
        // —— 圆轨道 e=0 ——
        { 7000e3, 0.0,  kHalfPI,     0.0, 0.0, 0.0},   // 极地圆
        { 6800e3, 0.0,  0.7,         2.5, 1.1, 1.7},   // 中等倾角圆
        { 7200e3, 0.0,  0.3,         1.0, 0.5, 0.8},   // 低倾角圆
        {10000e3, 0.0,  1.9,         0.7, 2.0, 1.2},   // 逆行倾角圆
        { 9000e3, 0.0,  0.15,        2.0, 0.9, 2.4},   // 近赤道圆
        // —— 低偏心率椭圆 0<e≤0.2 ——
        { 8000e3, 0.05, 1.1,         1.5, 0.9, 0.4},   // 近圆倾斜
        { 9500e3, 0.1,  0.5,         2.8, 1.7, 2.0},   // 低倾角近圆
        { 7500e3, 0.15, 2.6,         0.2, 2.6, 1.0},   // 逆行近圆
        {11000e3, 0.2,  1.3,         3.1, 0.3, 2.6},   // 中等倾角近圆
        { 7300e3, 0.2,  0.05,        0.4, 1.6, 0.2},   // 近赤道近圆
        // —— 中偏心率椭圆 0.2<e≤0.5 ——
        { 8000e3, 0.3,  0.6,         0.5, 1.0, 0.3},   // 顺行倾斜椭圆
        { 9000e3, 0.35, 1.5,         2.2, 1.4, 1.9},   // 中等倾角椭圆
        { 8500e3, 0.4,  0.2,         1.3, 2.1, 0.7},   // 低倾角椭圆
        { 7800e3, 0.45, 2.9,         0.9, 0.7, 2.4},   // 逆行椭圆
        {12000e3, 0.5,  1.7,         1.8, 2.8, 0.5},   // 大 a 椭圆
        { 6700e3, 0.5,  0.8,         2.9, 1.2, 2.9},   // 小 a 椭圆
        { 6900e3, 0.3,  kPI - 0.05,  1.9, 0.4, 1.4},   // 近逆行赤道椭圆
        { 7300e3, 0.25, kHalfPI,     0.6, 1.3, 0.9},   // 极地椭圆
        // —— 高偏心率椭圆 e>0.5 ——
        {10000e3, 0.5,  0.3,         2.0, 0.8, 1.5},   // 低倾角大椭圆
        { 9000e3, 0.6,  2.3,         1.6, 0.2, 2.8},   // 逆行高椭圆
        {15000e3, 0.7,  1.2,         0.9, 2.4, 0.1},   // 高偏心率椭圆
        { 6500e3, 0.6,  2.0,         1.7, 3.0, 2.2},   // 小 a 逆行高倾角椭圆
        {13000e3, 0.8,  0.9,         2.4, 1.5, 1.1},   // 高偏心率
        {20000e3, 0.85, 1.6,         0.3, 2.9, 0.6},   // 大 a 高偏心率
        { 6200e3, 0.75, 2.7,         2.1, 0.6, 2.5},   // 小 a 高偏心率逆行
        {18000e3, 0.9,  0.4,         1.1, 1.8, 1.3},   // 极高偏心率低倾角
        {14000e3, 0.65, 1.4,         2.7, 0.1, 2.3},   // 大 a 中高偏心率
        // —— 其他特殊组合 ——
        { 9900e3, 0.55, 1.05,        3.0, 2.2, 0.0},   // 中等倾角高椭圆 trueA=0
        { 8800e3, 0.4,  2.2,         1.4, 0.8, 1.6},   // 逆行中等椭圆
        { 7600e3, 0.7,  2.85,        2.3, 1.9, 0.3},   // 逆行高椭圆
        // —— 追加：更多倾角/偏心率组合 ——
        { 8200e3, 0.3,  0.85,        0.8,  0.6, 1.8},   // 顺行倾斜椭圆
        { 9600e3, 0.0,  1.25,        2.6,  1.5, 0.7},   // 中等倾角圆
        { 7000e3, 0.6,  0.35,        0.15, 2.4, 2.7},   // 低倾角高椭圆
        { 8900e3, 0.2,  2.45,        2.4,  0.9, 1.1},   // 逆行近圆
        {10800e3, 0.45, 1.55,        1.0,  1.9, 0.5},   // 中等倾角椭圆
        { 6400e3, 0.1,  0.65,        1.9,  2.8, 2.2},   // 低倾角近圆
        { 7700e3, 0.7,  2.15,        0.35, 1.2, 0.9},   // 逆行高椭圆
        { 9200e3, 0.35, 1.85,        2.7,  0.4, 2.9},   // 逆行中等椭圆
        { 8300e3, 0.5,  0.55,        0.25, 2.1, 0.3},   // 低倾角椭圆
        {11500e3, 0.8,  2.75,        1.15, 0.7, 1.6},   // 逆行高偏心率
    };

    const double relSlack = 1e-9;    // 区间内/外的数值余量（相对）
    const double boundaryTol = 1e-6; // 判定端点是否为窗口裁剪边界 [s]

    const size_t nOrbit = sizeof(orbits) / sizeof(orbits[0]);
    for (size_t pi = 0; pi < nOrbit; ++pi)
    {
        for (size_t pj = 0; pj < nOrbit; ++pj)
        {
            const OrbElem& orbit = orbits[pi];
            const OrbElem& other = orbits[pj];
            const double a = orbit.a_;
            const double n = std::sqrt(kEarthGrav / (a * a * a));
            const double T = kTwoPI / n;

            const TimeInterval window(epoch, epoch + 7.0 * T);   // 分析窗口取 7 个周期，覆盖跨圈复制与相邻圈合并

            auto absDist = [&](const TimePoint& t)
            {
                double d = 0.0;
                EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, t, d), eNoError);
                return std::fabs(d);
            };

            // 多组阈值（绝对米）：从远小于最大面距到超过最大面距，覆盖窄区间/普通区间/部分覆盖/整圈覆盖。
            const double thresholds[] = {1_m, 1_km, 30_km, 100_km, 200_km, 0.001 * a, 0.05 * a, 0.15 * a, 0.3 * a, 0.5 * a, 1.0 * a, 1.5 * a, 2 * a, 10 * a};

            for (double threshold : thresholds)
            {
                TimeIntervalList result;
                EXPECT_EQ(aOrbitPlaneProximityIntervals(epoch, orbit, other, kEarthGrav, threshold, window, result), eNoError);
                ASSERT_GE(result.size(), 1u);

                // 取出区间副本（operator[] 返回临时对象，避免对其 start/stop 取引用悬空）
                std::vector<TimeInterval> ivs(result.begin(), result.end());

                // 区间内采样：|d| ≤ threshold
                for (size_t i = 0; i < ivs.size(); ++i)
                {
                    const TimePoint lo = ivs[i].start();
                    const TimePoint hi = ivs[i].stop();
                    const double dur = hi - lo;

                    for (int k = 1; k < 10; ++k)
                    {
                        const TimePoint t = lo + dur * (k / 10.0);
                        EXPECT_LE(absDist(t), threshold * (1.0 + relSlack)) << "pair " << pi << "-" << pj << " threshold " << threshold << " interval " << i << " interior k=" << k;
                    }
                }

                // 区间端点：非窗口裁剪处 |d| 恰为 threshold，裁剪处仍在区间内
                for (size_t i = 0; i < ivs.size(); ++i)
                {
                    const TimePoint lo = ivs[i].start();
                    const TimePoint hi = ivs[i].stop();

                    if ((lo - window.start()) < boundaryTol)
                        EXPECT_LE(absDist(lo), threshold * (1.0 + relSlack)) << "pair " << pi << "-" << pj << " threshold " << threshold << " interval " << i << " clipped start";
                    else
                        EXPECT_NEAR(absDist(lo), threshold, 1e-3) << "pair " << pi << "-" << pj << " threshold " << threshold << " interval " << i << " start";

                    if ((window.stop() - hi) < boundaryTol)
                        EXPECT_LE(absDist(hi), threshold * (1.0 + relSlack)) << "pair " << pi << "-" << pj << " threshold " << threshold << " interval " << i << " clipped stop";
                    else
                        EXPECT_NEAR(absDist(hi), threshold, 1e-3) << "pair " << pi << "-" << pj << " threshold " << threshold << " interval " << i << " stop";
                }

                // 区间外采样（窗口首尾及相邻区间之间的空隙）：|d| > threshold
                auto checkGap = [&](const TimePoint& gLo, const TimePoint& gHi, const std::string& tag)
                {
                    const double gdur = gHi - gLo;
                    if (gdur <= boundaryTol) return;   // 跳过零长度/浮点 1-ulp 级的假间隙
                    for (int k = 1; k < 10; ++k)
                    {
                        const TimePoint t = gLo + gdur * (k / 10.0);
                        EXPECT_GT(absDist(t), threshold * (1.0 - relSlack)) << "pair " << pi << "-" << pj << " threshold " << threshold << " " << tag << " k=" << k;
                    }
                };

                checkGap(window.start(), ivs.front().start(), "gap before first");
                for (size_t i = 0; i + 1 < ivs.size(); ++i)
                    checkGap(ivs[i].stop(), ivs[i + 1].start(), "gap between " + std::to_string(i));
                checkGap(ivs.back().stop(), window.stop(), "gap after last");
            }
        }
    }
}

// 给定时刻到赤道面的有符号距离：极地圆轨道 z = a·sinν，d = z。
TEST(OrbitGeometry, PlaneDistance)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, 0.0};   // 极地圆轨道，trueA=0
    OrbElem other{a, 0.0, 0.0, 0.0, 0.0, 0.0};       // 赤道面

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;

    double dist = 0.0;
    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch, dist), eNoError);
    EXPECT_NEAR(dist, 0.0, 1e-6);                              // ν=0 → z=0

    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch + T / 4.0, dist), eNoError);
    EXPECT_NEAR(dist, a, 1e-6);                                // ν=π/2 → z=a

    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch + T / 2.0, dist), eNoError);
    EXPECT_NEAR(dist, 0.0, 1e-6);                              // ν=π → z=0

    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch + 3.0 * T / 4.0, dist), eNoError);
    EXPECT_NEAR(dist, -a, 1e-6);                               // ν=3π/2 → z=-a（负侧）
}

// 非零 trueA：历元时刻即位于 ν=π/2（z=a），推进 T/4 后到 ν=π（z=0）。
TEST(OrbitGeometry, PlaneDistanceNonzeroTrueAnomaly)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, kHalfPI};   // trueA = π/2
    OrbElem other{a, 0.0, 0.0, 0.0, 0.0, 0.0};

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;

    double dist = 0.0;
    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch, dist), eNoError);
    EXPECT_NEAR(dist, a, 1e-6);                                // ν=π/2 → z=a

    EXPECT_EQ(aOrbitPlaneDistance(epoch, orbit, other, kEarthGrav, epoch + T / 4.0, dist), eNoError);
    EXPECT_NEAR(dist, 0.0, 1e-6);                              // ν=π → z=0
}

// 极地圆轨道（P=(1,0,0)、Q=(0,0,1)）位置 r = a·(cosν, 0, sinν)。
TEST(OrbitGeometry, OrbitPosition)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    const double a = 7000e3;
    OrbElem orbit{a, 0.0, kHalfPI, 0.0, 0.0, 0.0};

    const double n = std::sqrt(kEarthGrav / (a * a * a));
    const double T = kTwoPI / n;

    Vector3d pos;
    aOrbitPosition(epoch, orbit, kEarthGrav, epoch, pos);                     // ν=0
    EXPECT_NEAR(pos.x(), a, 1e-6);
    EXPECT_NEAR(pos.y(), 0.0, 1e-6);
    EXPECT_NEAR(pos.z(), 0.0, 1e-6);

    aOrbitPosition(epoch, orbit, kEarthGrav, epoch + T / 4.0, pos);           // ν=π/2
    EXPECT_NEAR(pos.x(), 0.0, 1e-6);
    EXPECT_NEAR(pos.y(), 0.0, 1e-6);
    EXPECT_NEAR(pos.z(), a, 1e-6);

    aOrbitPosition(epoch, orbit, kEarthGrav, epoch + T / 2.0, pos);           // ν=π
    EXPECT_NEAR(pos.x(), -a, 1e-6);
    EXPECT_NEAR(pos.y(), 0.0, 1e-6);
    EXPECT_NEAR(pos.z(), 0.0, 1e-6);

    aOrbitPosition(epoch, orbit, kEarthGrav, epoch + 3.0 * T / 4.0, pos);     // ν=3π/2
    EXPECT_NEAR(pos.x(), 0.0, 1e-6);
    EXPECT_NEAR(pos.y(), 0.0, 1e-6);
    EXPECT_NEAR(pos.z(), -a, 1e-6);
}

// aOrbitPosition2（根数→笛卡尔状态→通用变量二体传播）与
// aOrbitPosition3（真近点角推进→根数转笛卡尔）作为两条独立路径，
// 共同验证 aOrbitPosition（平近点角线性推进 + 半径公式）。
TEST(OrbitGeometry, OrbitPositionMatchesPropagation)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0);
    OrbElem orbit{8000e3, 0.4, 0.6, 0.8, 1.2, 0.7};   // 一般椭圆倾斜轨道

    const double n = std::sqrt(kEarthGrav / (orbit.a_ * orbit.a_ * orbit.a_));
    const double T = kTwoPI / n;

    // 覆盖一整圈的采样点（含跨圈），逐点对比三种实现
    const double frac[] = {0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0};
    for (double f : frac)
    {
        const TimePoint t = epoch + f * T;
        Vector3d posRef, pos2, pos3;
        aOrbitPosition(epoch, orbit, kEarthGrav, t, posRef);
        aOrbitPosition2(epoch, orbit, kEarthGrav, t, pos2);
        aOrbitPosition3(epoch, orbit, kEarthGrav, t, pos3);

        EXPECT_NEAR((posRef - pos2).norm() / pos2.norm(), 0.0, 1e-9) << "pos2 t/T = " << f;
        EXPECT_NEAR((posRef - pos3).norm() / pos3.norm(), 0.0, 1e-9) << "pos3 t/T = " << f;
    }
}

GTEST_MAIN()
