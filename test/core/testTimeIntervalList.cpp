///
/// @file      testTimeIntervalList.cpp
/// @brief     绝对时段列表测试
/// @details   测试TimeIntervalList类的构造、访问、集合运算、跨历元运算和离散化功能
/// @author    axel
/// @date      2026-07-03
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

#include "ast/TimeIntervalList.hpp"
#include "ast/TimeInterval.hpp"
#include "ast/TimePoint.hpp"
#include "ast/TimeList.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE


// ————————————————————————
// 辅助函数
// ————————————————————————

/// @brief 获取测试用参考历元
static TimePoint testEpoch()
{
    return TimePoint::J2000TT();
}

/// @brief 创建测试用 TimeIntervalList（使用测试历元）
static TimeIntervalList makeList()
{
    return TimeIntervalList(testEpoch());
}


// ————————————————————————
// 构造
// ————————————————————————

TEST(TimeIntervalList, DefaultConstruction)
{
    TimeIntervalList list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.totalDuration(), 0.0);
    // 默认构造的列表为空的，epoch 值不假定
}

TEST(TimeIntervalList, EpochConstruction)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);

    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
    // 显式 epoch 应被保留
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, PushBackTimeInterval)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);

    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    EXPECT_EQ(list.size(), 2u);

    // 读取回来的绝对 TimeInterval 的 start/stop 应相对于同一 epoch
    TimeInterval iv0 = list[0];
    EXPECT_DOUBLE_EQ(iv0.start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(iv0.stop().durationFrom(epoch), 10.0);

    TimeInterval iv1 = list[1];
    EXPECT_DOUBLE_EQ(iv1.start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(iv1.stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, PushBackTimePoints)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);

    list.push_back(epoch.shiftedBySecond(0.0), epoch.shiftedBySecond(5.0));
    list.push_back(epoch.shiftedBySecond(10.0), epoch.shiftedBySecond(15.0));

    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(list[1].start().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(list[1].stop().durationFrom(epoch), 15.0);
}

TEST(TimeIntervalList, PopBack)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 1.0));
    list.push_back(TimeInterval(epoch, 2.0, 3.0));

    EXPECT_EQ(list.size(), 2u);
    list.pop_back();
    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, ReserveAndClear)
{
    TimeIntervalList list(testEpoch());
    list.reserve(100);
    list.push_back(TimeInterval(testEpoch(), 0.0, 1.0));
    EXPECT_FALSE(list.empty());
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}


// ————————————————————————
// 元素访问
// ————————————————————————

TEST(TimeIntervalList, ElementAccess)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeInterval iv0 = list[0];
    EXPECT_DOUBLE_EQ(iv0.start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(iv0.stop().durationFrom(epoch), 10.0);

    TimeInterval iv1 = list[1];
    EXPECT_DOUBLE_EQ(iv1.start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(iv1.stop().durationFrom(epoch), 30.0);

    TimeInterval at0 = list.at(0);
    EXPECT_DOUBLE_EQ(at0.start().durationFrom(epoch), 0.0);

    TimeInterval at1 = list.at(1);
    EXPECT_DOUBLE_EQ(at1.stop().durationFrom(epoch), 30.0);

    EXPECT_THROW(list.at(2), std::out_of_range);
}

TEST(TimeIntervalList, AtOnEmpty)
{
    TimeIntervalList list;
    EXPECT_THROW(list.at(0), std::out_of_range);
}

TEST(TimeIntervalList, OperatorBracketIsValueReturn)
{
    // operator[] 按值返回，修改返回值不影响原列表
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeInterval iv = list[0];
    iv.setStart(epoch.shiftedBySecond(999.0));  // 修改副本

    // 原列表不应改变
    TimeInterval iv2 = list[0];
    EXPECT_DOUBLE_EQ(iv2.start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(iv2.stop().durationFrom(epoch), 10.0);
}


// ————————————————————————
// 历元管理
// ————————————————————————

TEST(TimeIntervalList, EpochAccess)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);

    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, SetEpochOnEmpty)
{
    TimeIntervalList list;
    TimePoint newEpoch = testEpoch();

    list.setEpoch(newEpoch);
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(newEpoch), 0.0);
}

TEST(TimeIntervalList, EpochPreservedThroughOperations)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList merged = list.merged();
    EXPECT_DOUBLE_EQ(merged.epoch().durationFrom(epoch), 0.0);

    TimeIntervalList intersected = list.intersected(list);
    EXPECT_DOUBLE_EQ(intersected.epoch().durationFrom(epoch), 0.0);
}


// ————————————————————————
// 底层容器访问
// ————————————————————————

TEST(TimeIntervalList, IntervalsAccess)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 1.0));
    list.push_back(TimeInterval(epoch, 2.0, 3.0));

    const auto& ivs = list.intervals();
    EXPECT_EQ(ivs.size(), 2u);

    // 通过 mutable 访问追加
    list.intervals().push_back(4.0, 5.0);
    EXPECT_EQ(list.size(), 3u);
}

