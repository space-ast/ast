///
/// @file      OrbitGeometry.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-19
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

#include "OrbitGeometry.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TwoBody.hpp"
#include "AstCore/Vector.hpp"
#include "AstCore/TimeIntervalList.hpp"
#include "AstMath/BrentOptimizer.hpp"
#include "AstUtil/Constants.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

// #define AST_DEBUG_ORBIT_GEOMETRY

AST_NAMESPACE_BEGIN

namespace
{

/// @brief 交线退化判据（|L|^2 阈值，对应平面夹角正弦 < 1e-12）
constexpr double kDegenerateLineSq = 1e-24;

/// @brief 共面两轨道最短距离（一维回退：假定两最近点与焦点共线）
/// @details 共面时公共交线退化，改用同一方向下两轨道半径差的最小值近似。
///          预先用 perifocal 基算出近拱线角度差 Δω = atan2(P2·Q1, P2·P1)，
///          同一方向下轨道2的真近点角取 ν−Δω。因 aOrbitRadius 仅依赖 cos ν
///          （椭圆关于近拱线对称），该式对顺行/逆行共面均成立，无需区分。
///          同心圆及近拱对齐情形下精确，其余为保守上界。
double aCoplanarPathMinDistance(const OrbElem& o1, const OrbElem& o2)
{
    Vector3d p1, q1, p2, q2;
    aOrbitPerifocalBasis(o1, p1, q1);
    aOrbitPerifocalBasis(o2, p2, q2);

    // 近拱线角度差：P2 相对 P1 绕 n1 的有符号角
    const double dOmega = std::atan2(p2.dot(q1), p2.dot(p1));

    const auto dist2 = [&](double nu) {
        // 同一方向下轨道2的真近点角（cos 偶函数，顺/逆行均取 ν−Δω）
        const double nu2 = nu - dOmega;
        const double r1  = aOrbitRadius(o1.a_, o1.e_, nu);
        const double r2  = aOrbitRadius(o2.a_, o2.e_, nu2);
        const double d   = r1 - r2;
        return d * d;
    };

    const int    n    = 12;
    const double step = kTwoPI / n;
    double       best = std::numeric_limits<double>::max();
    double       bestNu = 0.0;
    for (int i = 0; i < n; ++i)
    {
        const double nu = i * step;
        const double d  = dist2(nu);
        if (d < best) { best = d; bestNu = nu; }
    }

    const double gridBest = best;
    aBrentMinimize(dist2, bestNu - step, bestNu + step, bestNu, &best);
    best = std::min(best, gridBest);   // 精化不应劣于网格最小值
    return std::sqrt(best);
}

} // namespace

void aOrbitPlaneNormal(double inc, double raan, Vector3d& normal)
{
    normal = Vector3d{std::sin(inc) * std::sin(raan),
                      -std::sin(inc) * std::cos(raan),
                      std::cos(inc)};
}

double aOrbitRadius(double semiMajorAxis, double eccentricity, double trueAnomaly)
{
    return semiMajorAxis * (1.0 - eccentricity * eccentricity)
         / (1.0 + eccentricity * std::cos(trueAnomaly));
}

void aOrbitPerifocalBasis(const OrbElem& e, Vector3d& p, Vector3d& q)
{
    const double cosInc  = std::cos(e.i_);
    const double sinInc  = std::sin(e.i_);
    const double cosRaan = std::cos(e.raan_);
    const double sinRaan = std::sin(e.raan_);
    const double cosArg  = std::cos(e.argper_);
    const double sinArg  = std::sin(e.argper_);

    p = Vector3d{cosRaan * cosArg - sinRaan * sinArg * cosInc,
                 sinRaan * cosArg + cosRaan * sinArg * cosInc,
                 sinArg * sinInc};
    q = Vector3d{-cosRaan * sinArg - sinRaan * cosArg * cosInc,
                 -sinRaan * sinArg + cosRaan * cosArg * cosInc,
                 cosArg * sinInc};
}

