///
/// @file      testTimeInterval.cpp
/// @brief     时间区间测试
/// @details   测试TimeInterval类的创建、格式化和离散化功能
/// @author    axel
/// @date      2026-04-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见： 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ast/TimeInterval.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Interval.hpp"
#include "ast/RunTime.hpp"
#include "ast/DateTime.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(TimeInterval, Construction)
{
    // 默认构造函数
    {
        TimeInterval interval;
        EXPECT_EQ(interval.duration(), 0.0);
    }
    
    // 构造函数：两个时间点
    {
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 1, 0, 0.0);
        TimeInterval interval(start, stop);
        EXPECT_EQ(interval.duration(), 3600.0);
    }
    
    // 构造函数：基准时间点 + 相对时间
    {
        TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimeInterval interval(epoch, 0.0, 3600.0);
        EXPECT_EQ(interval.duration(), 3600.0);
    }
}

TEST(TimeInterval, GetSet)
{
    TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
    TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 2, 0, 0.0);
    TimeInterval interval(start, stop);
    
    // 测试 getStart/getStop
    EXPECT_EQ(interval.getStart().integerPart(), start.integerPart());
    EXPECT_EQ(interval.getStop().integerPart(), stop.integerPart());
    EXPECT_EQ(interval.duration(), 7200.0);
    
    // 测试 toString
    std::string str = interval.toString();
    EXPECT_FALSE(str.empty());
}

TEST(TimeInterval, Format)
{
    TimePoint start = TimePoint::FromUTC(2026, 1, 1, 12, 30, 45.5);
    TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 14, 30, 45.5);
    TimeInterval interval(start, stop);
    
    std::string strStart, strStop;
    errc_t rc = aTimeIntervalFormat(interval, strStart, strStop);
    
    EXPECT_EQ(rc, eNoError);
    EXPECT_FALSE(strStart.empty());
    EXPECT_FALSE(strStop.empty());
}

TEST(TimeInterval, Parse)
{
    // 先格式化再解析，验证往返
    TimePoint start = TimePoint::FromUTC(2026, 6, 15, 8, 0, 0.0);
    TimePoint stop = TimePoint::FromUTC(2026, 6, 15, 18, 0, 0.0);
    TimeInterval original(start, stop);
    
    std::string strStart, strStop;
    aTimeIntervalFormat(original, strStart, strStop);
    
    TimeInterval parsed;
    errc_t rc = aTimeIntervalParse(strStart, strStop, parsed);
    
    EXPECT_EQ(rc, eNoError);
    EXPECT_NEAR(parsed.duration(), original.duration(), 1e-9);
}

TEST(TimeInterval, Discrete)
{
    // 测试离散化为相对时间（相对于某个epoch）
    // 时长正好是步长整数倍的情况
    {
        TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 1, 0, 0.0);
        TimeInterval interval(start, stop);

        std::vector<double> times;
        errc_t rc = interval.discrete(epoch, 1800.0, times);  // 30分钟步长

        // nnodes = ceil(3600/1800) + 1 = 3
        // 输出 3 个节点: 起点, 中点, 终点
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(times.size(), 3u);
        EXPECT_NEAR(times[0], 0.0, 1e-9);
        EXPECT_NEAR(times[1], 1800.0, 1e-9);
        EXPECT_NEAR(times[2], 3600.0, 1e-9);
    }

    // 测试时长不是步长整数倍的情况
    {
        TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 1, 30, 0.0);  // 1.5小时
        TimeInterval interval(start, stop);

        std::vector<double> times;
        errc_t rc = interval.discrete(epoch, 1800.0, times);  // 30分钟步长

        // nnodes = ceil(5400/1800) + 1 = 4
        // 输出 4 个节点：0, 1800, 3600, 5400
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(times.size(), 4u);
        EXPECT_NEAR(times[0], 0.0, 1e-9);
        EXPECT_NEAR(times[1], 1800.0, 1e-9);
        EXPECT_NEAR(times[2], 3600.0, 1e-9);
        EXPECT_NEAR(times[3], 5400.0, 1e-9);
    }

    // 测试离散化为绝对时间点
    {
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 0, 30, 0.0);
        TimeInterval interval(start, stop);

        std::vector<TimePoint> times;
        errc_t rc = interval.discrete(600.0, times);  // 10分钟步长

        // nnodes = ceil(1800/600) + 1 = 4
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(times.size(), 4u);
    }

    // 测试单步长覆盖整个区间
    {
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 0, 10, 0.0);
        TimeInterval interval(start, stop);

        std::vector<TimePoint> times;
        errc_t rc = interval.discrete(600.0, times);  // 10分钟步长，正好等于区间

        // nnodes = ceil(600/600) + 1 = 2
        // 输出 2 个节点：起点 和 终点
        EXPECT_EQ(rc, eNoError);
        EXPECT_EQ(times.size(), 2u);
    }
}