TEST(TimeIntervalList, ConstIntervalsAccessor)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 1.0));
    list.push_back(TimeInterval(epoch, 2.0, 3.0));

    const TimeIntervalList& clist = list;
    const auto& ivs = clist.intervals();
    EXPECT_EQ(ivs.size(), 2u);
    EXPECT_DOUBLE_EQ(ivs[0].start_, 0.0);
}


// ————————————————————————
// 工厂方法
// ————————————————————————

TEST(TimeIntervalList, FromTimeIntervals)
{
    TimePoint epoch = testEpoch();
    std::vector<TimeInterval> vec = {
        TimeInterval(epoch, 0.0, 10.0),
        TimeInterval(epoch, 20.0, 30.0),
    };

    TimeIntervalList list = TimeIntervalList::FromTimeIntervals(vec);
    EXPECT_EQ(list.size(), 2u);

    // epoch 应为第一个区间的 start
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, FromTimeIntervalsEmpty)
{
    std::vector<TimeInterval> vec;
    TimeIntervalList list = TimeIntervalList::FromTimeIntervals(vec);

    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(TimeIntervalList, FromTimeIntervalsWithEpoch)
{
    TimePoint epoch = testEpoch();
    std::vector<TimeInterval> vec = {
        TimeInterval(epoch, 0.0, 10.0),
        TimeInterval(epoch, 20.0, 30.0),
    };

    TimeIntervalList list = TimeIntervalList::FromTimeIntervals(vec, epoch);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, FromTimeIntervalsEmptyWithEpoch)
{
    TimePoint epoch = testEpoch();
    std::vector<TimeInterval> vec;

    TimeIntervalList list = TimeIntervalList::FromTimeIntervals(vec, epoch);
    EXPECT_TRUE(list.empty());
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, FromIntervalList)
{
    TimePoint epoch = testEpoch();
    IntervalList rel;
    rel.push_back(0.0, 10.0);
    rel.push_back(20.0, 30.0);

    TimeIntervalList list = TimeIntervalList::FromIntervalList(rel, epoch);
    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 10.0);
}


// ————————————————————————
// 查询
// ————————————————————————

TEST(TimeIntervalList, TotalDuration)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    EXPECT_DOUBLE_EQ(list.totalDuration(), 20.0);
}

TEST(TimeIntervalList, TotalDurationEmpty)
{
    TimeIntervalList list;
    EXPECT_DOUBLE_EQ(list.totalDuration(), 0.0);
}

TEST(TimeIntervalList, Contains)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(0.0)));
    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(5.0)));
    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(10.0)));   // 边界
    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(20.0)));
    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(25.0)));
    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(30.0)));   // 边界

    EXPECT_FALSE(list.contains(epoch.shiftedBySecond(-1.0)));
    EXPECT_FALSE(list.contains(epoch.shiftedBySecond(15.0)));  // 在间隙中
    EXPECT_FALSE(list.contains(epoch.shiftedBySecond(35.0)));
}

TEST(TimeIntervalList, ContainsEmpty)
{
    TimeIntervalList list;
    // 空列表不包含任何时间点，不假定 epoch 值
    EXPECT_FALSE(list.contains(TimePoint::J2000TT()));
}


// ————————————————————————
// 合并（merged）
// ————————————————————————

TEST(TimeIntervalList, MergedEmpty)
{
    TimeIntervalList list(testEpoch());
    TimeIntervalList result = list.merged();
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, MergedSingle)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, MergedNonOverlapping)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 20.0, 30.0));
    list.push_back(TimeInterval(epoch, 0.0, 10.0));   // 无序

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, MergedOverlapping)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
}

TEST(TimeIntervalList, MergedComplex)
{
    // [0,10], [5,15], [20,30], [25,35], [40,50]
    // → [0,15], [20,35], [40,50]
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 5.0, 15.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));
    list.push_back(TimeInterval(epoch, 25.0, 35.0));
    list.push_back(TimeInterval(epoch, 40.0, 50.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 3u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 35.0);
    EXPECT_DOUBLE_EQ(result[2].start().durationFrom(epoch), 40.0);
    EXPECT_DOUBLE_EQ(result[2].stop().durationFrom(epoch), 50.0);
}

TEST(TimeIntervalList, MergedDoesNotModifyOriginal)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 20.0, 30.0));
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    size_t origSize = list.size();
    list.merged();
    EXPECT_EQ(list.size(), origSize);
}

TEST(TimeIntervalList, MergedAdjacent)
{
    // [0,10] 和 [10,20] 首尾相邻 — API 文档承诺合并"相邻"区间
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 10.0, 20.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}

