///
/// @file      testInterval.cpp
/// @brief     相对时间区间测试
/// @details   测试Interval类的构造、集合运算和离散化功能
/// @author    axel
/// @date      2026-08-24
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

#include "ast/Interval.hpp"
#include "ast/Test.h"
#include <limits>

AST_USING_NAMESPACE

TEST(Interval, Discretize)
{
    // 时长正好是步长整数倍
    {
        Interval interval{0.0, 3600.0};
        auto range = interval.discretize(1800.0);  // 30分钟步长

        // nnodes = ceil(3600/1800) + 1 = 3
        EXPECT_EQ(range.size(), 3u);
        auto it = range.begin();
        EXPECT_NEAR(*it, 0.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 1800.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 3600.0, 1e-9);
    }

    {
        Interval interval{0.0, 5400.0};  // 1.5小时
        auto range = interval.discretize(1800.0);

        // nnodes = ceil(5400/1800) + 1 = 4
        EXPECT_EQ(range.size(), 4u);
        auto it = range.begin();
        EXPECT_NEAR(*it, 0.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 1800.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 3600.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 5400.0, 1e-9);
    }

    // 单步长刚好覆盖整个区间
    {
        Interval interval{0.0, 600.0};
        auto range = interval.discretize(600.0);

        // nnodes = ceil(600/600) + 1 = 2
        EXPECT_EQ(range.size(), 2u);
    }

    // 相对起点不为 0 的情况
    {
        Interval interval{100.0, 3700.0};
        auto range = interval.discretize(1800.0);
        EXPECT_EQ(range.size(), 3u);

        auto it = range.begin();
        EXPECT_NEAR(*it, 100.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 1900.0, 1e-9); ++it;
        EXPECT_NEAR(*it, 3700.0, 1e-9);
    }

    // 无效步长 → 空范围
    {
        Interval interval{0.0, 3600.0};
        EXPECT_EQ(interval.discretize(0.0).size(), 0u);
        EXPECT_EQ(interval.discretize(-1.0).size(), 0u);
    }

    // 点区间（start == stop）→ 单节点
    {
        Interval interval{5.0, 5.0};
        EXPECT_EQ(interval.discretize(600.0).size(), 1u);
    }
}

TEST(Interval, DiscretizedCount)
{
    // 时长正好是步长整数倍
    {
        Interval interval{0.0, 3600.0};
        EXPECT_EQ(interval.discretizedCount(1800.0), 3u);
    }

    // 时长不是步长整数倍
    {
        Interval interval{0.0, 5400.0};
        EXPECT_EQ(interval.discretizedCount(1800.0), 4u);
    }

    // 单步长刚好覆盖整个区间
    {
        Interval interval{0.0, 600.0};
        EXPECT_EQ(interval.discretizedCount(600.0), 2u);
    }

    // 无效步长 → 0
    {
        Interval interval{0.0, 3600.0};
        EXPECT_EQ(interval.discretizedCount(0.0), 0u);
        EXPECT_EQ(interval.discretizedCount(-1.0), 0u);
    }

    // 点区间（start == stop）→ 1
    {
        Interval interval{5.0, 5.0};
        EXPECT_EQ(interval.discretizedCount(600.0), 1u);
    }

    // 数量与 discretize 产出的 range 一致
    {
        Interval interval{0.0, 7200.0};
        auto range = interval.discretize(1800.0);
        EXPECT_EQ(interval.discretizedCount(1800.0), range.size());
    }
}

TEST(Interval, DiscretizeIterator)
{
    // 验证迭代器遍历与解引用
    Interval interval{0.0, 3600.0};
    auto range = interval.discretize(1800.0);

    size_t count = 0;
    double first = 0.0, last = 0.0;
    for (auto it = range.begin(); it != range.end(); ++it) {
        if (count == 0) first = *it;
        last = *it;
        count++;
    }

    // nnodes = ceil(3600/1800) + 1 = 3
    EXPECT_EQ(count, 3u);
    EXPECT_NEAR(first, 0.0, 1e-9);
    EXPECT_NEAR(last, 3600.0, 1e-9);
}