TEST(TimeInterval, Infinite)
{
    // 测试无限时间区间
    {
        TimeInterval interval;
        interval.setInfinite();
        
        EXPECT_TRUE(std::isinf(interval.duration()));
        EXPECT_TRUE(interval.duration() > 0);
    }
}

TEST(TimeInterval, Iterator)
{
    // 测试迭代器
    {
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 1, 0, 0.0);
        TimeInterval interval(start, stop);
        
        auto range = interval.discrete(1800.0);  // 30分钟步长
        size_t count = 0;
        for(auto it = range.begin(); it != range.end(); ++it)
        {
            count++;
        }
        // nnodes = ceil(3600/1800) + 1 = 3
        EXPECT_EQ(count, 3u);
    }

    // 测试相对时间的迭代器
    {
        TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint start = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
        TimePoint stop = TimePoint::FromUTC(2026, 1, 1, 1, 0, 0.0);
        TimeInterval interval(start, stop);

        auto range = interval.discrete(epoch, 1800.0);
        size_t count = 0;
        for(auto it = range.begin(); it != range.end(); ++it)
        {
            count++;
        }
        EXPECT_EQ(count, 3u);
    }
}

TEST(TimeInterval, ToInterval)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);

    // 同一历元：直接还原相对偏移
    {
        TimeInterval interval(epoch, 100.0, 3600.0);
        Interval rel = interval.toInterval(epoch);
        EXPECT_DOUBLE_EQ(rel.start_, 100.0);
        EXPECT_DOUBLE_EQ(rel.stop_, 3600.0);
        EXPECT_DOUBLE_EQ(rel.duration(), 3500.0);
    }

    // 不同历元：真实减法语义（相对历元2）
    {
        TimeInterval interval(epoch, 100.0, 3600.0);
        TimePoint epoch2 = epoch.shiftedBySecond(50.0);
        Interval rel = interval.toInterval(epoch2);
        EXPECT_DOUBLE_EQ(rel.start_, 50.0);    // 100 - 50
        EXPECT_DOUBLE_EQ(rel.stop_, 3550.0);   // 3600 - 50
    }

    // 往返：toInterval 与 setBounds(epoch, interval) 互为逆
    {
        Interval src{100.0, 3600.0};
        TimeInterval interval;
        interval.setBounds(epoch, src);
        Interval rel = interval.toInterval(epoch);
        EXPECT_DOUBLE_EQ(rel.start_, src.start_);
        EXPECT_DOUBLE_EQ(rel.stop_, src.stop_);
    }

    // 方向保持
    {
        TimePoint epochA = epoch.shiftedBySecond(200.0);
        TimePoint epochB = epoch.shiftedBySecond(50.0);
        TimeInterval interval(epochA, epochB);  // start > stop
        Interval rel = interval.toInterval(epoch);
        EXPECT_TRUE(rel.start_ > rel.stop_);
    }
}

TEST(TimeInterval, ToIntervalInfinite)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
    TimeInterval interval;
    interval.setInfinite();

    Interval rel = interval.toInterval(epoch);
    EXPECT_TRUE(std::isinf(rel.start_));
    EXPECT_TRUE(std::isinf(rel.stop_));
    EXPECT_TRUE(rel.start_ < 0);     // 负无穷
    EXPECT_TRUE(rel.stop_  > 0);     // 正无穷
    EXPECT_TRUE(std::isinf(rel.duration()));
}

TEST(TimeInterval, ToIntervalZero)
{
    TimePoint epoch = TimePoint::FromUTC(2026, 1, 1, 0, 0, 0.0);
    TimeInterval interval;
    interval.setZero();

    Interval rel = interval.toInterval(epoch);
    // 零时长：start == stop，且等于该绝对时刻相对 epoch 的秒偏移
    double off = interval.start().durationFrom(epoch);
    EXPECT_DOUBLE_EQ(rel.start_, off);
    EXPECT_DOUBLE_EQ(rel.stop_, off);
    EXPECT_DOUBLE_EQ(rel.duration(), 0.0);
}

GTEST_MAIN()