TEST(TimeIntervalList, MergedMultipleAdjacent)
{
    // [0,10], [10,20], [20,30] → [0,30]
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 10.0, 20.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, MergedFullyContained)
{
    // [0,20] 完全包含 [5,10]，合并后仍为 [0,20]
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 20.0));
    list.push_back(TimeInterval(epoch, 5.0, 10.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}

TEST(TimeIntervalList, MergedMixedAdjacentAndOverlap)
{
    // [0,10] 相邻 [10,20]，[25,35] 重叠 [30,40] → [0,20], [25,40]
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 10.0, 20.0));
    list.push_back(TimeInterval(epoch, 25.0, 35.0));
    list.push_back(TimeInterval(epoch, 30.0, 40.0));

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 25.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 40.0);
}


// ————————————————————————
// 原地合并（mergeInPlace）
// ————————————————————————

TEST(TimeIntervalList, MergeInPlaceEmpty)
{
    TimeIntervalList list(testEpoch());
    list.mergeInPlace();
    EXPECT_TRUE(list.empty());
}

TEST(TimeIntervalList, MergeInPlaceOverlapping)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 5.0, 15.0));

    list.mergeInPlace();
    EXPECT_EQ(list.size(), 1u);

    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 15.0);
}

TEST(TimeIntervalList, MergeInPlaceIdempotent)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 20.0, 30.0));
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    list.mergeInPlace();
    // 再次调用应不变
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(list[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(list[1].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, MergeInPlaceAdjacent)
{
    // [10,20], [0,10] (乱序相邻) → [0,20]
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 10.0, 20.0));
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    list.mergeInPlace();
    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop().durationFrom(epoch), 20.0);
}


// ————————————————————————
// 交集（intersect）— 同历元
// ————————————————————————

TEST(TimeIntervalList, IntersectEmpty)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);

    TimeIntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, IntersectNoOverlap)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, IntersectPartial)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, IntersectMultiple)
{
    // a: [0,10], [20,30]
    // b: [5,25]
    // 交集: [5,10], [20,25]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 25.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 25.0);
}

TEST(TimeIntervalList, IntersectedDoesNotModifyOperands)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 25.0));

    size_t aSize = a.size();
    size_t bSize = b.size();
    a.intersected(b);

    // 副本方法不应改变操作数
    EXPECT_EQ(a.size(), aSize);
    EXPECT_EQ(b.size(), bSize);
}

TEST(TimeIntervalList, IntersectInPlace)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 25.0));

    TimeIntervalList& ref = a.intersect(b);

    // 原地修改：a 变为交集 [5,10], [20,25]，epoch 不变
    EXPECT_EQ(&ref, &a);
    EXPECT_DOUBLE_EQ(a.epoch().durationFrom(epoch), 0.0);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_DOUBLE_EQ(a[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(a[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(a[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(a[1].stop().durationFrom(epoch), 25.0);
}

TEST(TimeIntervalList, IntersectBothEmpty)
{
    TimeIntervalList a(testEpoch());
    TimeIntervalList b(testEpoch());

    TimeIntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, IntersectFullyContained)
{
    // [0,20] ∩ [5,10] = [5,10]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 20.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 10.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, IntersectSelf)
{
    // a ∩ a = a.merged() — 集合与自身的交集等于自身
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 10.0, 20.0));  // 乱序
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.intersected(a);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}

TEST(TimeIntervalList, IntersectComplexBothMultiple)
{
    // a: [0,10], [20,30], [40,50]
    // b: [5,25], [35,45]
    // 交集: [5,10], [20,25], [40,45]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    a.push_back(TimeInterval(epoch, 40.0, 50.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 25.0));
    b.push_back(TimeInterval(epoch, 35.0, 45.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 25.0);
    EXPECT_DOUBLE_EQ(result[2].start().durationFrom(epoch), 40.0);
    EXPECT_DOUBLE_EQ(result[2].stop().durationFrom(epoch), 45.0);
}


// ————————————————————————
// 并集（unite）— 同历元
// ————————————————————————

TEST(TimeIntervalList, UniteEmpty)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, UniteNonOverlapping)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, UniteOverlapping)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
}

TEST(TimeIntervalList, UnitedDoesNotModifyOperands)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 15.0));

    size_t aSize = a.size();
    a.united(b);
    // 副本方法不应改变操作数
    EXPECT_EQ(a.size(), aSize);
}

