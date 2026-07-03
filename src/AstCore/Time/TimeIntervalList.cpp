///
/// @file      TimeIntervalList.cpp
/// @brief     绝对时段列表实现
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

#include "TimeIntervalList.hpp"

AST_NAMESPACE_BEGIN


// ============================================================
// 匿名辅助函数：跨历元转换
// ============================================================
namespace {

/// @brief 将 IntervalList 从 fromEpoch 转换到 toEpoch
/// @param src 源相对区间列表
/// @param fromEpoch 源历元
/// @param toEpoch 目标历元
/// @return 转换后的相对区间列表（相对于 toEpoch）
IntervalList convertEpoch(const IntervalList& src,
                          const TimePoint& fromEpoch,
                          const TimePoint& toEpoch)
{
    if (src.empty())
    {
        return {};
    }

    IntervalList result;
    result.reserve(src.size());

    for (const auto& iv : src)
    {
        TimePoint absStart = fromEpoch.shiftedBySecond(iv.start_);
        TimePoint absStop  = fromEpoch.shiftedBySecond(iv.stop_);
        result.push_back(absStart.durationFrom(toEpoch),
                         absStop.durationFrom(toEpoch));
    }

    return result;
}

} // anonymous namespace


// ============================================================
// 工厂方法
// ============================================================

TimeIntervalList TimeIntervalList::FromTimeIntervals(const std::vector<TimeInterval>& intervals)
{
    if (intervals.empty())
    {
        return {};
    }

    // 以第一个区间的 start 作为 epoch
    TimePoint epoch = intervals[0].start();

    IntervalList rel;
    rel.reserve(intervals.size());

    for (const auto& ti : intervals)
    {
        rel.push_back(ti.start().durationFrom(epoch),
                      ti.stop().durationFrom(epoch));
    }

    TimeIntervalList result;
    result.setEpoch(epoch);
    result.intervals() = std::move(rel);
    return result;
}

TimeIntervalList TimeIntervalList::FromTimeIntervals(const std::vector<TimeInterval>& intervals,
                                                  const TimePoint& epoch)
{
    if (intervals.empty())
    {
        TimeIntervalList result;
        result.setEpoch(epoch);
        return result;
    }

    IntervalList rel;
    rel.reserve(intervals.size());

    for (const auto& ti : intervals)
    {
        rel.push_back(ti.start().durationFrom(epoch),
                      ti.stop().durationFrom(epoch));
    }

    TimeIntervalList result;
    result.setEpoch(epoch);
    result.intervals() = std::move(rel);
    return result;
}

TimeIntervalList TimeIntervalList::FromIntervals(const std::vector<Interval>& intervals,
                                                  const TimePoint& epoch)
{
    TimeIntervalList result;
    result.setEpoch(epoch);
    result.intervals().intervals() = intervals;
    return result;
}

TimeIntervalList TimeIntervalList::FromIntervalList(const IntervalList& list,
                                                     const TimePoint& epoch)
{
    return FromIntervals(list.intervals(), epoch);
}


// ============================================================
// 元素访问
// ============================================================

TimeInterval TimeIntervalList::operator[](size_t i) const
{
    const Interval& iv = intervals_[i];
    return TimeInterval(epoch_.shiftedBySecond(iv.start_),
                        epoch_.shiftedBySecond(iv.stop_));
}

TimeInterval TimeIntervalList::at(size_t i) const
{
    const Interval& iv = intervals_.at(i);
    return TimeInterval(epoch_.shiftedBySecond(iv.start_),
                        epoch_.shiftedBySecond(iv.stop_));
}


// ============================================================
// 修改
// ============================================================

void TimeIntervalList::push_back(const TimeInterval& ti)
{
    intervals_.push_back(ti.start().durationFrom(epoch_),
                         ti.stop().durationFrom(epoch_));
}

void TimeIntervalList::push_back(const TimePoint& start, const TimePoint& stop)
{
    intervals_.push_back(start.durationFrom(epoch_),
                         stop.durationFrom(epoch_));
}


// ============================================================
// 查询
// ============================================================

bool TimeIntervalList::contains(const TimePoint& t) const
{
    return intervals_.contains(t.durationFrom(epoch_));
}


// ============================================================
// 集合运算
// ============================================================

TimeIntervalList TimeIntervalList::merged() const
{
    TimeIntervalList result;
    result.epoch_ = epoch_;
    result.intervals_ = intervals_.merged();
    return result;
}

void TimeIntervalList::mergeInPlace()
{
    intervals_.mergeInPlace();
}

TimeIntervalList TimeIntervalList::intersect(const TimeIntervalList& other) const
{
    // 若 other 的历元不同，先将其区间转换到当前历元
    const IntervalList* otherRel = &other.intervals_;
    IntervalList converted;
    if (!(other.epoch_ == epoch_))
    {
        converted = convertEpoch(other.intervals_, other.epoch_, epoch_);
        otherRel = &converted;
    }

    TimeIntervalList result;
    result.epoch_ = epoch_;
    result.intervals_ = intervals_.intersect(*otherRel);
    return result;
}

TimeIntervalList TimeIntervalList::unite(const TimeIntervalList& other) const
{
    const IntervalList* otherRel = &other.intervals_;
    IntervalList converted;
    if (!(other.epoch_ == epoch_))
    {
        converted = convertEpoch(other.intervals_, other.epoch_, epoch_);
        otherRel = &converted;
    }

    TimeIntervalList result;
    result.epoch_ = epoch_;
    result.intervals_ = intervals_.unite(*otherRel);
    return result;
}

TimeIntervalList TimeIntervalList::subtract(const TimeIntervalList& other) const
{
    const IntervalList* otherRel = &other.intervals_;
    IntervalList converted;
    if (!(other.epoch_ == epoch_))
    {
        converted = convertEpoch(other.intervals_, other.epoch_, epoch_);
        otherRel = &converted;
    }

    TimeIntervalList result;
    result.epoch_ = epoch_;
    result.intervals_ = intervals_.subtract(*otherRel);
    return result;
}


AST_NAMESPACE_END
