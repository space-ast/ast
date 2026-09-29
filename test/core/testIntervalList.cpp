///
/// @file      testIntervalList.cpp
/// @brief     相对时段列表测试
/// @details   测试IntervalList类的构造、访问、集合运算和离散化功能
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

#include "ast/IntervalList.hpp"
#include "ast/TimeList.hpp"
#include "ast/TimePoint.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE


// ————————————————————————
// 构造与容量
// ————————————————————————

TEST(IntervalList, DefaultConstruction)
{
    IntervalList list;
    EXPECT_EQ(list.size(), 0u);
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.totalDuration(), 0.0);
}

TEST(IntervalList, InitializerListConstruction)
{
    IntervalList list = {
        {0.0, 10.0},
        {20.0, 30.0},
    };

    EXPECT_EQ(list.size(), 2u);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list[0].start_, 0.0);
    EXPECT_EQ(list[0].stop_, 10.0);
    EXPECT_EQ(list[1].start_, 20.0);
    EXPECT_EQ(list[1].stop_, 30.0);
}

TEST(IntervalList, PushBack)
{
    IntervalList list;
    list.push_back({0.0, 5.0});
    list.push_back(10.0, 15.0);

    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].start_, 0.0);
    EXPECT_EQ(list[0].stop_, 5.0);
    EXPECT_EQ(list[1].start_, 10.0);
    EXPECT_EQ(list[1].stop_, 15.0);
}

TEST(IntervalList, PopBack)
{
    IntervalList list = {{0.0, 1.0}, {2.0, 3.0}};
    EXPECT_EQ(list.size(), 2u);
    list.pop_back();
    EXPECT_EQ(list.size(), 1u);
    EXPECT_EQ(list[0].start_, 0.0);
}

TEST(IntervalList, ReserveAndClear)
{
    IntervalList list;
    list.reserve(100);
    list.push_back(0.0, 1.0);
    EXPECT_FALSE(list.empty());
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}


// ————————————————————————
// 元素访问
// ————————————————————————

TEST(IntervalList, ElementAccess)
{
    IntervalList list = {{0.0, 10.0}, {20.0, 30.0}};

    EXPECT_EQ(list[0].start_, 0.0);
    EXPECT_EQ(list[0].stop_, 10.0);
    EXPECT_EQ(list[1].start_, 20.0);
    EXPECT_EQ(list[1].stop_, 30.0);

    EXPECT_EQ(list.at(0).start_, 0.0);
    EXPECT_EQ(list.at(1).stop_, 30.0);

    EXPECT_THROW(list.at(2), std::out_of_range);
}

TEST(IntervalList, Iterator)
{
    IntervalList list = {{0.0, 1.0}, {2.0, 3.0}, {4.0, 5.0}};

    size_t count = 0;
    for (const auto& iv : list)
    {
        A_UNUSED(iv);
        ++count;
    }
    EXPECT_EQ(count, 3u);
}

TEST(IntervalList, IntervalsAccess)
{
    IntervalList list = {{0.0, 1.0}, {2.0, 3.0}};

    const auto& ivs = list.intervals();
    EXPECT_EQ(ivs.size(), 2u);

    list.intervals().push_back({4.0, 5.0});
    EXPECT_EQ(list.size(), 3u);
}


// ————————————————————————
// 从区间向量构造
// ————————————————————————

TEST(IntervalList, FromIntervals)
{
    std::vector<Interval> vec = {{1.0, 2.0}, {3.0, 4.0}};
    IntervalList list = IntervalList::FromIntervals(vec);

    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].start_, 1.0);
    EXPECT_EQ(list[1].stop_, 4.0);
}


// ————————————————————————
// 查询
// ————————————————————————

TEST(IntervalList, TotalDuration)
{
    IntervalList list = {
        {0.0, 10.0},
        {20.0, 30.0},
    };

    // 不重叠区间：10 + 10 = 20
    EXPECT_DOUBLE_EQ(list.totalDuration(), 20.0);
}

TEST(IntervalList, TotalDurationWithOverlap)
{
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 15.0},  // 与第一个重叠
    };

    // totalDuration 不考虑重叠：10 + 10 = 20
    EXPECT_DOUBLE_EQ(list.totalDuration(), 20.0);
}

TEST(IntervalList, TotalDurationEmpty)
{
    IntervalList list;
    EXPECT_DOUBLE_EQ(list.totalDuration(), 0.0);
}

TEST(IntervalList, Contains)
{
    IntervalList list = {
        {0.0, 10.0},
        {20.0, 30.0},
    };

    EXPECT_TRUE(list.contains(0.0));
    EXPECT_TRUE(list.contains(5.0));
    EXPECT_TRUE(list.contains(10.0));   // 边界
    EXPECT_TRUE(list.contains(20.0));
    EXPECT_TRUE(list.contains(25.0));
    EXPECT_TRUE(list.contains(30.0));   // 边界

    EXPECT_FALSE(list.contains(-1.0));
    EXPECT_FALSE(list.contains(15.0));  // 在间隙中
    EXPECT_FALSE(list.contains(35.0));
}

TEST(IntervalList, ContainsEmpty)
{
    IntervalList list;
    EXPECT_FALSE(list.contains(0.0));
    EXPECT_FALSE(list.contains(10.0));
}


// ————————————————————————
// 合并（merged）
// ————————————————————————

TEST(IntervalList, MergedEmpty)
{
    IntervalList list;
    IntervalList result = list.merged();
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, MergedSingle)
{
    IntervalList list = {{0.0, 10.0}};
    IntervalList result = list.merged();

    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, MergedNonOverlapping)
{
    IntervalList list = {
        {20.0, 30.0},
        {0.0, 10.0},   // 无序
    };

    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 2u);
    // 应已排序
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
}

