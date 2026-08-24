///
/// @file      CloseApproachAnalyzer.cpp
/// @brief     接近分析编排器实现
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

#include "CloseApproachAnalyzer.hpp"
#include "CloseApproachGeometry.hpp"
#include "AstCore/OutOfDateFilter.hpp"
#include "AstCore/ApogeePerigeeFilter.hpp"
#include "AstCore/OrbitPathFilter.hpp"
#include "AstCore/DistanceConstraint.hpp"
#include "AstCore/AccessEvaluator.hpp"
#include "AstCore/FixedStepStepper.hpp"
#include "AstCore/TimeIntervalList.hpp"
#include "AstCore/SGP4.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/BrentOptimizer.hpp"
#include "AstUtil/Constants.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>

AST_NAMESPACE_BEGIN

// #define AST_DEBUG_CLOSE_APPROACH

namespace
{

using Clock = std::chrono::steady_clock;

/// @brief 计算自 t0 起的经过秒数
inline double elapsedSeconds(Clock::time_point t0)
{
    return std::chrono::duration<double>(Clock::now() - t0).count();
}

/// @brief 应用单个过滤器，返回保留的候选列表
std::vector<TLE> applyFilter(const CloseApproachFilter& filter, const std::vector<TLE>& in)
{
    std::vector<TLE> out;
    out.reserve(in.size());
    for (const auto& tle : in)
    {
        if (filter.keep(tle)) { out.push_back(tle); }
    }
    return out;
}

/// @brief 在单个接近时间窗内精化最近点（TCA）与最小距离
bool refineMinDistance(const Point& ref, const Point& cand,
                       const TimeInterval& interval, CloseApproachResult& out)
{
    const double dur = interval.duration();
    if (dur <= 0.0) { return false; }

    // 细采样定位最小距离样本，构造单峰括号
    const int    nSamples = 32;
    int          bestIdx  = 0;
    double       bestDist = std::numeric_limits<double>::max();
    for (int i = 0; i <= nSamples; ++i)
    {
        const double x = dur * i / nSamples;
        Vector3d p1, p2;
        ref.getPos(interval.start() + x, p1);
        cand.getPos(interval.start() + x, p2);
        const double d = (p1 - p2).norm();
        if (d < bestDist) { bestDist = d; bestIdx = i; }
    }

    const int lo = std::max(0, bestIdx - 1);
    const int hi = std::min(nSamples, bestIdx + 1);

    const double xlo = dur * lo / nSamples;
    const double xhi = dur * hi / nSamples;

    double xMin    = dur * bestIdx / nSamples;
    double minDist = bestDist;
    aBrentMinimize(
        [&](double x) {
            Vector3d p1, p2;
            ref.getPos(interval.start() + x, p1);
            cand.getPos(interval.start() + x, p2);
            return (p1 - p2).norm();
        },
        xlo, xhi, xMin, &minDist);

    const TimePoint tca = interval.start() + xMin;

    Vector3d p1, p2, v1, v2;
    ref.getPosVel(tca, p1, v1);
    cand.getPosVel(tca, p2, v2);

    out.tca         = tca;
    out.minDistance = minDist;
    out.relVelocity = (v1 - v2).norm();
    return true;
}

/// @brief 求单个时间区间内两对象距离的最小值
/// @details 时间过滤产出的区间对应一次潜在接近，近似单峰，直接调用 Brent 法
///          求区间内距离最小值，用作细采样前的门控。
double minDistanceInInterval(const Point& ref, const Point& cand,
                             const TimeInterval& interval)
{
    const double dur = interval.duration();
    if (dur <= 0.0) { return std::numeric_limits<double>::max(); }

    double minX    = 0.0;
    double minDist = std::numeric_limits<double>::max();
    aBrentMinimize(
        [&](double x) {
            Vector3d p1, p2;
            ref.getPos(interval.start() + x, p1);
            cand.getPos(interval.start() + x, p2);
            return (p1 - p2).norm();
        },
        0.0, dur, minX, &minDist);
    return minDist;
}

/// @brief 用星历状态计算两对象相对彼此轨道面的接近时间区间
/// @details 轨道面法向取对象瞬时角动量方向 r×v（含 J2 长期项），与后续
///          暴力采样/精化所用星历严格一致。解析式 aOrbitPlaneProximityIntervals
///          基于冻结开普勒轨道，在数日传播后交点过境时刻因 J2 长期进动漂移数分钟，
///          导致真实接近被误删（假阴性），故时间过滤改用本星历版本。
///
///          单次粗采样同时得到两个有符号面距：
///            d1 = r_ref · n̂_cand  —— 参考目标相对候选轨道面的距离
///            d2 = r_cand · n̂_ref  —— 候选目标相对参考轨道面的距离
///          检测各自过零（交点过境）区间，并以 margin 半宽外扩，覆盖 |d|≤pad 的
///          接近窗口（交点过境与最近接近时刻相差通常不足数秒）。若全程 |d|≤pad
///          （近共面退化），则返回整个窗口。
void planeProximityIntervalsBoth(
    const Point& ref, const Point& cand, const TimeInterval& window,
    double coarseStep, double pad, double margin,
    TimeIntervalList& out1, TimeIntervalList& out2)
{
    out1 = TimeIntervalList(window.start());
    out2 = TimeIntervalList(window.start());

    const double dur = window.duration();
    if (dur <= 0.0 || coarseStep <= 0.0) { return; }

    const TimePoint w0 = window.start();
    const int n = static_cast<int>(std::ceil(dur / coarseStep));

    auto bothDist = [&](const TimePoint& t, double& d1, double& d2)
    {
        Vector3d rr, vr, rc, vc;
        ref.getPosVel(t, rr, vr);
        cand.getPosVel(t, rc, vc);
        const double nnr = rr.cross(vr).norm();
        const double nnc = rc.cross(vc).norm();
        d1 = nnc > 0.0 ? rr.dot(rc.cross(vc)) / nnc : 0.0;
        d2 = nnr > 0.0 ? rc.dot(rr.cross(vr)) / nnr : 0.0;
    };

    bool coplanar1 = true;
    bool coplanar2 = true;

    double prevD1 = 0.0, prevD2 = 0.0;
    bothDist(w0, prevD1, prevD2);
    if (std::fabs(prevD1) > pad) { coplanar1 = false; }
    if (std::fabs(prevD2) > pad) { coplanar2 = false; }

    for (int i = 1; i <= n; ++i)
    {
        const double x = std::min(coarseStep * i, dur);
        double curD1 = 0.0, curD2 = 0.0;
        bothDist(w0 + x, curD1, curD2);
        if (std::fabs(curD1) > pad) { coplanar1 = false; }
        if (std::fabs(curD2) > pad) { coplanar2 = false; }

        const double prevOff = coarseStep * (i - 1);
        if (prevD1 * curD1 < 0.0)
        {
            const double s = std::max(0.0, prevOff - margin);
            const double e = std::min(dur,  x + margin);
            if (s < e) { out1.push_back(w0 + s, w0 + e); }
        }
        if (prevD2 * curD2 < 0.0)
        {
            const double s = std::max(0.0, prevOff - margin);
            const double e = std::min(dur,  x + margin);
            if (s < e) { out2.push_back(w0 + s, w0 + e); }
        }
        prevD1 = curD1;
        prevD2 = curD2;
    }

    if (coplanar1) { out1 = TimeIntervalList::FromTimeInterval(window); }
    else           { out1.mergeInPlace(); }
    if (coplanar2) { out2 = TimeIntervalList::FromTimeInterval(window); }
    else           { out2.mergeInPlace(); }
}

} // namespace