TEST(TimeIntervalList, UniteInPlace)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 15.0));
    b.push_back(TimeInterval(epoch, 25.0, 35.0));

    TimeIntervalList& ref = a.unite(b);

    // 原地修改：a 变为并集 [0,15], [20,35]，epoch 不变
    EXPECT_EQ(&ref, &a);
    EXPECT_DOUBLE_EQ(a.epoch().durationFrom(epoch), 0.0);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_DOUBLE_EQ(a[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(a[0].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(a[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(a[1].stop().durationFrom(epoch), 35.0);
}

TEST(TimeIntervalList, UniteBothEmpty)
{
    TimeIntervalList a(testEpoch());
    TimeIntervalList b(testEpoch());

    TimeIntervalList result = a.united(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, UniteFullyContained)
{
    // [0,20] ∪ [5,10] = [0,20]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 20.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 10.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}

TEST(TimeIntervalList, UniteSelf)
{
    // a ∪ a = a.merged() — 集合与自身的并集等于自身
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 10.0, 20.0));
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.united(a);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}

TEST(TimeIntervalList, UniteAdjacent)
{
    // [0,10] ∪ [10,20] = [0,20] — 相邻区间并集应合并
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 10.0, 20.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 20.0);
}


// ————————————————————————
// 差集（subtract）— 同历元
// ————————————————————————

TEST(TimeIntervalList, SubtractEmpty)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, SubtractMiddle)
{
    // [0,10] - [3,7] = [0,3], [7,10]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 3.0, 7.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 3.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 7.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, SubtractComplete)
{
    // [0,10] - [0,10] = empty
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, SubtractMultiple)
{
    // [0,30] - [5,10], [15,20] = [0,5], [10,15], [20,30]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 10.0));
    b.push_back(TimeInterval(epoch, 15.0, 20.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 3u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(result[2].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[2].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, SubtractLeftEmpty)
{
    // empty - [0,10] = empty
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, SubtractBothEmpty)
{
    TimeIntervalList a(testEpoch());
    TimeIntervalList b(testEpoch());

    TimeIntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, SubtractInPlace)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 10.0));
    b.push_back(TimeInterval(epoch, 15.0, 20.0));

    TimeIntervalList& ref = a.subtract(b);

    // 原地修改：a 变为差集 [0,5], [10,15], [20,30]，epoch 不变
    EXPECT_EQ(&ref, &a);
    EXPECT_DOUBLE_EQ(a.epoch().durationFrom(epoch), 0.0);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_DOUBLE_EQ(a[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(a[0].stop().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(a[1].start().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(a[1].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(a[2].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(a[2].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, SubtractLeftOverhang)
{
    // b 从左侧伸出: [5,15] - [0,10] = [10,15]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 5.0, 15.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
}

TEST(TimeIntervalList, SubtractRightOverhang)
{
    // b 从右侧伸出: [5,15] - [10,20] = [5,10]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 5.0, 15.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 10.0, 20.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, SubtractBothOverhang)
{
    // b 从两侧伸出: [5,10] - [0,20] = empty
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 5.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 20.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, SubtractSelf)
{
    // a - a = empty — 集合与自身的差集为空
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList result = a.subtracted(a);
    EXPECT_TRUE(result.empty());
}

TEST(TimeIntervalList, SubtractAtLeftBoundary)
{
    // [0,10] - [5,10] = [0,5] — b 与 a 右对齐
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 10.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 5.0);
}

TEST(TimeIntervalList, SubtractAtRightBoundary)
{
    // [0,10] - [0,5] = [5,10] — b 与 a 左对齐
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 0.0, 5.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}


// ————————————————————————
// 集合运算对无序/重叠输入的鲁棒性
//
// API 文档承诺："列表可以无序。集合运算（merged、intersect 等）
// 内部会先排序合并。"以下测试验证这一契约——
// 直接传入乱序、有重叠的列表与先 merged() 再运算的结果一致。
// ————————————————————————

TEST(TimeIntervalList, IntersectUnsortedOverlappingA)
{
    // a (重叠+乱序): [20,30], [5,15], [8,12] → 实际集合: [5,15], [20,30]
    // b: [10,25]
    // 交集: [10,15], [20,25]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    a.push_back(TimeInterval(epoch, 5.0, 15.0));
    a.push_back(TimeInterval(epoch, 8.0, 12.0));  // 被 [5,15] 完全包含

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 10.0, 25.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 25.0);
}

TEST(TimeIntervalList, IntersectBothUnsortedOverlapping)
{
    // a (重叠+乱序): [25,35], [0,12], [5,15] → 实际集合: [0,15], [25,35]
    // b (重叠+乱序): [28,32], [8,20]         → 实际集合: [8,20], [28,32]
    // 交集: [8,15], [28,32]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 25.0, 35.0));
    a.push_back(TimeInterval(epoch, 0.0, 12.0));
    a.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 28.0, 32.0));
    b.push_back(TimeInterval(epoch, 8.0, 20.0));

    TimeIntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 8.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 15.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 28.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 32.0);
}

TEST(TimeIntervalList, UniteUnsortedOverlappingInputs)
{
    // a (重叠+乱序): [20,30], [0,8], [5,15] → 实际集合: [0,15], [20,30]
    // b: [12,25]
    // 并集: [0,30]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    a.push_back(TimeInterval(epoch, 0.0, 8.0));
    a.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 12.0, 25.0));

    TimeIntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, SubtractUnsortedOverlappingInputs)
{
    // a (重叠+乱序): [20,30], [5,15], [0,8] → 实际集合: [0,15], [20,30]
    // b: [10,25]
    // 差集: [0,10], [25,30]
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    a.push_back(TimeInterval(epoch, 5.0, 15.0));
    a.push_back(TimeInterval(epoch, 0.0, 8.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 10.0, 25.0));

    TimeIntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 25.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, SetOpsEquivalentToMerged)
{
    // 核心不变式：对任意无序/重叠输入，集合运算结果 ≡ 先合并再运算
    // intersected(a, b) == merged(a).intersected(merged(b))
    // united(a, b)      == merged(a).united(merged(b))
    // subtracted(a, b)  == merged(a).subtracted(merged(b))
    TimePoint epoch = testEpoch();

    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    a.push_back(TimeInterval(epoch, 5.0, 15.0));
    a.push_back(TimeInterval(epoch, 8.0, 12.0));  // 乱序 + 重叠

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 28.0, 32.0));
    b.push_back(TimeInterval(epoch, 8.0, 20.0));  // 乱序 + 重叠

    // 直接运算（未先合并）
    TimeIntervalList directInter = a.intersected(b);
    TimeIntervalList directUnite = a.united(b);
    TimeIntervalList directSub = a.subtracted(b);

    // 先合并再运算（参照标准）
    TimeIntervalList mergedInter = a.merged().intersected(b.merged());
    TimeIntervalList mergedUnite = a.merged().united(b.merged());
    TimeIntervalList mergedSub = a.merged().subtracted(b.merged());

    // 结果应完全一致
    ASSERT_EQ(directInter.size(), mergedInter.size());
    ASSERT_EQ(directUnite.size(), mergedUnite.size());
    ASSERT_EQ(directSub.size(), mergedSub.size());

    for (size_t i = 0; i < directInter.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(directInter[i].start().durationFrom(epoch),
                         mergedInter[i].start().durationFrom(epoch));
        EXPECT_DOUBLE_EQ(directInter[i].stop().durationFrom(epoch),
                         mergedInter[i].stop().durationFrom(epoch));
    }
    for (size_t i = 0; i < directUnite.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(directUnite[i].start().durationFrom(epoch),
                         mergedUnite[i].start().durationFrom(epoch));
        EXPECT_DOUBLE_EQ(directUnite[i].stop().durationFrom(epoch),
                         mergedUnite[i].stop().durationFrom(epoch));
    }
    for (size_t i = 0; i < directSub.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(directSub[i].start().durationFrom(epoch),
                         mergedSub[i].start().durationFrom(epoch));
        EXPECT_DOUBLE_EQ(directSub[i].stop().durationFrom(epoch),
                         mergedSub[i].stop().durationFrom(epoch));
    }
}


// ————————————————————————
// 跨历元运算
// ————————————————————————

TEST(TimeIntervalList, IntersectDifferentEpochNoOverlap)
{
    // epoch1: J2000 + 0s    — 区间 [0, 50]
    // epoch2: J2000 + 100s  — 区间 [0, 25] (绝对时间 [100, 125])
    // 交集应为空
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(100.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 50.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 25.0));

    TimeIntervalList result = list1.intersected(list2);
    EXPECT_TRUE(result.empty());
    // epoch 应为 list1 的 epoch
    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
}

TEST(TimeIntervalList, IntersectDifferentEpochWithOverlap)
{
    // epoch1: J2000 + 0s    — 区间 [0, 50]
    // epoch2: J2000 + 30s   — 区间 [0, 30] (绝对时间 [30, 60])
    // 交集: [epoch1+30, epoch1+50]
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(30.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 50.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 30.0));

    TimeIntervalList result = list1.intersected(list2);
    EXPECT_EQ(result.size(), 1u);

    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 30.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 50.0);
}