errc_t aOrbitNodeLineDistances(const OrbElem& orb1, const OrbElem& orb2,
                               double& distPlus, double& distMinus)
{
    Vector3d n1, n2;
    aOrbitPlaneNormal(orb1.i_, orb1.raan_, n1);
    aOrbitPlaneNormal(orb2.i_, orb2.raan_, n2);

    const Vector3d line = n1.cross(n2);
    if (line.squaredNorm() < kDegenerateLineSq) { return eErrorInvalidParam; } // 近共面：交线退化

    const Vector3d lineHat = line.normalized();

    // 每条轨道沿交线方向的交点（以 lineHat 为正向的有符号坐标）：
    //   s_plus  = +r(ν)，s_minus = -r(ν+π)
    Vector3d p, q;
    double   s1Plus, s1Minus, s2Plus, s2Minus;

    aOrbitPerifocalBasis(orb1, p, q);
    double nu = std::atan2(q.dot(lineHat), p.dot(lineHat));
    s1Plus  =  aOrbitRadius(orb1.a_, orb1.e_, nu);
    s1Minus = -aOrbitRadius(orb1.a_, orb1.e_, nu + kPI);

    aOrbitPerifocalBasis(orb2, p, q);
    nu      = std::atan2(q.dot(lineHat), p.dot(lineHat));
    s2Plus  =  aOrbitRadius(orb2.a_, orb2.e_, nu);
    s2Minus = -aOrbitRadius(orb2.a_, orb2.e_, nu + kPI);

    distPlus  = std::fabs(s1Plus  - s2Plus);
    distMinus = std::fabs(s1Minus - s2Minus);
    return eNoError;
}

errc_t aOrbitPathMinDistance(const OrbElem& orb1, const OrbElem& orb2, double& dist)
{
    double distPlus  = 0.0;
    double distMinus = 0.0;
    const errc_t err = aOrbitNodeLineDistances(orb1, orb2, distPlus, distMinus);
    if (err != eNoError)
    {
        // 共面/近共面回退：交线退化，改用一维共线假设求最小距离。
        dist = aCoplanarPathMinDistance(orb1, orb2);
        return eNoError;
    }

    dist = std::min(distPlus, distMinus);
    return eNoError;
}