TEST(IntervalList, MergedOverlapping)
{
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 15.0},
    };

    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}

TEST(IntervalList, MergedAdjacent)
{
    IntervalList list = {
        {0.0, 10.0},
        {10.0, 20.0},  // 相邻
    };

    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 20.0);
}

TEST(IntervalList, MergedComplex)
{
    // [0,10], [5,15], [20,30], [25,35], [40,50]
    // 合并后应为 [0,15], [20,35], [40,50]
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 15.0},
        {20.0, 30.0},
        {25.0, 35.0},
        {40.0, 50.0},
    };

    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 35.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 40.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 50.0);
}

TEST(IntervalList, MergedContainment)
{
    // 一个区间完全包含另一个
    IntervalList list = {
        {0.0, 100.0},
        {20.0, 30.0},
    };

    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 100.0);
}


// ————————————————————————
// 交集（intersect）
// ————————————————————————

TEST(IntervalList, IntersectEmpty)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b;
    IntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, IntersectNoOverlap)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{20.0, 30.0}};

    IntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, IntersectPartial)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{5.0, 15.0}};

    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, IntersectContainment)
{
    IntervalList a = {{0.0, 100.0}};
    IntervalList b = {{20.0, 30.0}};

    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 30.0);
}

TEST(IntervalList, IntersectMultiple)
{
    // a: [0,10], [20,30]
    // b: [5,25]
    // 交集: [5,10], [20,25]
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 25.0}};

    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 25.0);
}


// ————————————————————————
// 并集（unite）
// ————————————————————————

TEST(IntervalList, UniteEmpty)
{
    IntervalList a;
    IntervalList b = {{0.0, 10.0}};

    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, UniteNonOverlapping)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{20.0, 30.0}};

    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
}

TEST(IntervalList, UniteOverlapping)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{5.0, 15.0}};

    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}


// ————————————————————————
// 差集（subtract）
// ————————————————————————

TEST(IntervalList, SubtractEmpty)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b;

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, SubtractNoOverlap)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{20.0, 30.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, SubtractMiddle)
{
    // [0,10] - [3,7] = [0,3], [7,10]
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{3.0, 7.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 3.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 7.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 10.0);
}

TEST(IntervalList, SubtractLeft)
{
    // [5,15] - [0,10] = [10,15]
    IntervalList a = {{5.0, 15.0}};
    IntervalList b = {{0.0, 10.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}

TEST(IntervalList, SubtractRight)
{
    // [0,10] - [5,15] = [0,5]
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{5.0, 15.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
}

TEST(IntervalList, SubtractMultiple)
{
    // [0,30] - [5,10] - [15,20] = [0,5], [10,15], [20,30]
    IntervalList a = {{0.0, 30.0}};
    IntervalList b = {{5.0, 10.0}, {15.0, 20.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 15.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 30.0);
}

TEST(IntervalList, SubtractComplete)
{
    // [0,10] - [0,10] = empty
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{0.0, 10.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}


// ————————————————————————
// 离散化（discretize）
// ————————————————————————

TEST(IntervalList, DiscretizeEmpty)
{
    IntervalList list;
    TimePoint epoch{};
    TimeList tl = list.discretize(epoch, 1.0);

    EXPECT_TRUE(tl.empty());
}

TEST(IntervalList, DiscretizeZeroStep)
{
    IntervalList list = {{0.0, 10.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 0.0);

    EXPECT_TRUE(tl.empty());
}

TEST(IntervalList, DiscretizeBasic)
{
    // [0, 10] 步长 2.0 → 闭区间含端点: {0, 2, 4, 6, 8, 10}
    IntervalList list = {{0.0, 10.0}};
    TimePoint epoch;

    TimeList tl = list.discretize(epoch, 2.0);
    ASSERT_EQ(tl.size(), 6u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 2.0);
    EXPECT_DOUBLE_EQ(secs[2], 4.0);
    EXPECT_DOUBLE_EQ(secs[3], 6.0);
    EXPECT_DOUBLE_EQ(secs[4], 8.0);
    EXPECT_DOUBLE_EQ(secs[5], 10.0);

    EXPECT_EQ(tl.epoch().integerPart(), epoch.integerPart());
}

TEST(IntervalList, DiscretizeMultipleIntervals)
{
    // [0, 2] ∪ [5, 7]，步长 1.0 → {0, 1, 2, 5, 6, 7}
    IntervalList list = {
        {0.0, 2.0},
        {5.0, 7.0},
    };
    TimePoint epoch;

    TimeList tl = list.discretize(epoch, 1.0);
    ASSERT_EQ(tl.size(), 6u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 1.0);
    EXPECT_DOUBLE_EQ(secs[2], 2.0);
    EXPECT_DOUBLE_EQ(secs[3], 5.0);
    EXPECT_DOUBLE_EQ(secs[4], 6.0);
    EXPECT_DOUBLE_EQ(secs[5], 7.0);
}

TEST(IntervalList, DiscretizeOriginNotAffected)
{
    IntervalList list = {
        {0.0, 10.0},
        {20.0, 30.0},
        {5.0, 15.0},  // 重叠区间
    };

    IntervalList copy = list;
    TimePoint epoch;
    list.discretize(epoch, 1.0);

    // discretize 不应修改原列表
    EXPECT_EQ(list.size(), copy.size());
    for (size_t i = 0; i < list.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(list[i].start_, copy[i].start_);
        EXPECT_DOUBLE_EQ(list[i].stop_, copy[i].stop_);
    }
}


// ————————————————————————
// 拷贝与移动
// ————————————————————————

TEST(IntervalList, CopyConstruction)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = a;

    EXPECT_EQ(b.size(), 2u);
    EXPECT_DOUBLE_EQ(b[0].start_, a[0].start_);
    EXPECT_DOUBLE_EQ(b[1].stop_, a[1].stop_);
}

TEST(IntervalList, CopyAssignment)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{100.0, 200.0}};

    b = a;
    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(b[0].stop_, 10.0);
}

TEST(IntervalList, MoveConstruction)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = std::move(a);

    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start_, 0.0);
}

TEST(IntervalList, MoveAssignment)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{100.0, 200.0}};

    b = std::move(a);
    EXPECT_EQ(b.size(), 1u);
    EXPECT_DOUBLE_EQ(b[0].start_, 0.0);
}