TEST(TimeIntervalList, UniteDifferentEpoch)
{
    // epoch1: J2000 + 0s    — 区间 [0, 10]
    // epoch2: J2000 + 50s   — 区间 [0, 10] (绝对时间 [50, 60])
    // 并集: [epoch1+0, epoch1+10], [epoch1+50, epoch1+60]
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(50.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 10.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 10.0));

    TimeIntervalList result = list1.united(list2);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch1), 50.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch1), 60.0);
}

TEST(TimeIntervalList, SubtractDifferentEpoch)
{
    // epoch1: J2000 + 0s    — 区间 [0, 100]
    // epoch2: J2000 + 30s   — 区间 [0, 20] (绝对时间 [30, 50])
    // 差集: [0,30], [50,100]
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(30.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 100.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 20.0));

    TimeIntervalList result = list1.subtracted(list2);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 30.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch1), 50.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch1), 100.0);
}

TEST(TimeIntervalList, IntersectDifferentEpochMultiInterval)
{
    // epoch1: J2000 + 0s    — [0,20], [40,60]
    // epoch2: J2000 + 10s   — [0,15] (绝对 [10,25]), [30,40] (绝对 [40,50])
    // 交集: [10,20], [40,50]
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(10.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 20.0));
    list1.push_back(TimeInterval(epoch1, 40.0, 60.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 15.0));
    list2.push_back(TimeInterval(epoch2, 30.0, 40.0));

    TimeIntervalList result = list1.intersected(list2);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 20.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch1), 40.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch1), 50.0);
}