errc_t aOrbitPlaneProximityIntervals(
    const TimePoint& epoch,
    const OrbElem& orbit,
    const OrbElem& other,
    double gm,
    double threshold,
    const TimeInterval& window,
    TimeIntervalList& result)
{
    // —— 参数校验 ——
    if (!(threshold > 0.0)) return eErrorInvalidParam;
    if (!(gm > 0.0))        return eErrorInvalidParam;
    if (!(orbit.a_ > 0.0))  return eErrorInvalidParam;
    if (orbit.e_ < 0.0 || orbit.e_ >= 1.0 - 1e-12) return eErrorInvalidParam;

    const double winDur = window.duration();
    if (!std::isfinite(winDur)) return eErrorInvalidParam;   // 无限窗口无法枚举
    if (window.isEmpty()) { result = TimeIntervalList(epoch); return eNoError; }   // 空/反向/NaN 窗口无物可扫描；点窗口视为有效交由后续几何判别

    // —— 轨道面几何量 ——
    Vector3d p, q;
    aOrbitPerifocalBasis(orbit, p, q);
    Vector3d n2;
    aOrbitPlaneNormal(other.i_, other.raan_, n2);

    const double alpha = p.dot(n2);
    const double beta  = q.dot(n2);

    // 共面退化：d ≡ 0，整个窗口均在阈值内
    if (alpha * alpha + beta * beta < kDegenerateLineSq)
    {
        result = TimeIntervalList::FromTimeInterval(window);
        return eNoError;
    }

    const double pparam = orbit.a_ * (1.0 - orbit.e_ * orbit.e_);              // 半通径
    const double n      = std::sqrt(gm / (orbit.a_ * orbit.a_ * orbit.a_));     // 平均角速度
    const double T      = kTwoPI / n;                                           // 周期

    // 真近点角 → 过近点后时间。aTrueToMean 返回主值 (-π, π]，负主值需 +2π 包回 [0, 2π)；
    // nu 可跨多圈（≥ 2π），先按整圈数分离主值再叠加 k·T，保证时间随 nu 单调递增。
    auto timeSincePerigee = [&](double nu)
    {
        const double turns = std::floor(nu / kTwoPI);
        const double nu0   = nu - turns * kTwoPI;   // nu0 ∈ [0, 2π)
        double M = aTrueToMean(nu0, orbit.e_);
        if (M < 0.0) M += kTwoPI;
        return (M + turns * kTwoPI) / n;
    };

    // 过近点时刻：epoch 对应真近点角 trueA_
    const TimePoint tp = epoch - timeSincePerigee(orbit.trueA_);

    // 有符号面距 d(ν) = r(ν)·(α cosν + β sinν)
    auto dist = [&](double nu)
    {
        return aOrbitRadius(orbit.a_, orbit.e_, nu) * (alpha * std::cos(nu) + beta * std::sin(nu));
    };

    // —— 求 |d(ν)| = threshold 的真近点角边界点（解析求根，零迭代）——
    // 有符号面距 d(ν) = r(ν)·(α cosν + β sinν)，其中 r = p/(1+e·cosν)、p = a(1-e²) 为半通径。
    // 令 d(ν) = σ（σ 取 ±threshold），交叉相乘消去分母，得关于 ν 的线性三角方程：
    //     p·(α cosν + β sinν) = σ·(1 + e·cosν)
    //   ⇒ (p·α - σ·e)·cosν + (p·β)·sinν = σ
    // 记 C = p·α - σ·e、S = p·β，上式即 C·cosν + S·sinν = σ。
    // 用辅助角合并：C·cosν + S·sinν = R·cos(ν - ψ)，其中 R = √(C²+S²)、ψ = atan2(S, C)，
    // 方程化为 cos(ν - ψ) = σ/R。
    //   · 若 R < |σ|：该瓣幅值仍达不到 σ，方程无解 → 无边界点；
    //   · 否则 ν = ψ ± acos(σ/R)，即真近点角上 |d| = σ 的两个边界点。
    std::vector<double> bounds;
    auto addRoots = [&](double sigma)
    {
        const double C = pparam * alpha - sigma * orbit.e_;   // (p·α - σ·e)
        const double S = pparam * beta;                       // (p·β)
        const double R = std::sqrt(C * C + S * S);            // 辅助幅值 = C·cosν + S·sinν 的峰值
        if (R < std::fabs(sigma)) return;                     // 峰值达不到 σ，无解

        const double psi = std::atan2(S, C);                  // 辅助角：C·cosν + S·sinν = R·cos(ν - ψ)
        const double da  = acosSafe(sigma / R);               // acos(σ/R)，acosSafe 自动钳位防浮点越界
        double nu1 = std::fmod(psi + da, kTwoPI);             // 根 1：ν = ψ + da
        double nu2 = std::fmod(psi - da, kTwoPI);             // 根 2：ν = ψ - da
        if (nu1 < 0.0) nu1 += kTwoPI;                         // 包回 [0, 2π)
        if (nu2 < 0.0) nu2 += kTwoPI;
        bounds.push_back(nu1);
        bounds.push_back(nu2);
    };
    addRoots( threshold);   // σ = +threshold：解 d(ν) = +threshold 的边界
    addRoots(-threshold);   // σ = -threshold：解 d(ν) = -threshold 的边界
    // 阈值恰等于 d(ν) 局部极值（峰值/谷值）时，acos(σ/R)=0 或 π，ψ±da 两根本重合（重根），
    // 同一边界会被 push 两次；排序去重后边界点才严格唯一，否则后续分段判别会产生零长度/重复区间。
    std::sort(bounds.begin(), bounds.end());
    bounds.erase(std::unique(bounds.begin(), bounds.end()), bounds.end());

    // —— 分段判别并转换为相对过近点的时间区间 [0, T) ——
    std::vector<std::pair<double, double>> revIntervals;
    auto pushNuInterval = [&](double nuLo, double nuHi)
    {
        // nuLo ∈ [0, 2π)；nuHi 可为 2π（整圈/尾段闭合）或 > 2π（跨周期尾段）。
        // timeSincePerigee 已正确处理 2π→T 及跨周期的单调递增时间。
        const double tLo = timeSincePerigee(nuLo);
        const double tHi = timeSincePerigee(nuHi);
        if (tHi > tLo) revIntervals.emplace_back(tLo, tHi);
    };

    const size_t m = bounds.size();
    if (m == 0)
    {
        // 阈值覆盖整圈（|d| ≤ threshold 在整圈恒成立），整个窗口都在区间内。
        // 直接返回窗口，避免跨圈复制时 T+kT 与 (k+1)T 的浮点 1-ulp 误差在周期边界产生假 gap。
        result = TimeIntervalList::FromTimeInterval(window);
        return eNoError;
    }
    else
    {
        for (size_t i = 0; i + 1 < m; ++i)
        {
            const double lo = bounds[i], hi = bounds[i + 1];
            if (std::fabs(dist(0.5 * (lo + hi))) <= threshold) pushNuInterval(lo, hi);
        }
        // 跨 0/2π 的尾段 [bounds[m-1], bounds[0] + 2π)。
        // 注意：合并为单个跨周期区间 push，而非拆成 [lo,2π] + [0,bounds[0]] 两段，
        // 否则跨圈复制时 T+kT 与 (k+1)T 的浮点 1-ulp 差异会让相邻区间在周期边界处无法合并，
        // 产生落在负峰值(|d|<threshold)处的假 gap。
        const double lo = bounds[m - 1];
        const double hi = bounds[0] + kTwoPI;
        double nuMid = 0.5 * (lo + hi);
        if (nuMid >= kTwoPI) nuMid -= kTwoPI;
        if (std::fabs(dist(nuMid)) <= threshold)
        {
            pushNuInterval(lo, hi);   // 跨周期区间 [lo, bounds[0]+2π]
        }
    }

    // —— 跨圈复制并裁剪到窗口 ——
    const double winStart = window.start() - tp;
    const double winStop  = window.stop()  - tp;

    std::vector<TimeInterval> out;
    const long long kMin = static_cast<long long>(std::floor(winStart / T)) - 1;
    const long long kMax = static_cast<long long>(std::ceil (winStop  / T)) + 1;
    for (long long k = kMin; k <= kMax; ++k)
    {
        const double shift = static_cast<double>(k) * T;
        for (const auto& iv : revIntervals)
        {
            const double s = std::max(iv.first  + shift, winStart);
            const double e = std::min(iv.second + shift, winStop);
            if (s <= e) out.emplace_back(tp + s, tp + e);
        }
    }

    result = TimeIntervalList::FromTimeIntervals(out, epoch);
    result.mergeInPlace();
    return eNoError;
}

