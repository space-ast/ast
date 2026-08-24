///
/// @file      testCloseApproachFilter.cpp
/// @brief     接近分析预过滤器单元测试
/// @details   分别验证过期、近/远地点、轨道路径三级预过滤器的判定行为。
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
#include "ast/TLE.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Constants.h"
#include "ast/CloseApproachOptions.hpp"
#include "ast/CloseApproachFilter.hpp"
#include "ast/CloseApproachGeometry.hpp"
#include "ast/OutOfDateFilter.hpp"
#include "ast/ApogeePerigeeFilter.hpp"
#include "ast/OrbitPathFilter.hpp"

using namespace ast;

namespace
{

/// @brief 用 setter 构造一条 TLE（不经过文本解析）
TLE makeTLE(int id, double incDeg, double raanDeg, double ecc, double argpDeg,
            double meanAnomalyDeg, double revPerDay, const TimePoint& epoch)
{
    TLE tle;
    tle.setNoradId(id);
    tle.setInclination(incDeg * kDegToRad);
    tle.setRightAscenOfNode(raanDeg * kDegToRad);
    tle.setEccentricity(ecc);
    tle.setArgOfPerigee(argpDeg * kDegToRad);
    tle.setMeanAnomaly(meanAnomalyDeg * kDegToRad);
    tle.setMeanMotion(revPerDay * kTwoPI / 86400.0);
    tle.setBstar(0.0);
    tle.setMeanMotionDot(0.0);
    tle.setMotionDotDot(0.0);
    tle.setEpochTime(epoch);
    return tle;
}

/// @brief 构造一个 LEO 参考上下文（inc=98°, raan=0°, e=0.01, ~14.6 rev/day）
CloseApproachContext makeLeoContext(const CloseApproachOptions& opts, const TimePoint& epoch, TLE& ref)
{
    ref = makeTLE(0, 98.0, 0.0, 0.01, 0.0, 0.0, 14.6, epoch);

    CloseApproachContext ctx;
    ctx.ref     = &ref;
    ctx.gm      = kEarthGrav;
    ctx.refElem = aTLEToOrbElem(ref, ctx.gm);
    return ctx;
}

} // namespace

// ==================== 过期预过滤 ====================

TEST(CloseApproachFilterTest, OutOfDate)
{
    const TimePoint epoch = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0.0);
    const TimeInterval window(epoch, epoch + 86400.0);

    CloseApproachOptions opts;
    opts.window           = window;
    opts.outOfDatePad = 30.0;

    OutOfDateFilter filter(opts);

    TLE recent = makeTLE(1, 98.0, 0.0, 0.0, 0.0, 0.0, 14.6, epoch);
    TLE stale  = makeTLE(2, 98.0, 0.0, 0.0, 0.0, 0.0, 14.6, epoch - 100.0 * kSecondsPerDay);

    EXPECT_TRUE(filter.keep(recent));
    EXPECT_FALSE(filter.keep(stale));
}

// ==================== 近/远地点预过滤 ====================

TEST(CloseApproachFilterTest, ApogeePerigee)
{
    const TimePoint epoch = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0.0);

    CloseApproachOptions opts;
    opts.window = TimeInterval(epoch, epoch + 86400.0);

    TLE ref;
    CloseApproachContext ctx = makeLeoContext(opts, epoch, ref);
    ApogeePerigeeFilter filter(opts, ctx);

    TLE leo = makeTLE(1, 98.0, 0.0, 0.01, 0.0, 0.0, 14.6, epoch);  // 高度重叠 → 保留
    TLE geo = makeTLE(2, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, epoch);     // GEO → 剔除

    EXPECT_TRUE(filter.keep(leo));
    EXPECT_FALSE(filter.keep(geo));
}

// ==================== 轨道路径预过滤 ====================

TEST(CloseApproachFilterTest, OrbitPath)
{
    const TimePoint epoch = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0.0);

    CloseApproachOptions opts;
    opts.window = TimeInterval(epoch, epoch + 86400.0);
    opts.enableOrbitPath = true;   // 该过滤器默认关闭，单元测试显式启用

    TLE ref;
    CloseApproachContext ctx = makeLeoContext(opts, epoch, ref);
    OrbitPathFilter filter(opts, ctx);

    TLE crossing = makeTLE(1, 98.0, 3.0, 0.01, 0.0, 0.0, 14.6, epoch); // 路径相交 → 保留
    TLE geo      = makeTLE(2, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, epoch);    // 路径相距 ~35000 km → 剔除

    EXPECT_TRUE(filter.keep(crossing));
    EXPECT_FALSE(filter.keep(geo));
}

TEST(CloseApproachFilterTest, OrbitPathCoplanar)
{
    const TimePoint epoch = TimePoint::FromUTC(2021, 3, 25, 0, 0, 0.0);

    CloseApproachOptions opts;
    opts.window = TimeInterval(epoch, epoch + 86400.0);
    opts.enableOrbitPath = true;   // 该过滤器默认关闭，单元测试显式启用

    TLE ref;
    CloseApproachContext ctx = makeLeoContext(opts, epoch, ref);
    OrbitPathFilter filter(opts, ctx);

    // 与参考对象同平面（inc/raan 相同）→ 公共交线退化，一维径向回退给出的是距离上界，
    // 可能误删真实接近，故过滤器保守保留（含同平面 GEO，虽属假阳性但安全）。
    TLE coplanarNear = makeTLE(3, 98.0, 0.0, 0.01, 0.0, 0.0, 14.6, epoch); // 近同高 → 保留
    TLE coplanarFar  = makeTLE(4, 98.0, 0.0, 0.0, 0.0, 0.0, 1.0, epoch);   // 同平面 GEO → 也保守保留

    EXPECT_TRUE(filter.keep(coplanarNear));
    EXPECT_TRUE(filter.keep(coplanarFar));
}

GTEST_MAIN()
