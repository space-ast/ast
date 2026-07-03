///
/// @file      IntervalList.cpp
/// @brief     相对时段列表实现
/// @details   ~
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

#include "IntervalList.hpp"
#include "TimeList.hpp"
#include "TimeInterval.hpp"
#include <algorithm>

AST_NAMESPACE_BEGIN


// ————————————————————————
// 工厂方法
// ————————————————————————

IntervalList IntervalList::FromIntervals(const std::vector<Interval>& intervals)
{
    IntervalList result;
    result.intervals_ = intervals;
    return result;
}


// ————————————————————————
// 集合运算
// ————————————————————————

IntervalList IntervalList::merged() const
{
    if (intervals_.empty())
    {
        return IntervalList();
    }

    // 复制并排序
    std::vector<Interval> sorted = intervals_;
    std::sort(sorted.begin(), sorted.end(),
              [](const Interval& a, const Interval& b) {
                  return a.start_ < b.start_;
              });

    IntervalList result;
    result.intervals_.push_back(sorted[0]);

    for (size_t i = 1; i < sorted.size(); ++i)
    {
        Interval& last = result.intervals_.back();
        const Interval& curr = sorted[i];

        if (curr.start_ <= last.stop_)
        {
            // 重叠或相邻，合并
            if (curr.stop_ > last.stop_)
            {
                last.stop_ = curr.stop_;
            }
        }
        else
        {
            // 不重叠，追加新区间
            result.intervals_.push_back(curr);
        }
    }

    return result;
}

IntervalList IntervalList::intersect(const IntervalList& other) const
{
    IntervalList a = this->merged();
    IntervalList b = other.merged();

    IntervalList result;

    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size())
    {
        double start = std::max(a[i].start_, b[j].start_);
        double stop  = std::min(a[i].stop_, b[j].stop_);

        if (start < stop)
        {
            result.push_back(start, stop);
        }

        // 移动 stop 较小的指针
        if (a[i].stop_ < b[j].stop_)
        {
            ++i;
        }
        else
        {
            ++j;
        }
    }

    return result;
}

IntervalList IntervalList::unite(const IntervalList& other) const
{
    IntervalList result;
    result.intervals_.reserve(intervals_.size() + other.intervals_.size());
    result.intervals_.insert(result.intervals_.end(), intervals_.begin(), intervals_.end());
    result.intervals_.insert(result.intervals_.end(), other.intervals_.begin(), other.intervals_.end());
    return result.merged();
}

IntervalList IntervalList::subtract(const IntervalList& other) const
{
    IntervalList a = this->merged();
    IntervalList b = other.merged();

    IntervalList result;

    size_t j = 0;
    for (size_t i = 0; i < a.size(); ++i)
    {
        double remainingStart = a[i].start_;
        double remainingStop  = a[i].stop_;

        // 跳过在 a[i] 之前结束的 b 区间
        while (j < b.size() && b[j].stop_ <= remainingStart)
        {
            ++j;
        }

        // 用 b 的区间逐段切除
        while (j < b.size() && b[j].start_ < remainingStop)
        {
            if (b[j].start_ > remainingStart)
            {
                // b 的起始在当前剩余段中间，保留前半段
                result.push_back(remainingStart, b[j].start_);
            }

            remainingStart = std::max(remainingStart, b[j].stop_);

            if (remainingStart >= remainingStop)
            {
                break;
            }
            ++j;
        }

        // 保留剩余部分
        if (remainingStart < remainingStop)
        {
            result.push_back(remainingStart, remainingStop);
        }
    }

    return result;
}


// ————————————————————————
// 与 TimeList 互转
// ————————————————————————

TimeList IntervalList::discrete(const TimePoint& epoch, double step) const
{
    TimeList result(epoch);

    if (step <= 0.0 || intervals_.empty())
    {
        return result;
    }

    for (const auto& iv : intervals_)
    {
        TimeInterval ti(epoch, iv.start_, iv.stop_);
        ti.discrete(epoch, step, result.seconds());
    }

    return result;
}


AST_NAMESPACE_END
