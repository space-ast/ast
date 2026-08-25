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
#include <algorithm>
#include <cmath>

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
    IntervalList result = *this;
    result.mergeInPlace();
    return result;
}

void IntervalList::mergeInPlace()
{
    if (intervals_.empty())
    {
        return;
    }

    // 原地排序
    std::sort(intervals_.begin(), intervals_.end(),
              [](const Interval& a, const Interval& b) {
                  return a.start_ < b.start_;
              });

    // 原地合并相邻或重叠的区间
    size_t writeIdx = 0;
    for (size_t i = 1; i < intervals_.size(); ++i)
    {
        if (intervals_[i].start_ <= intervals_[writeIdx].stop_)
        {
            // 重叠或相邻，扩展当前区间
            if (intervals_[i].stop_ > intervals_[writeIdx].stop_)
            {
                intervals_[writeIdx].stop_ = intervals_[i].stop_;
            }
        }
        else
        {
            // 不重叠，写入下一个位置
            ++writeIdx;
            intervals_[writeIdx] = intervals_[i];
        }
    }

    // 截断多余元素
    intervals_.resize(writeIdx + 1);
}

bool IntervalList::intersects(const IntervalList& other) const
{
    for (const auto& a : intervals_)
    {
        for (const auto& b : other.intervals_)
        {
            if (a.intersects(b))
            {
                return true;
            }
        }
    }
    return false;
}

IntervalList IntervalList::intersected(const IntervalList& other) const
{
    IntervalList a = *this;
    a.mergeInPlace();
    IntervalList b = other.merged();

    IntervalList result;
    result.intervals_.reserve(a.size() + b.size());

    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size())
    {
        double start = std::max(a[i].start_, b[j].start_);
        double stop  = std::min(a[i].stop_, b[j].stop_);

        if (start <= stop)   // 相切产生的点区间也是有效瞬时（isPoint）
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

IntervalList IntervalList::united(const IntervalList& other) const
{
    IntervalList result;
    result.intervals_.reserve(intervals_.size() + other.intervals_.size());
    result.intervals_.insert(result.intervals_.end(), intervals_.begin(), intervals_.end());
    result.intervals_.insert(result.intervals_.end(), other.intervals_.begin(), other.intervals_.end());
    result.mergeInPlace();
    return result;
}

IntervalList IntervalList::subtracted(const IntervalList& other) const
{
    IntervalList a = *this;
    a.mergeInPlace();
    IntervalList b = other.merged();

    IntervalList result;
    result.intervals_.reserve(a.size() + b.size());

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


IntervalList& IntervalList::intersect(const IntervalList& other)
{
    *this = intersected(other);
    return *this;
}

IntervalList& IntervalList::unite(const IntervalList& other)
{
    *this = united(other);
    return *this;
}

IntervalList& IntervalList::subtract(const IntervalList& other)
{
    *this = subtracted(other);
    return *this;
}


// ————————————————————————
// 与 TimeList 互转
// ————————————————————————

TimeList IntervalList::discretize(const TimePoint& epoch, double step) const
{
    TimeList result(epoch);

    if (step <= 0.0)
    {
        return result;
    }

    // 先合并重叠区间，避免重叠区域产生重复时间点
    IntervalList mergedList = *this;
    mergedList.mergeInPlace();
    if (mergedList.empty())
    {
        return result;
    }

    for (const auto& iv : mergedList)
    {
        if (iv.isEmpty())
        {
            continue;
        }
        double dur = iv.duration();

        // 直接从 Interval 的相對秒数生成离散时间点，绕过 TimeInterval 中间层
        auto& secs = result.seconds();
        ptrdiff_t nnodes = static_cast<ptrdiff_t>(std::ceil(dur / step) + 1);
        secs.reserve(secs.size() + nnodes);

        for (ptrdiff_t i = 0; i < nnodes - 1; ++i)
        {
            secs.push_back(iv.start_ + i * step);
        }
        secs.push_back(iv.stop_);
    }

    return result;
}


AST_NAMESPACE_END