TEST(IntervalList, MoveConstructionLeavesSourceEmpty)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    size_t cap = a.intervals().capacity();

    IntervalList b = std::move(a);
    // 移动后源对象应为空（std::vector 移动语义保证）
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.size(), 0u);
    // 目标对象接管资源
    EXPECT_EQ(b.size(), 2u);
    EXPECT_DOUBLE_EQ(b[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(b[1].start_, 20.0);
}

TEST(IntervalList, MoveAssignmentLeavesSourceEmpty)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{100.0, 200.0}};

    b = std::move(a);
    // 移动赋值后源对象应为空
    EXPECT_TRUE(a.empty());
    EXPECT_EQ(a.size(), 0u);
    // 目标对象接管资源
    EXPECT_EQ(b.size(), 2u);
    EXPECT_DOUBLE_EQ(b[0].start_, 0.0);
}

// ————————————————————————
// 构造边界情况
// ————————————————————————

TEST(IntervalList, EmptyInitializerList)
{
    IntervalList list = {};
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0u);
}

TEST(IntervalList, ZeroDurationInterval)
{
    // 零长度区间（start == stop）
    IntervalList list = {{5.0, 5.0}};
    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].duration(), 0.0);
    EXPECT_DOUBLE_EQ(list.totalDuration(), 0.0);
}

TEST(IntervalList, NegativeTimeValues)
{
    // 负时间值（在相对时间系统中是合法的）
    IntervalList list = {
        {-30.0, -20.0},
        {-10.0, 0.0},
    };
    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start_, -30.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, -20.0);
    EXPECT_DOUBLE_EQ(list[0].duration(), 10.0);
    EXPECT_DOUBLE_EQ(list.totalDuration(), 20.0);
}

TEST(IntervalList, LargeValues)
{
    // 大数值区间
    IntervalList list = {
        {0.0, 1e12},
        {2e12, 3e12},
    };
    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list.totalDuration(), 2e12);
}

TEST(IntervalList, ReversedInterval)
{
    // stop < start 的反向区间（数据层面允许存储）
    IntervalList list = {{10.0, 0.0}};
    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start_, 10.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 0.0);
    // duration 为负
    EXPECT_LT(list[0].duration(), 0.0);
}


// ————————————————————————
// 元素访问边界
// ————————————————————————

TEST(IntervalList, AtOnEmpty)
{
    IntervalList list;
    EXPECT_THROW(list.at(0), std::out_of_range);
}

TEST(IntervalList, ConstIntervalsAccessor)
{
    const IntervalList list = {{0.0, 1.0}, {2.0, 3.0}};
    const auto& ivs = list.intervals();
    EXPECT_EQ(ivs.size(), 2u);
    EXPECT_DOUBLE_EQ(ivs[0].start_, 0.0);
}

TEST(IntervalList, SelfAssignment)
{
    IntervalList list = {{0.0, 10.0}, {20.0, 30.0}};
    list = list;  // 自赋值
    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[1].stop_, 30.0);
}


// ————————————————————————
// Contains 边界情况
// ————————————————————————

TEST(IntervalList, ContainsZeroLengthInterval)
{
    // 零长度区间 [5,5]：只有 t=5 被认为在区间内
    IntervalList list = {{5.0, 5.0}};
    EXPECT_TRUE(list.contains(5.0));
    EXPECT_FALSE(list.contains(4.999));
    EXPECT_FALSE(list.contains(5.001));
}

TEST(IntervalList, ContainsJustOutsideBoundary)
{
    IntervalList list = {{0.0, 10.0}};
    // 刚好在边界外的值
    EXPECT_FALSE(list.contains(-1e-15));
    EXPECT_FALSE(list.contains(10.0 + 1e-15));
}

TEST(IntervalList, ContainsWithNegativeIntervals)
{
    IntervalList list = {{-10.0, -5.0}, {5.0, 10.0}};
    EXPECT_TRUE(list.contains(-7.0));
    EXPECT_TRUE(list.contains(-10.0));
    EXPECT_TRUE(list.contains(-5.0));
    EXPECT_TRUE(list.contains(7.0));
    EXPECT_FALSE(list.contains(0.0));   // 在间隙中
    EXPECT_FALSE(list.contains(-1.0));  // 在间隙中
}

TEST(IntervalList, ContainsReversedInterval)
{
    // 反向区间 [10, 0]，contains 检查 t >= 10 && t <= 0，永远为 false
    IntervalList list = {{10.0, 0.0}};
    EXPECT_FALSE(list.contains(5.0));
    EXPECT_FALSE(list.contains(10.0));
    EXPECT_FALSE(list.contains(0.0));
}


// ————————————————————————
// Merged 边界情况
// ————————————————————————

TEST(IntervalList, MergedAllIdentical)
{
    IntervalList list = {
        {0.0, 10.0},
        {0.0, 10.0},
        {0.0, 10.0},
    };
    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, MergedAllIntoOne)
{
    // [0,5], [3,8], [7,12] → [0,12]
    IntervalList list = {
        {0.0, 5.0},
        {3.0, 8.0},
        {7.0, 12.0},
    };
    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 12.0);
}

TEST(IntervalList, MergedAllAdjacent)
{
    // 全部相邻但不重叠
    IntervalList list = {
        {0.0, 10.0},
        {10.0, 20.0},
        {20.0, 30.0},
        {30.0, 40.0},
    };
    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 40.0);
}