CloseApproachAnalyzer::CloseApproachAnalyzer(const CloseApproachOptions& opts)
    : opts_(opts)
{
}

errc_t CloseApproachAnalyzer::analyze(const TLE& reference, const TLEDatabase& catalog,
                                      CloseApproachReport& report)
{
    return analyze(reference, catalog.getEntries(), report);
}

errc_t CloseApproachAnalyzer::analyze(const TLE& reference, const std::vector<TLE>& candidates,
                                      CloseApproachReport& report)
{
    CloseApproachReport result;
    result.totalCatalog = candidates.size();

    const double gm = (opts_.gm > 0.0) ? opts_.gm : kEarthGrav;

    CloseApproachContext ctx;
    ctx.ref      = &reference;
    ctx.gm       = gm;
    ctx.refElem  = aTLEToOrbElem(reference, gm);

    // ---- 预过滤级联 ----
    std::vector<TLE> pass = candidates;

    // 过滤掉与参考对象相同 NORAD 编号的候选（避免与自身比较）
    const int refId = reference.noradId();
    pass.erase(std::remove_if(pass.begin(), pass.end(),
                              [refId](const TLE& tle) { return tle.noradId() == refId; }),
               pass.end());

    Clock::time_point t0 = Clock::now();
    if (opts_.enableOutOfDate)
    {
        OutOfDateFilter filter(opts_);
        pass = applyFilter(filter, pass);
    }
    result.afterOutOfDate   = pass.size();
    result.secondsOutOfDate = elapsedSeconds(t0);

    Clock::time_point t1 = Clock::now();
    if (opts_.enableApogeePerigee)
    {
        ApogeePerigeeFilter filter(opts_, ctx);
        pass = applyFilter(filter, pass);
    }
    result.afterApogeePerigee   = pass.size();
    result.secondsApogeePerigee = elapsedSeconds(t1);

    Clock::time_point t2 = Clock::now();
    if (opts_.enableOrbitPath)
    {
        OrbitPathFilter filter(opts_, ctx);
        pass = applyFilter(filter, pass);
    }
    result.afterOrbitPath   = pass.size();
    result.secondsOrbitPath = elapsedSeconds(t2);

    // ---- 最终检测 + 精化（含基于粗扫描的时间过滤）----
    Clock::time_point t4 = Clock::now();
    SGP4             refEph(reference);


    size_t  afterTimeFilter = 0;
    double  coarseAcc = 0.0;   ///< 时间过滤总耗时（面扫描 + Brent 门控）
    double  scanAcc   = 0.0;   ///< 面扫描（交点过境粗扫描）耗时
    double  fineAcc   = 0.0;
    double  totalSearchDur = 0.0;

    for (const TLE& cand : pass)
    {
        SGP4 candEph(cand);

        // 时间过滤收窄：基于星历状态的交点过境重叠区间。
        // 解析式 aOrbitPlaneProximityIntervals 采用冻结开普勒轨道，数日传播后
        // 交点过境时刻因 J2 长期进动漂移，导致真实接近被误删（假阴性）；此处改用
        // SGP4 状态直接计算轨道面法向（r×v），与后续暴力采样/精化严格一致。
        Clock::time_point tc = Clock::now();
        TimeIntervalList searchIntervals;
        if(opts_.enableTimeFilter)
        {
            const double pad    = opts_.threshold + opts_.planeDistancePad;
            const double step   = opts_.coarseStep;
            const double margin = opts_.searchMargin;

            TimeIntervalList intervals1, intervals2;
            planeProximityIntervalsBoth(
                refEph, candEph, opts_.window, step, pad, margin,
                intervals1, intervals2);

            #ifdef AST_DEBUG_CLOSE_APPROACH
            if(cand.noradId() == 45249)
            {
                auto str = intervals1.toString();
                printf("noradid: %d, intervals1:\n%s\n", cand.noradId(), str.c_str());
                str = intervals2.toString();
                printf("noradid: %d, intervals2:\n%s\n", cand.noradId(), str.c_str());
            }
            #endif

            searchIntervals = intervals1.intersected(intervals2);
        }
        else
        {
            searchIntervals = TimeIntervalList::FromTimeInterval(opts_.window);
        }
        scanAcc += elapsedSeconds(tc);

        if (searchIntervals.empty()) { continue; }
        ++afterTimeFilter;

        #ifdef AST_DEBUG_CLOSE_APPROACH
        if(cand.noradId() == 45249                )
        {
            auto str = searchIntervals.toString();
            printf("noradid: %d, intervals:\n%s\n", cand.noradId(), str.c_str());
        }
        #endif

        const double fineStep =  opts_.sampleStep;
        FixedStepStepper fineStepper(fineStep);

        DistanceConstraint constraint(&refEph, &candEph, opts_.threshold);

        // 时间过滤后：遍历每个时间区间，求区间内的距离最小值，
        // 仅当最小值低于阈值时才细采样求具体的接近窗口。
        for (size_t i = 0; i < searchIntervals.size(); ++i)
        {
            const TimeInterval interval = searchIntervals[i];

            /*
            @todo
            当候选与参考目标共面（i/raan 相同）时，planeProximityIntervalsBoth 使 coplanar1=coplanar2=true，返回整个数天窗口作为单一区间，intersect()
            也得到同样的整窗口。minDistanceInInterval 随后在 [0, 5天] 上运行 aBrentMinimize，而该距离函数有约 80 个局部极小（每圈一个）。Brent/GoldenSection（见
            BrentOptimizer.cpp:61，假设单峰）只收敛到某一个局部极小。若该局部极小 >= 阈值，第 351 行的 `continue` 会跳过 aEvaluateAccess，静默丢弃窗口内其他位置的真实接近。SameOrbitTrailingObject
            测试因距离近似恒定而未能暴露此问题。
            */

            Clock::time_point tg = Clock::now();
            const double minDist = minDistanceInInterval(refEph, candEph, interval);
            coarseAcc += elapsedSeconds(tg);

            if (minDist >= opts_.threshold) { continue; }

            totalSearchDur += interval.duration();

            Clock::time_point tf = Clock::now();
            TimeIntervalList intervals;
            aEvaluateAccess(&constraint, &fineStepper, interval, intervals);

            for (size_t j = 0; j < intervals.size(); ++j)
            {
                CloseApproachResult res;
                if (refineMinDistance(refEph, candEph, intervals[j], res))
                {
                    res.noradId = cand.noradId();
                    res.name    = cand.lines().name();
                    result.results.push_back(std::move(res));
                }
            }
            fineAcc += elapsedSeconds(tf);
        }
    }
    result.afterTimeFilter   = afterTimeFilter;
    result.secondsTimeFilter = scanAcc + coarseAcc;   // 时间过滤总耗时（面扫描 + Brent 门控）
    result.secondsBruteForce = elapsedSeconds(t4);
    result.secondsCoarseScan = scanAcc;               // 仅面扫描（交点过境粗扫描）耗时
    result.secondsFineScan   = fineAcc;
    result.totalSearchSeconds = totalSearchDur;

    // ---- 排序与截断 ----
    std::sort(result.results.begin(), result.results.end(),
              [](const CloseApproachResult& a, const CloseApproachResult& b) {
                  return a.tca < b.tca;
              });

    if (opts_.maxResults > 0 && result.results.size() > opts_.maxResults)
    {
        result.results.resize(opts_.maxResults);
    }

    report = std::move(result);
    return eNoError;
}

AST_NAMESPACE_END