TEST(TimeIntervalList, UniteDifferentEpochEmpty)
{
    // 空列表 ∪ 跨历元列表 = 对方列表（转换到当前历元）
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(100.0);

    TimeIntervalList list1(epoch1);  // 空

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 10.0));  // 绝对 [100, 110]

    TimeIntervalList result = list1.united(list2);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 100.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 110.0);
}

TEST(TimeIntervalList, SubtractDifferentEpochBOverlapsLeft)
{
    // epoch1: J2000 + 0s    — [50, 100]
    // epoch2: J2000 + 30s   — [0, 40] (绝对 [30, 70])
    // 差集: [70, 100]
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(30.0);

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 50.0, 100.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 0.0, 40.0));

    TimeIntervalList result = list1.subtracted(list2);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 70.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 100.0);
}

TEST(TimeIntervalList, CrossEpochBEarlierThanA)
{
    // epoch1: J2000 + 100s  — [0, 50] (绝对 [100, 150])
    // epoch2: J2000 + 0s    — [120, 130] (绝对 [120, 130])
    // b 的历元早于 a 的历元
    // 交集: [20, 30] (相对于 epoch1)
    TimePoint epochBase = testEpoch();
    TimePoint epoch1 = epochBase.shiftedBySecond(100.0);
    TimePoint epoch2 = epochBase;  // b 的历元早于 a

    TimeIntervalList list1(epoch1);
    list1.push_back(TimeInterval(epoch1, 0.0, 50.0));

    TimeIntervalList list2(epoch2);
    list2.push_back(TimeInterval(epoch2, 120.0, 130.0));

    TimeIntervalList result = list1.intersected(list2);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result.epoch().durationFrom(epoch1), 0.0);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch1), 20.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch1), 30.0);
}


// ————————————————————————
// 转换
// ————————————————————————

TEST(TimeIntervalList, ToIntervalList)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    IntervalList rel = list.toIntervalList();
    EXPECT_EQ(rel.size(), 2u);
    EXPECT_DOUBLE_EQ(rel[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(rel[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(rel[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(rel[1].stop_, 30.0);
}

TEST(TimeIntervalList, ToIntervalListRoundTrip)
{
    TimePoint epoch = testEpoch();
    IntervalList rel;
    rel.push_back(0.0, 10.0);
    rel.push_back(20.0, 30.0);

    TimeIntervalList list = TimeIntervalList::FromIntervalList(rel, epoch);
    IntervalList relBack = list.toIntervalList();

    EXPECT_EQ(relBack.size(), rel.size());
    EXPECT_DOUBLE_EQ(relBack[0].start_, rel[0].start_);
    EXPECT_DOUBLE_EQ(relBack[0].stop_, rel[0].stop_);
    EXPECT_DOUBLE_EQ(relBack[1].start_, rel[1].start_);
    EXPECT_DOUBLE_EQ(relBack[1].stop_, rel[1].stop_);
}


// ————————————————————————
// 离散化（discretize）
//
// 离散化的区间语义为闭区间 [start, stop]：
// - 端点 start 和 stop 始终被包含
// - 中间按步长 step 生成均匀分布的点
// - 若步长不能整除区间长度，stop 作为最后一个点被强制加入
//   例如：[0, 10] step=3 → 0, 3, 6, 9, 10（5个点）
// ————————————————————————

TEST(TimeIntervalList, DiscretizeBasic)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeList tl = list.discretize(2.0);
    ASSERT_EQ(tl.size(), 6u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 2.0);
    EXPECT_DOUBLE_EQ(secs[2], 4.0);
    EXPECT_DOUBLE_EQ(secs[3], 6.0);
    EXPECT_DOUBLE_EQ(secs[4], 8.0);
    EXPECT_DOUBLE_EQ(secs[5], 10.0);
}

TEST(TimeIntervalList, DiscretizeEmpty)
{
    TimeIntervalList list(testEpoch());
    TimeList tl = list.discretize(1.0);
    EXPECT_TRUE(tl.empty());
}

TEST(TimeIntervalList, DiscretizeZeroStep)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeList tl = list.discretize(0.0);
    EXPECT_TRUE(tl.empty());
}

TEST(TimeIntervalList, DiscretizeWithExplicitEpoch)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimePoint outputEpoch = epoch.shiftedBySecond(500.0);
    TimeList tl = list.discretize(outputEpoch, 2.0);

    ASSERT_FALSE(tl.empty());
    EXPECT_DOUBLE_EQ(tl.epoch().durationFrom(outputEpoch), 0.0);
}

TEST(TimeIntervalList, DiscretizePreservesList)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    size_t origSize = list.size();
    list.discretize(1.0);

    EXPECT_EQ(list.size(), origSize);
}

TEST(TimeIntervalList, DiscretizeMultipleIntervals)
{
    // [0,5], [10,15] step 2.0
    // 每个区间输出: start + i*step 直到 <= stop，且始终包含端点 stop
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 5.0));
    list.push_back(TimeInterval(epoch, 10.0, 15.0));

    TimeList tl = list.discretize(2.0);
    // [0,5]: 0, 2, 4, 5 (4点；5 是端点)
    // [10,15]: 10, 12, 14, 15 (4点；15 是端点)
    ASSERT_EQ(tl.size(), 8u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 2.0);
    EXPECT_DOUBLE_EQ(secs[2], 4.0);
    EXPECT_DOUBLE_EQ(secs[3], 5.0);
    EXPECT_DOUBLE_EQ(secs[4], 10.0);
    EXPECT_DOUBLE_EQ(secs[5], 12.0);
    EXPECT_DOUBLE_EQ(secs[6], 14.0);
    EXPECT_DOUBLE_EQ(secs[7], 15.0);
}