TEST(IntervalList, MergedWithZeroDurationIntervals)
{
    // 零长度区间不应影响合并结果
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 5.0},    // 零长度，在 [0,10] 内
        {15.0, 15.0},  // 零长度，孤立
        {20.0, 30.0},
    };
    IntervalList result = list.merged();
    // [0,10], [15,15], [20,30] — 零长度不与其他合并
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 15.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 30.0);
}

TEST(IntervalList, MergedNegativeIntervals)
{
    IntervalList list = {
        {-30.0, -20.0},
        {-25.0, -10.0},
        {-5.0, 0.0},
    };
    IntervalList result = list.merged();
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, -30.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, -10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, -5.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 0.0);
}

TEST(IntervalList, MergedDoesNotModifyOriginal)
{
    IntervalList list = {
        {20.0, 30.0},
        {0.0, 10.0},
    };
    IntervalList copy = list;
    list.merged();
    // 原列表不应改变
    EXPECT_EQ(list.size(), copy.size());
    EXPECT_DOUBLE_EQ(list[0].start_, copy[0].start_);
    EXPECT_DOUBLE_EQ(list[0].stop_, copy[0].stop_);
    EXPECT_DOUBLE_EQ(list[1].start_, copy[1].start_);
    EXPECT_DOUBLE_EQ(list[1].stop_, copy[1].stop_);
}


// ————————————————————————
// 原地合并（mergeInPlace）
// ————————————————————————

TEST(IntervalList, MergeInPlaceEmpty)
{
    IntervalList list;
    list.mergeInPlace();
    EXPECT_TRUE(list.empty());
}

TEST(IntervalList, MergeInPlaceSingle)
{
    IntervalList list = {{0.0, 10.0}};
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 10.0);
}

TEST(IntervalList, MergeInPlaceNonOverlapping)
{
    IntervalList list = {
        {20.0, 30.0},
        {0.0, 10.0},   // 无序
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(list[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(list[1].stop_, 30.0);
}

TEST(IntervalList, MergeInPlaceOverlapping)
{
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 15.0},
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 15.0);
}

TEST(IntervalList, MergeInPlaceAdjacent)
{
    IntervalList list = {
        {0.0, 10.0},
        {10.0, 20.0},
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 20.0);
}

TEST(IntervalList, MergeInPlaceComplex)
{
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 15.0},
        {20.0, 30.0},
        {25.0, 35.0},
        {40.0, 50.0},
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 3u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 15.0);
    EXPECT_DOUBLE_EQ(list[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(list[1].stop_, 35.0);
    EXPECT_DOUBLE_EQ(list[2].start_, 40.0);
    EXPECT_DOUBLE_EQ(list[2].stop_, 50.0);
}

TEST(IntervalList, MergeInPlaceContainment)
{
    IntervalList list = {
        {0.0, 100.0},
        {20.0, 30.0},
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 1u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 100.0);
}

TEST(IntervalList, MergeInPlaceIdempotent)
{
    IntervalList list = {
        {20.0, 30.0},
        {0.0, 10.0},
    };
    list.mergeInPlace();
    // 再次调用应不变
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 2u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(list[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(list[1].stop_, 30.0);
}

TEST(IntervalList, MergeInPlaceWithZeroDuration)
{
    IntervalList list = {
        {0.0, 10.0},
        {5.0, 5.0},
        {15.0, 15.0},
        {20.0, 30.0},
    };
    list.mergeInPlace();

    EXPECT_EQ(list.size(), 3u);
    EXPECT_DOUBLE_EQ(list[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(list[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(list[1].start_, 15.0);
    EXPECT_DOUBLE_EQ(list[1].stop_, 15.0);
    EXPECT_DOUBLE_EQ(list[2].start_, 20.0);
    EXPECT_DOUBLE_EQ(list[2].stop_, 30.0);
}


// ————————————————————————
// Intersect 边界情况
// ————————————————————————

TEST(IntervalList, IntersectBothEmpty)
{
    IntervalList a;
    IntervalList b;
    IntervalList result = a.intersected(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, IntersectSelf)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList result = a.intersected(a);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
}

TEST(IntervalList, IntersectTouchingAtPoint)
{
    // 仅在边界点接触 [0,10] ∩ [10,20] → 点区间 [10,10]（start <= stop，isPoint 非空）
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{10.0, 20.0}};
    IntervalList result = a.intersected(b);
    ASSERT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_TRUE(result[0].isPoint());
    // intersects 与 intersected 一致：相切算相交
    EXPECT_TRUE(a.intersects(b));
}

TEST(IntervalList, IntersectComplex)
{
    // a: [0,10], [20,30], [40,50]
    // b: [5,25], [35,45]
    // 交集: [5,10], [20,25], [40,45]
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}, {40.0, 50.0}};
    IntervalList b = {{5.0, 25.0}, {35.0, 45.0}};
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 25.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 40.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 45.0);
}

TEST(IntervalList, IntersectOneSpansMultiple)
{
    // a: [0,100]
    // b: [10,20], [30,40], [50,60]
    IntervalList a = {{0.0, 100.0}};
    IntervalList b = {{10.0, 20.0}, {30.0, 40.0}, {50.0, 60.0}};
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 30.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 40.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 50.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 60.0);
}

TEST(IntervalList, IntersectedDoesNotModifyOperands)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 25.0}};
    IntervalList aCopy = a;
    IntervalList bCopy = b;

    a.intersected(b);

    // 副本方法不应改变操作数
    EXPECT_EQ(a.size(), aCopy.size());
    EXPECT_EQ(b.size(), bCopy.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(a[i].start_, aCopy[i].start_);
        EXPECT_DOUBLE_EQ(a[i].stop_, aCopy[i].stop_);
    }
}

