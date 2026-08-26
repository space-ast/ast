///
/// @file      testRange.cpp
/// @brief     DoubleRange / TimePointRange 下标访问测试
/// @details   测试按步长采样范围类型的 operator[] 语义（末点返回 stop_）
/// @author    axel
/// @date      2026-08-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
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

#include "ast/Interval.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(Range, DoubleRangeSubscript)
{
    // 时长正好是步长整数倍：0, 1800, 3600
    {
        Interval interval{0.0, 3600.0};
        auto range = interval.discretize(1800.0);
        ASSERT_EQ(range.size(), 3u);
        EXPECT_NEAR(range[0], 0.0, 1e-9);
        EXPECT_NEAR(range[1], 1800.0, 1e-9);
        EXPECT_NEAR(range[2], 3600.0, 1e-9);
    }

    // 时长不是步长整数倍：0, 1800, 3600, 5400（末点强制并入）
    {
        Interval interval{0.0, 5400.0};
        auto range = interval.discretize(1800.0);
        ASSERT_EQ(range.size(), 4u);
        EXPECT_NEAR(range[0], 0.0, 1e-9);
        EXPECT_NEAR(range[1], 1800.0, 1e-9);
        EXPECT_NEAR(range[2], 3600.0, 1e-9);
        EXPECT_NEAR(range[3], 5400.0, 1e-9);
    }

    // operator[] 与迭代器遍历结果一致
    {
        Interval interval{100.0, 7300.0};
        auto range = interval.discretize(1800.0);
        size_t i = 0;
        for (auto it = range.begin(); it != range.end(); ++it, ++i) {
            EXPECT_NEAR(range[i], *it, 1e-9);
        }
        EXPECT_EQ(i, range.size());
    }
}

TEST(Range, TimePointRangeSubscript)
{
    TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
    TimePoint stop  = TimePoint::FromUTC(2026, 1, 1, 1, 0, 0.0);   // 3600 秒后

    // 时长正好是步长整数倍：start, start+1800, stop
    {
        TimeInterval interval{start, stop};
        auto range = interval.discretize(1800.0);
        ASSERT_EQ(range.size(), 3u);
        EXPECT_EQ(range[0], start);
        EXPECT_EQ(range[1], start + 1800.0);
        EXPECT_EQ(range[2], stop);
    }

    // operator[] 与迭代器遍历结果一致（含非整数倍末点）
    {
        TimeInterval interval{start, start + 5400.0};
        auto range = interval.discretize(1800.0);
        ASSERT_EQ(range.size(), 4u);
        size_t i = 0;
        for (auto it = range.begin(); it != range.end(); ++it, ++i) {
            EXPECT_EQ(range[i], *it);
        }
        EXPECT_EQ(i, range.size());
        EXPECT_EQ(range[3], start + 5400.0);   // 末点强制并入
    }
}

GTEST_MAIN()