TEST(TimeIntervalList, DiscretizeNonDivisibleStep)
{
    // [0,10] step 3.0 — 步长不能整除区间长度，端点 10 应被强制包含
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeList tl = list.discretize(3.0);
    // 步进点: 0, 3, 6, 9；端点: 10
    ASSERT_EQ(tl.size(), 5u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 3.0);
    EXPECT_DOUBLE_EQ(secs[2], 6.0);
    EXPECT_DOUBLE_EQ(secs[3], 9.0);
    EXPECT_DOUBLE_EQ(secs[4], 10.0);
}

TEST(TimeIntervalList, DiscretizeStepLargerThanInterval)
{
    // [1, 2] step 5.0 — 步长大于区间长度，至少包含起止端点
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 1.0, 2.0));

    TimeList tl = list.discretize(5.0);
    ASSERT_EQ(tl.size(), 2u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 1.0);
    EXPECT_DOUBLE_EQ(secs[1], 2.0);
}


// ————————————————————————
// 拷贝与移动
// ————————————————————————

TEST(TimeIntervalList, CopyConstruction)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList b = a;
    EXPECT_EQ(b.size(), 2u);
    EXPECT_DOUBLE_EQ(b.epoch().durationFrom(epoch), 0.0);

    EXPECT_DOUBLE_EQ(b[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(b[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(b[1].start().durationFrom(epoch), 20.0);
    EXPECT_DOUBLE_EQ(b[1].stop().durationFrom(epoch), 30.0);
}

TEST(TimeIntervalList, CopyAssignment)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 100.0, 200.0));

    b = a;
    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(b[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, MoveConstruction)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, MoveConstructionLeavesSourceEmpty)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));

    TimeIntervalList b = std::move(a);
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.size(), 0u);
    EXPECT_EQ(b.size(), 2u);
}

TEST(TimeIntervalList, MoveAssignment)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 100.0, 200.0));

    b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start().durationFrom(epoch), 0.0);
}

TEST(TimeIntervalList, SelfAssignment)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    list = list;  // 自赋值
    EXPECT_EQ(list.size(), 2u);
}


// ————————————————————————
// 链式操作
// ————————————————————————

TEST(TimeIntervalList, ChainedMergedIntersect)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 5.0));
    a.push_back(TimeInterval(epoch, 3.0, 10.0));
    a.push_back(TimeInterval(epoch, 12.0, 15.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 13.0));

    // a.merged() = [0,10], [12,15]
    // 交集: [5,10], [12,13]
    TimeIntervalList result = a.merged().intersected(b);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 12.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 13.0);
}

TEST(TimeIntervalList, ChainedUniteSubtract)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));

    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 15.0));

    TimeIntervalList c(epoch);
    c.push_back(TimeInterval(epoch, 2.0, 3.0));

    // a.united(b) = [0,15], subtract c → [0,2], [3,15]
    TimeIntervalList result = a.united(b).subtracted(c);
    EXPECT_EQ(result.size(), 2u);

    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 2.0);
    EXPECT_DOUBLE_EQ(result[1].start().durationFrom(epoch), 3.0);
    EXPECT_DOUBLE_EQ(result[1].stop().durationFrom(epoch), 15.0);
}


// ————————————————————————
// 边界情况
// ————————————————————————

TEST(TimeIntervalList, ZeroDurationInterval)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 5.0, 5.0));

    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list.totalDuration(), 0.0);
}

TEST(TimeIntervalList, NegativeOffsetIntervals)
{
    // 负偏移（相对于 epoch）
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, -30.0, -20.0));
    list.push_back(TimeInterval(epoch, -10.0, 0.0));

    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list.totalDuration(), 20.0);

    EXPECT_TRUE(list.contains(epoch.shiftedBySecond(-25.0)));
    EXPECT_FALSE(list.contains(epoch.shiftedBySecond(-15.0)));
}

TEST(TimeIntervalList, LargeNumberOfIntervals)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    const size_t N = 1000;
    for (size_t i = 0; i < N; ++i)
    {
        double t = static_cast<double>(i);
        list.push_back(TimeInterval(epoch, t * 100.0, t * 100.0 + 50.0));
    }
    EXPECT_EQ(list.size(), N);
    EXPECT_DOUBLE_EQ(list.totalDuration(), N * 50.0);

    // 合并不应改变结果（不重叠）
    TimeIntervalList merged = list.merged();
    EXPECT_EQ(merged.size(), N);
}