TEST(IntervalList, IntersectInPlace)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 25.0}};

    IntervalList& ref = a.intersect(b);

    // 原地修改：a 变为交集 [5,10], [20,25]
    EXPECT_EQ(&ref, &a);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_DOUBLE_EQ(a[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(a[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(a[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(a[1].stop_, 25.0);
}

TEST(IntervalList, IntersectUnsortedInputs)
{
    // 无序但无重叠的输入，结果应与有序无异
    IntervalList a = {{20.0, 30.0}, {0.0, 10.0}};  // 无序
    IntervalList b = {{15.0, 25.0}};                 // 有序
    // merged(a)=[0,10],[20,30] ∩ [15,25] = [20,25]
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 25.0);
}

TEST(IntervalList, IntersectOverlappingInputs)
{
    // 输入有内部重叠（未预合并）
    IntervalList a = {{0.0, 10.0}, {5.0, 15.0}};     // 有重叠
    IntervalList b = {{12.0, 20.0}};
    // merged(a)=[0,15] ∩ [12,20] = [12,15]
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 12.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}

TEST(IntervalList, IntersectBothUnsortedAndOverlapping)
{
    // 双方都无序且有重叠
    IntervalList a = {{10.0, 20.0}, {0.0, 8.0}, {5.0, 12.0}};  // 无序+重叠
    IntervalList b = {{25.0, 35.0}, {3.0, 15.0}};               // 无序+重叠
    // merged(a)=[0,20] ∩ merged(b)=[3,15],[25,35] = [3,15]
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 3.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}


// ————————————————————————
// Unite 边界情况
// ————————————————————————

TEST(IntervalList, UniteBothEmpty)
{
    IntervalList a;
    IntervalList b;
    IntervalList result = a.united(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, UniteSelf)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList result = a.united(a);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
}

TEST(IntervalList, UniteComplex)
{
    // a: [0,10], [20,30]
    // b: [5,15], [25,35]
    // 并集: [0,15], [20,35]
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 15.0}, {25.0, 35.0}};
    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 35.0);
}

TEST(IntervalList, UniteAllOverlapping)
{
    // 所有区间都重叠，最终合并为一个
    IntervalList a = {{0.0, 50.0}};
    IntervalList b = {{10.0, 60.0}, {40.0, 100.0}};
    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 100.0);
}

TEST(IntervalList, UnitedDoesNotModifyOperands)
{
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{5.0, 15.0}};
    IntervalList aCopy = a;
    IntervalList bCopy = b;

    a.united(b);

    // 副本方法不应改变操作数
    EXPECT_EQ(a.size(), aCopy.size());
    EXPECT_EQ(b.size(), bCopy.size());
    EXPECT_DOUBLE_EQ(a[0].start_, aCopy[0].start_);
    EXPECT_DOUBLE_EQ(a[0].stop_, aCopy[0].stop_);
}

