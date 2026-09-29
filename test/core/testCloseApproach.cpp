///
/// @file      testCloseApproach.cpp
/// @brief     接近分析端到端测试
/// @details   使用已知 TLE（NORAD 26661）及其扰动副本，验证编排器：
///            1. 同轨道尾随目标可被识别（距离小于阈值）
///            2. 平面相交目标的最近距离与暴力密集采样真值一致
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
#include "ast/SGP4.hpp"
#include "ast/TLE.hpp"
#include "ast/TLELines.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Vector.hpp"
#include "ast/Constants.hpp"
#include "ast/CloseApproachAnalyzer.hpp"
#include "ast/CloseApproachOptions.hpp"
#include "ast/CloseApproachResult.hpp"
#include <algorithm>
#include <limits>
#include <vector>

using namespace ast;

namespace
{

static const char* kLine1 = "1 26661U 99057LT  21084.99115140  .00000893  00000-0  18006-3 0  9994";
static const char* kLine2 = "2 26661  98.3488 112.3622 0070611 169.6757 257.7924 14.62494120100130";

/// @brief 解析参考 TLE（NORAD 26661，太阳同步轨道）
TLE makeRefTLE()
{
    TLELines lines;
    lines.line1() = kLine1;
    lines.line2() = kLine2;
    return TLE::FromLines(lines);
}

/// @brief 暴力密集采样求两星历在窗口内的最小距离
double bruteForceMinDistance(const SGP4& ref, const SGP4& cand,
                             const TimePoint& start, double spanSec, double stepSec,
                             TimePoint& tMin)
{
    double minD = std::numeric_limits<double>::max();
    for (double t = 0.0; t <= spanSec; t += stepSec)
    {
        TimePoint tp = start + t;
        Vector3d p1, p2;
        ref.getPos(tp, p1);
        cand.getPos(tp, p2);
        double d = (p1 - p2).norm();
        if (d < minD) { minD = d; tMin = tp; }
    }
    return minD;
}

} // namespace

// ==================== 测试 1：同轨道尾随目标 ====================

TEST(CloseApproachTest, SameOrbitTrailingObject)
{
    TLE ref = makeRefTLE();
    TLE cand = ref;
    cand.setNoradId(90001);
    cand.setMeanAnomaly(ref.meanAnomaly() + 0.2 * kDegToRad);   // 相位偏移 ~0.2°

    CloseApproachOptions opts;
    opts.window    = TimeInterval(ref.epochTime(), ref.epochTime() + 86400.0);
    opts.threshold = 200.0e3;   // 200 km
    opts.sampleStep = 60.0;

    CloseApproachAnalyzer analyzer(opts);
    CloseApproachReport report;
    std::vector<TLE> catalog{ cand };

    ASSERT_EQ(analyzer.analyze(ref, catalog, report), eNoError);

    EXPECT_GE(report.results.size(), 1u);
    for (const auto& r : report.results)
    {
        EXPECT_LT(r.minDistance, opts.threshold);
        EXPECT_EQ(r.noradId, 90001);
    }
}

// ==================== 测试 2：平面相交目标与暴力采样真值对比 ====================

TEST(CloseApproachTest, CrossingMatchesBruteForce)
{
    TLE ref = makeRefTLE();
    TLE cand = ref;
    cand.setNoradId(90002);
    cand.setRightAscenOfNode(ref.rightAscenOfNode() + 3.0 * kDegToRad);  // 平面相交

    CloseApproachOptions opts;
    opts.window    = TimeInterval(ref.epochTime(), ref.epochTime() + 86400.0);
    opts.threshold = 2000.0e3;   // 2000 km，充分覆盖
    opts.sampleStep = 30.0;

    CloseApproachAnalyzer analyzer(opts);
    CloseApproachReport report;
    std::vector<TLE> catalog{ cand };

    ASSERT_EQ(analyzer.analyze(ref, catalog, report), eNoError);
    ASSERT_GE(report.results.size(), 1u);

    double analyzerMin = std::numeric_limits<double>::max();
    for (const auto& r : report.results)
    {
        analyzerMin = std::min(analyzerMin, r.minDistance);
    }

    SGP4 refEph(ref);
    SGP4 candEph(cand);
    TimePoint tMin;
    double bruteMin = bruteForceMinDistance(refEph, candEph, ref.epochTime(), 86400.0, 1.0, tMin);

    // 分析器精化后的最小值应接近暴力采样真值（Brent 精化通常更准，故允许下界更低）
    EXPECT_NEAR(analyzerMin, bruteMin, 10.0e3);
    EXPECT_LE(analyzerMin, bruteMin + 10.0e3);
}

GTEST_MAIN()