TEST(TimeIntervalList, ReverseInterval)
{
    // start > stop 的反向区间：duration 应为 0，不包含任何点
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 10.0, 0.0));  // 反向

    EXPECT_EQ(list.size(), 1u);
    // IntervalList::totalDuration 文档："反向区间贡献为 0"
    EXPECT_DOUBLE_EQ(list.totalDuration(), 0.0);
    // 反向区间内不应包含任何点
    EXPECT_FALSE(list.contains(epoch.shiftedBySecond(5.0)));
}

TEST(TimeIntervalList, ZeroDurationInMerged)
{
    // 零时长区间与普通区间合并：应被吸收
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 5.0, 5.0));  // 零时长点

    TimeIntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop().durationFrom(epoch), 10.0);
}

TEST(TimeIntervalList, PushBackUnsortedMaintainsStorage)
{
    // push_back 乱序区间后，内部存储顺序应保持插入顺序
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 20.0, 30.0));
    list.push_back(TimeInterval(epoch, 0.0, 10.0));

    const auto& ivs = list.intervals();
    EXPECT_EQ(ivs.size(), 2u);
    EXPECT_DOUBLE_EQ(ivs[0].start_, 20.0);  // 第一个插入的
    EXPECT_DOUBLE_EQ(ivs[1].start_, 0.0);   // 第二个插入的
}

TEST(TimeIntervalList, MoveSelfAssignment)
{
    // 移动自赋值：不应崩溃
    TimePoint epoch = testEpoch();
    TimeIntervalList list(epoch);
    list.push_back(TimeInterval(epoch, 0.0, 10.0));
    list.push_back(TimeInterval(epoch, 20.0, 30.0));

    list = std::move(list);  // 移动自赋值
    // 不崩溃即为通过，但内容应仍可安全访问
    EXPECT_GE(list.size(), 0u);
}

TEST(TimeIntervalList, SetEpochOnNonEmpty)
{
    // 在非空列表上 setEpoch：底层相对偏移不变，但返回的绝对区间使用新历元
    TimePoint epoch1 = testEpoch();
    TimePoint epoch2 = epoch1.shiftedBySecond(1000.0);

    TimeIntervalList list(epoch1);
    list.push_back(TimeInterval(epoch1, 0.0, 10.0));

    size_t oldSize = list.size();
    list.setEpoch(epoch2);

    // 底层数据量不变
    EXPECT_EQ(list.size(), oldSize);
    // 新 epoch 已生效
    EXPECT_DOUBLE_EQ(list.epoch().durationFrom(epoch2), 0.0);
    // 底层相对偏移不变
    EXPECT_DOUBLE_EQ(list.intervals()[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list.intervals()[0].stop_, 10.0);
    // 返回的绝对区间使用新历元（而非旧历元）
    TimeInterval iv = list[0];
    EXPECT_DOUBLE_EQ(iv.start().durationFrom(epoch2), 0.0);
    EXPECT_DOUBLE_EQ(iv.stop().durationFrom(epoch2), 10.0);
}


TEST(TimeIntervalList, OperatorOverloads)
{
    TimePoint epoch = testEpoch();
    TimeIntervalList a(epoch);
    a.push_back(TimeInterval(epoch, 0.0, 10.0));
    a.push_back(TimeInterval(epoch, 20.0, 30.0));
    TimeIntervalList b(epoch);
    b.push_back(TimeInterval(epoch, 5.0, 25.0));

    // 副本运算符
    TimeIntervalList inter = a & b;   // [5,10], [20,25]
    TimeIntervalList uni  = a | b;    // [0,30]
    TimeIntervalList diff = a - b;    // [0,5], [25,30]

    EXPECT_EQ(inter.size(), 2u);
    EXPECT_DOUBLE_EQ(inter[0].start().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(inter[1].start().durationFrom(epoch), 20.0);

    EXPECT_EQ(uni.size(), 1u);
    EXPECT_DOUBLE_EQ(uni[0].start().durationFrom(epoch), 0.0);
    EXPECT_DOUBLE_EQ(uni[0].stop().durationFrom(epoch), 30.0);

    EXPECT_EQ(diff.size(), 2u);
    EXPECT_DOUBLE_EQ(diff[0].stop().durationFrom(epoch), 5.0);
    EXPECT_DOUBLE_EQ(diff[1].start().durationFrom(epoch), 25.0);

    // 副本运算符不改操作数
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 1u);

    // 原地运算符
    TimeIntervalList c = a;
    c &= b;
    EXPECT_EQ(c.size(), 2u);

    c = a;
    c |= b;
    EXPECT_EQ(c.size(), 1u);

    c = a;
    c -= b;
    EXPECT_EQ(c.size(), 2u);
}


GTEST_MAIN()