TEST(IntervalList, UniteInPlace)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 15.0}, {25.0, 35.0}};

    IntervalList& ref = a.unite(b);

    // 原地修改：a 变为并集 [0,15], [20,35]
    EXPECT_EQ(&ref, &a);
    EXPECT_EQ(a.size(), 2u);
    EXPECT_DOUBLE_EQ(a[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(a[0].stop_, 15.0);
    EXPECT_DOUBLE_EQ(a[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(a[1].stop_, 35.0);
}

TEST(IntervalList, UniteUnsortedInputs)
{
    // 双方都无序
    IntervalList a = {{20.0, 30.0}, {0.0, 10.0}};     // 无序
    IntervalList b = {{35.0, 40.0}, {10.0, 15.0}};     // 无序
    // 并: [0,10], [10,15], [20,30], [35,40] → merged → [0,15], [20,30], [35,40]
    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 35.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 40.0);
}

TEST(IntervalList, UniteOverlappingInputs)
{
    // 双方各自内部有重叠
    IntervalList a = {{0.0, 10.0}, {5.0, 20.0}};       // 内部重叠 → [0,20]
    IntervalList b = {{15.0, 25.0}, {22.0, 35.0}};      // 内部重叠 → [15,35]
    // 并: [0,20] ∪ [15,35] → [0,35]
    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 35.0);
}


// ————————————————————————
// Subtract 边界情况
// ————————————————————————

TEST(IntervalList, SubtractFromEmpty)
{
    IntervalList a;
    IntervalList b = {{0.0, 10.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, SubtractOvershootBothSides)
{
    // [5,10] - [0,20] = empty
    IntervalList a = {{5.0, 10.0}};
    IntervalList b = {{0.0, 20.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalList, SubtractExactBoundary)
{
    // [0,10] - [0,5] = [5,10]（b 与 a 左边界完全对齐）
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{0.0, 5.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
}

TEST(IntervalList, SubtractBSpanningGaps)
{
    // a: [0,10], [20,30], [40,50]
    // b: [5,45]  — 跨过 a 的间隙
    // 结果: [0,5], [45,50]
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}, {40.0, 50.0}};
    IntervalList b = {{5.0, 45.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 45.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 50.0);
}

TEST(IntervalList, SubtractWithZeroResult)
{
    // 逐段扣除后某一段恰好被完全消除
    // a: [0,10], [10,20] → merged → [0,20]
    // b: [5,15]
    // 结果: [0,5], [15,20]
    IntervalList a = {{0.0, 10.0}, {10.0, 20.0}};
    IntervalList b = {{5.0, 15.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 20.0);
}

TEST(IntervalList, SubtractMultipleBInsideOneA)
{
    // a: [0,100]
    // b: [10,20], [40,50], [80,90]
    // 结果: [0,10], [20,40], [50,80], [90,100]
    IntervalList a = {{0.0, 100.0}};
    IntervalList b = {{10.0, 20.0}, {40.0, 50.0}, {80.0, 90.0}};
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 4u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 40.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 50.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 80.0);
    EXPECT_DOUBLE_EQ(result[3].start_, 90.0);
    EXPECT_DOUBLE_EQ(result[3].stop_, 100.0);
}

TEST(IntervalList, SubtractedDoesNotModifyOperands)
{
    IntervalList a = {{0.0, 30.0}};
    IntervalList b = {{5.0, 10.0}, {15.0, 20.0}};
    IntervalList aCopy = a;
    IntervalList bCopy = b;

    a.subtracted(b);

    // 副本方法不应改变操作数
    EXPECT_EQ(a.size(), aCopy.size());
    EXPECT_EQ(b.size(), bCopy.size());
    EXPECT_DOUBLE_EQ(a[0].start_, aCopy[0].start_);
    EXPECT_DOUBLE_EQ(b[0].start_, bCopy[0].start_);
    EXPECT_DOUBLE_EQ(b[1].start_, bCopy[1].start_);
}

TEST(IntervalList, SubtractInPlace)
{
    IntervalList a = {{0.0, 30.0}};
    IntervalList b = {{5.0, 10.0}, {15.0, 20.0}};

    IntervalList& ref = a.subtract(b);

    // 原地修改：a 变为差集 [0,5], [10,15], [20,30]
    EXPECT_EQ(&ref, &a);
    EXPECT_EQ(a.size(), 3u);
    EXPECT_DOUBLE_EQ(a[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(a[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(a[1].start_, 10.0);
    EXPECT_DOUBLE_EQ(a[1].stop_, 15.0);
    EXPECT_DOUBLE_EQ(a[2].start_, 20.0);
    EXPECT_DOUBLE_EQ(a[2].stop_, 30.0);
}

TEST(IntervalList, SubtractUnsortedInputs)
{
    // a 无序，b 有序 — 结果应与有序时一致
    IntervalList a = {{20.0, 30.0}, {0.0, 10.0}};    // 无序
    IntervalList b = {{5.0, 25.0}};
    // merged(a)=[0,10],[20,30] - [5,25] = [0,5], [25,30]
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 25.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 30.0);
}

TEST(IntervalList, SubtractOverlappingInputA)
{
    // a 内部有重叠，b 有序无重叠
    IntervalList a = {{0.0, 30.0}, {10.0, 40.0}};    // 内部重叠 → merged → [0,40]
    IntervalList b = {{5.0, 10.0}, {20.0, 25.0}};
    // [0,40] - ([5,10] ∪ [20,25]) = [0,5], [10,20], [25,40]
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 20.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 25.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 40.0);
}

TEST(IntervalList, SubtractBothOverlapping)
{
    // 双方都内部重叠
    IntervalList a = {{0.0, 20.0}, {10.0, 40.0}};      // a: [0,40]
    IntervalList b = {{5.0, 15.0}, {12.0, 25.0}};       // b: [5,25]
    // [0,40] - [5,25] = [0,5], [25,40]
    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 25.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 40.0);
}


// ————————————————————————
// 多区间交错运算 — 双方各有多个区间，且交错排列
// ————————————————————————

TEST(IntervalList, IntersectInterleaving)
{
    // a: [0,10], [25,35], [50,60]
    // b: [5,15], [30,40], [55,65]
    // 双方各有3个区间，部分交错
    // 结果: [5,10], [30,35], [55,60] — 各对区间重叠部分
    IntervalList a = {{0.0, 10.0}, {25.0, 35.0}, {50.0, 60.0}};
    IntervalList b = {{5.0, 15.0}, {30.0, 40.0}, {55.0, 65.0}};

    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 30.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 35.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 55.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 60.0);
}

TEST(IntervalList, IntersectInterleavingWithGaps)
{
    // a: [0,15], [30,50]
    // b: [5,10], [20,25], [35,45]
    // b 的区间 [20,25] 与 a 无交集 → 被跳过
    // 结果: [5,10], [35,45]
    IntervalList a = {{0.0, 15.0}, {30.0, 50.0}};
    IntervalList b = {{5.0, 10.0}, {20.0, 25.0}, {35.0, 45.0}};

    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 35.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 45.0);
}

TEST(IntervalList, UniteInterleaving)
{
    // a: [0,10], [30,40], [60,70]
    // b: [15,25], [45,55]
    // 双方区间交错排列，互不重叠
    // 结果: [0,10], [15,25], [30,40], [45,55], [60,70]
    IntervalList a = {{0.0, 10.0}, {30.0, 40.0}, {60.0, 70.0}};
    IntervalList b = {{15.0, 25.0}, {45.0, 55.0}};

    IntervalList result = a.united(b);
    EXPECT_EQ(result.size(), 5u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 15.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 25.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 30.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 40.0);
    EXPECT_DOUBLE_EQ(result[3].start_, 45.0);
    EXPECT_DOUBLE_EQ(result[3].stop_, 55.0);
    EXPECT_DOUBLE_EQ(result[4].start_, 60.0);
    EXPECT_DOUBLE_EQ(result[4].stop_, 70.0);
}

TEST(IntervalList, SubtractInterleaving)
{
    // a: [0,20], [30,50], [60,80]
    // b: [5,10], [35,40], [65,70]
    // 每个 b 区间落在 a 的一个区间内
    // 结果: [0,5], [10,20], [30,35], [40,50], [60,65], [70,80]
    IntervalList a = {{0.0, 20.0}, {30.0, 50.0}, {60.0, 80.0}};
    IntervalList b = {{5.0, 10.0}, {35.0, 40.0}, {65.0, 70.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 6u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 20.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 30.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 35.0);
    EXPECT_DOUBLE_EQ(result[3].start_, 40.0);
    EXPECT_DOUBLE_EQ(result[3].stop_, 50.0);
    EXPECT_DOUBLE_EQ(result[4].start_, 60.0);
    EXPECT_DOUBLE_EQ(result[4].stop_, 65.0);
    EXPECT_DOUBLE_EQ(result[5].start_, 70.0);
    EXPECT_DOUBLE_EQ(result[5].stop_, 80.0);
}

TEST(IntervalList, SubtractExactEdges)
{
    // subtract 边界情况：切除后某段恰好收缩为零长度（不应产生零长度区间）
    // a: [0,10], [10,20]
    // b: [8,12]
    // merged(a)=[0,20], subtract [8,12] = [0,8], [12,20]
    IntervalList a = {{0.0, 10.0}, {10.0, 20.0}};
    IntervalList b = {{8.0, 12.0}};

    IntervalList result = a.subtracted(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 8.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 12.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 20.0);
    // 不会出现 [10,10] 这样的零长度区间
    for (const auto& iv : result)
    {
        EXPECT_GT(iv.duration(), 0.0);
    }
}


// ————————————————————————
// Discretize 边界情况
// ————————————————————————

TEST(IntervalList, DiscretizeSinglePointInterval)
{
    // 点区间 [5,5] 离散化为单点 {5}
    IntervalList list = {{5.0, 5.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 1.0);
    ASSERT_EQ(tl.size(), 1u);
    EXPECT_DOUBLE_EQ(tl.seconds()[0], 5.0);
}

TEST(IntervalList, DiscretizeStepLargerThanInterval)
{
    // 步长大于区间长度，应至少产生起点和终点
    IntervalList list = {{0.0, 0.5}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 1.0);
    ASSERT_FALSE(tl.empty());
    EXPECT_EQ(tl.size(), 2u);

    EXPECT_DOUBLE_EQ(tl.seconds()[0], 0.0);
    EXPECT_DOUBLE_EQ(tl.seconds()[1], 0.5);
}

TEST(IntervalList, DiscretizeExactDivisible)
{
    // [0, 10] 步长 2.0 → 闭区间含端点: {0, 2, 4, 6, 8, 10}
    IntervalList list = {{0.0, 10.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 2.0);
    ASSERT_EQ(tl.size(), 6u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 2.0);
    EXPECT_DOUBLE_EQ(secs[2], 4.0);
    EXPECT_DOUBLE_EQ(secs[3], 6.0);
    EXPECT_DOUBLE_EQ(secs[4], 8.0);
    EXPECT_DOUBLE_EQ(secs[5], 10.0);
}

TEST(IntervalList, DiscretizeNonIntegerStep)
{
    // [0, 1] 步长 0.3 → 闭区间含端点: {0, 0.3, 0.6, 0.9, 1.0}
    IntervalList list = {{0.0, 1.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 0.3);
    ASSERT_EQ(tl.size(), 5u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 0.3);
    EXPECT_DOUBLE_EQ(secs[2], 0.6);
    EXPECT_DOUBLE_EQ(secs[3], 0.9);
    EXPECT_DOUBLE_EQ(secs[4], 1.0);
}

TEST(IntervalList, DiscretizeStepNotDivisible)
{
    // [0, 10] 步长 3.0 → 不能整除，闭区间含端点: {0, 3, 6, 9, 10}
    IntervalList list = {{0.0, 10.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 3.0);
    ASSERT_EQ(tl.size(), 5u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], 0.0);
    EXPECT_DOUBLE_EQ(secs[1], 3.0);
    EXPECT_DOUBLE_EQ(secs[2], 6.0);
    EXPECT_DOUBLE_EQ(secs[3], 9.0);
    EXPECT_DOUBLE_EQ(secs[4], 10.0);
}

TEST(IntervalList, DiscretizeVerySmallStep)
{
    // 很小的步长，产生大量点 — 验证首尾值
    IntervalList list = {{0.0, 0.1}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 0.001);
    ASSERT_FALSE(tl.empty());
    EXPECT_GT(tl.size(), 10u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs.front(), 0.0);
    EXPECT_DOUBLE_EQ(secs.back(), 0.1);
}

TEST(IntervalList, DiscretizePreservesEpoch)
{
    TimePoint epoch;
    IntervalList list = {{10.0, 20.0}};
    TimeList tl = list.discretize(epoch, 2.0);
    EXPECT_EQ(tl.epoch().integerPart(), epoch.integerPart());

    // 验证秒偏移量也正确
    ASSERT_FALSE(tl.empty());
    EXPECT_DOUBLE_EQ(tl.seconds()[0], 10.0);
}

TEST(IntervalList, DiscretizeNegativeInterval)
{
    // 负区间 [-10, 0] 步长 2.0 → {-10, -8, -6, -4, -2, 0}
    IntervalList list = {{-10.0, 0.0}};
    TimePoint epoch;
    TimeList tl = list.discretize(epoch, 2.0);
    ASSERT_EQ(tl.size(), 6u);

    const auto& secs = tl.seconds();
    EXPECT_DOUBLE_EQ(secs[0], -10.0);
    EXPECT_DOUBLE_EQ(secs[1], -8.0);
    EXPECT_DOUBLE_EQ(secs[2], -6.0);
    EXPECT_DOUBLE_EQ(secs[3], -4.0);
    EXPECT_DOUBLE_EQ(secs[4], -2.0);
    EXPECT_DOUBLE_EQ(secs[5], 0.0);
}


// ————————————————————————
// 链式操作
// ————————————————————————

TEST(IntervalList, ChainedMergedIntersect)
{
    // 先合并再求交
    IntervalList a = {{0.0, 5.0}, {3.0, 10.0}, {12.0, 15.0}};
    // a.merged() = [0,10], [12,15]
    IntervalList b = {{5.0, 13.0}};
    // 交集: [5,10], [12,13]
    IntervalList result = a.merged().intersected(b);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 12.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 13.0);
}

TEST(IntervalList, ChainedUniteSubtract)
{
    // 先合并再求差
    IntervalList a = {{0.0, 10.0}};
    IntervalList b = {{5.0, 15.0}};
    IntervalList c = {{2.0, 3.0}};
    // a.united(b) = [0,15], 再 subtract c → [0,2], [3,15]
    IntervalList result = a.united(b).subtracted(c);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 2.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 3.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 15.0);
}

TEST(IntervalList, ChainedIntersectThenMerged)
{
    // 交后再合并（intersect 本身已合并，链式调用验证幂等性）
    IntervalList a = {{0.0, 8.0}, {5.0, 15.0}};
    IntervalList b = {{7.0, 20.0}};
    // a.intersected(b) 内部先 merged a=[0,15], 再与 b=[7,20] 交 → [7,15]
    IntervalList result = a.intersected(b).merged();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(result[0].start_, 7.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 15.0);
}

TEST(IntervalList, ChainedSubtractAll)
{
    // 连续减去多个集合
    IntervalList a = {{0.0, 100.0}};
    IntervalList b = {{10.0, 20.0}};
    IntervalList c = {{50.0, 60.0}};
    IntervalList d = {{85.0, 95.0}};

    IntervalList result = a.subtracted(b).subtracted(c).subtracted(d);
    EXPECT_EQ(result.size(), 4u);
    EXPECT_DOUBLE_EQ(result[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(result[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(result[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(result[1].stop_, 50.0);
    EXPECT_DOUBLE_EQ(result[2].start_, 60.0);
    EXPECT_DOUBLE_EQ(result[2].stop_, 85.0);
    EXPECT_DOUBLE_EQ(result[3].start_, 95.0);
    EXPECT_DOUBLE_EQ(result[3].stop_, 100.0);
}


// ————————————————————————
// 大型列表与压力测试
// ————————————————————————

TEST(IntervalList, LargeNumberOfIntervals)
{
    // 构造大量不重叠区间
    IntervalList list;
    const size_t N = 1000;
    for (size_t i = 0; i < N; ++i)
    {
        double t = static_cast<double>(i);
        list.push_back(t * 100.0, t * 100.0 + 50.0);
    }
    EXPECT_EQ(list.size(), N);
    EXPECT_DOUBLE_EQ(list.totalDuration(), N * 50.0);

    // 合并不应改变结果（不重叠）
    IntervalList merged = list.merged();
    EXPECT_EQ(merged.size(), N);
}

TEST(IntervalList, LargeNumberOfOverlappingIntervals)
{
    // 大量重叠区间 → 合并为一个
    IntervalList list;
    const size_t N = 500;
    for (size_t i = 0; i < N; ++i)
    {
        double t = static_cast<double>(i);
        list.push_back(t, t + 100.0);
    }
    EXPECT_EQ(list.size(), N);

    IntervalList merged = list.merged();
    EXPECT_EQ(merged.size(), 1u);
    EXPECT_DOUBLE_EQ(merged[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(merged[0].stop_, N - 1.0 + 100.0);
}

TEST(IntervalList, ManyIntervalsIntersect)
{
    // 两个大型列表求交
    IntervalList a;
    IntervalList b;
    const size_t N = 200;
    for (size_t i = 0; i < N; ++i)
    {
        double t = static_cast<double>(i);
        a.push_back(t * 10.0, t * 10.0 + 5.0);
        b.push_back(t * 10.0 + 2.0, t * 10.0 + 7.0);
    }
    // 每对区间有 3 秒重叠
    IntervalList result = a.intersected(b);
    EXPECT_EQ(result.size(), N);
    for (size_t i = 0; i < N; ++i)
    {
        EXPECT_DOUBLE_EQ(result[i].start_, i * 10.0 + 2.0);
        EXPECT_DOUBLE_EQ(result[i].stop_, i * 10.0 + 5.0);
    }
}


TEST(IntervalList, OperatorOverloads)
{
    IntervalList a = {{0.0, 10.0}, {20.0, 30.0}};
    IntervalList b = {{5.0, 25.0}};

    // 副本运算符
    IntervalList inter = a & b;   // [5,10], [20,25]
    IntervalList uni  = a | b;    // [0,30]
    IntervalList diff = a - b;    // [0,5], [25,30]

    EXPECT_EQ(inter.size(), 2u);
    EXPECT_DOUBLE_EQ(inter[0].start_, 5.0);
    EXPECT_DOUBLE_EQ(inter[0].stop_, 10.0);
    EXPECT_DOUBLE_EQ(inter[1].start_, 20.0);
    EXPECT_DOUBLE_EQ(inter[1].stop_, 25.0);

    EXPECT_EQ(uni.size(), 1u);
    EXPECT_DOUBLE_EQ(uni[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(uni[0].stop_, 30.0);

    EXPECT_EQ(diff.size(), 2u);
    EXPECT_DOUBLE_EQ(diff[0].start_, 0.0);
    EXPECT_DOUBLE_EQ(diff[0].stop_, 5.0);
    EXPECT_DOUBLE_EQ(diff[1].start_, 25.0);
    EXPECT_DOUBLE_EQ(diff[1].stop_, 30.0);

    // 副本运算符不改操作数
    EXPECT_EQ(a.size(), 2u);
    EXPECT_EQ(b.size(), 1u);

    // 原地运算符
    IntervalList c = a;
    c &= b;
    EXPECT_EQ(c.size(), 2u);   // [5,10], [20,25]

    c = a;
    c |= b;
    EXPECT_EQ(c.size(), 1u);   // [0,30]

    c = a;
    c -= b;
    EXPECT_EQ(c.size(), 2u);   // [0,5], [25,30]
}


GTEST_MAIN()