double aOrbitTrueAnomaly(
    const TimePoint& epoch,
    const OrbElem& orbit,
    double gm,
    const TimePoint& time)
{
    // 由 epoch 处平近点角 + 平均角速度线性推进，得查询时刻真近点角
    const double n  = std::sqrt(gm / (orbit.a_ * orbit.a_ * orbit.a_));
    const double M  = aTrueToMean(orbit.trueA_, orbit.e_) + n * (time - epoch);
    return aMeanToTrue(M, orbit.e_);
}

void aOrbitPosition(
    const TimePoint& epoch,
    const OrbElem& orbit,
    double gm,
    const TimePoint& time,
    Vector3d& position)
{
    const double nu = aOrbitTrueAnomaly(epoch, orbit, gm, time);

    Vector3d p, q;
    aOrbitPerifocalBasis(orbit, p, q);

    // 位置矢量 r = r(ν)·(cosν·P + sinν·Q)
    const double r = aOrbitRadius(orbit.a_, orbit.e_, nu);
    position = p * (r * std::cos(nu)) + q * (r * std::sin(nu));
}

void aOrbitPosition2(const TimePoint &epoch, const OrbElem &orbit, double gm, const TimePoint &time, Vector3d &position)
{
    Vector3d velocity;
    aOrbElemToCart(orbit, gm, position, velocity);
    aTwoBodyProp(time - epoch, gm, position, velocity);
}

void aOrbitPosition3(const TimePoint &epoch, const OrbElem &orbit, double gm, const TimePoint &time, Vector3d &position)
{
    OrbElem newOrbit = orbit;
    newOrbit.trueA_ = aOrbitTrueAnomaly(epoch, orbit, gm, time);
    Vector3d velocity;
    aOrbElemToCart(newOrbit, gm, position, velocity);
}

errc_t aOrbitPlaneDistance(
    const TimePoint& epoch,
    const OrbElem& orbit,
    const OrbElem& other,
    double gm,
    const TimePoint& time,
    double& dist)
{
    if (!(gm > 0.0))       return eErrorInvalidParam;
    if (!(orbit.a_ > 0.0)) return eErrorInvalidParam;
    if (orbit.e_ < 0.0 || orbit.e_ >= 1.0 - 1e-12) return eErrorInvalidParam;

    Vector3d n2;
    aOrbitPlaneNormal(other.i_, other.raan_, n2);

    // 有符号面距 d = r·n2，位置由 aOrbitPosition 给出
    Vector3d position;
    aOrbitPosition(epoch, orbit, gm, time, position);
    dist = position.dot(n2);
    return eNoError;
}

AST_NAMESPACE_END