TEST(Interval, Predicates)
{
    // 有效区间
    Interval valid{0.0, 10.0};
    EXPECT_TRUE(valid.isValid());
    EXPECT_TRUE(valid.isNonEmpty());
    EXPECT_FALSE(valid.isEmpty());
    EXPECT_FALSE(valid.isPoint());
    EXPECT_FALSE(valid.isDegenerate());

    // 点区间（start == stop，非空，含单瞬时）
    Interval point{5.0, 5.0};
    EXPECT_TRUE(point.isValid());
    EXPECT_TRUE(point.isNonEmpty());
    EXPECT_FALSE(point.isEmpty());
    EXPECT_TRUE(point.isPoint());
    EXPECT_TRUE(point.isDegenerate());
    EXPECT_TRUE(point.contains(5.0));
    EXPECT_FALSE(point.contains(4.999));

    // 反向区间（start > stop，空）
    Interval reversed{10.0, 0.0};
    EXPECT_FALSE(reversed.isValid());
    EXPECT_FALSE(reversed.isNonEmpty());
    EXPECT_TRUE(reversed.isEmpty());
    EXPECT_FALSE(reversed.isPoint());
    EXPECT_FALSE(reversed.contains(5.0));
    EXPECT_FALSE(reversed.contains(0.0));

    // 规范空哨兵 {+∞, -∞}
    Interval empty = Interval::Empty();
    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(empty.isValid());
    EXPECT_FALSE(empty.isPoint());
    EXPECT_FALSE(empty.contains(0.0));
    // 空哨兵与任意有效区间并集 = 该区间（空为并集单位元）
    EXPECT_DOUBLE_EQ(empty.united(valid).start(), 0.0);
    EXPECT_DOUBLE_EQ(empty.united(valid).stop(), 10.0);

    // NaN 边界（部分序）→ 视为空
    Interval nanInterval{std::numeric_limits<double>::quiet_NaN(), 5.0};
    EXPECT_TRUE(nanInterval.isEmpty());
    EXPECT_FALSE(nanInterval.isValid());
    EXPECT_FALSE(nanInterval.isPoint());
    EXPECT_FALSE(nanInterval.contains(3.0));
}

TEST(Interval, Intersect)
{
    // 重叠 → 有效交集
    {
        Interval a{0.0, 10.0}, b{5.0, 15.0};
        auto r = a.intersected(b);
        EXPECT_FALSE(r.isEmpty());
        EXPECT_DOUBLE_EQ(r.start(), 5.0);
        EXPECT_DOUBLE_EQ(r.stop(), 10.0);
        EXPECT_TRUE(a.intersects(b));
    }

    // 相切 → 点区间（非空），与 intersects 一致
    {
        Interval a{0.0, 10.0}, b{10.0, 20.0};
        auto r = a.intersected(b);
        EXPECT_FALSE(r.isEmpty());
        EXPECT_TRUE(r.isPoint());
        EXPECT_DOUBLE_EQ(r.start(), 10.0);
        EXPECT_TRUE(a.intersects(b));
    }

    // 不相交 → 规范空区间；与 intersects 一致
    {
        Interval a{0.0, 10.0}, b{20.0, 30.0};
        auto r = a.intersected(b);
        EXPECT_TRUE(r.isEmpty());
        EXPECT_FALSE(a.intersects(b));
    }

    // 原地 intersect
    {
        Interval a{0.0, 10.0};
        a.intersect(Interval{20.0, 30.0});
        EXPECT_TRUE(a.isEmpty());
    }

    // NaN 边界（部分序）→ 交集为空，与 intersects 一致
    {
        Interval a{0.0, 10.0};
        Interval nanInterval{std::numeric_limits<double>::quiet_NaN(), 5.0};
        EXPECT_TRUE(a.intersected(nanInterval).isEmpty());
        EXPECT_FALSE(a.intersects(nanInterval));
        a.intersect(nanInterval);
        EXPECT_TRUE(a.isEmpty());
    }
}

GTEST_MAIN()
