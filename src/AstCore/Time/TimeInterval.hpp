///
/// @file      TimeInterval.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-04
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

#pragma once

#include "AstGlobal.h"
#include "TimePoint.hpp"
#include "AstUtil/Logger.hpp"
#include <string>
#include <limits>
#include <vector>
#include <cmath>

AST_NAMESPACE_BEGIN



/*!
    @addtogroup Time
    @{
*/


class TimeInterval;

/// @brief 将时间区间格式化为字符串
/// @param interval 时间区间
/// @param str 输出字符串
/// @return errc_t 错误码
AST_CORE_CAPI errc_t aTimeIntervalFormat(const TimeInterval& interval, std::string& strStart, std::string& strEnd);


/// @brief 从字符串解析时间区间
/// @param str 输入字符串
/// @param interval 输出时间区间
/// @return errc_t 错误码
AST_CORE_CAPI errc_t aTimeIntervalParse(StringView strStart, StringView strEnd, TimeInterval& interval);


/// @brief 时间区间
/// @details 时间区间表示一个连续时间区间，由开始时间点和结束时间点定义。
class TimeInterval
{
public:
    TimeInterval() = default;
    ~TimeInterval() = default;

    /// @brief 构造函数
    /// @param start 开始时间点
    /// @param stop 结束时间点
    TimeInterval(const TimePoint& start, const TimePoint& stop){
        this->setStartStop(start, stop);
    }

    /// @brief 构造函数
    /// @param epoch 时间区间的基准时间点
    /// @param start 开始时间点（相对基准时间点的秒数）
    /// @param stop 结束时间点（相对基准时间点的秒数）
    TimeInterval(const TimePoint& epoch, double start, double stop)
    {
        this->setStartStop(epoch, start, stop);
    }

    /// @brief 时间区间的开始时间点
    const TimePoint& start() const{return reinterpret_cast<const TimePoint&>(*this);}

    /// @brief 时间区间的开始时间点
    const TimePoint& getStart() const{return reinterpret_cast<const TimePoint&>(*this);}

    /// @brief 时间区间的结束时间点
    TimePoint stop() const{return {epoch_, stop_};}

    /// @brief 时间区间的结束时间点
    TimePoint getStop() const{return {epoch_, stop_};}


    /// @brief 设置时间区间的开始时间点和结束时间点
    /// @param start 开始时间点
    /// @param stop 结束时间点
    void setStartStop(const TimePoint& start, const TimePoint& stop){
        epoch_ = start.integerPart();
        start_ = start.fractionalPart();
        stop_ = stop.fractionalPart() + (stop.integerPart() - start.integerPart());
    }

    /// @brief 设置时间区间的开始时间点和结束时间点
    /// @param epoch 时间区间的基准时间点
    /// @param start 开始时间点（相对基准时间点的秒数）
    /// @param stop 结束时间点（相对基准时间点的秒数）
    void setStartStop(const TimePoint& epoch, double start, double stop){
        epoch_ = epoch.integerPart();
        start_ = epoch.fractionalPart() + start;
        stop_ = epoch.fractionalPart() + stop;
    }

    /// @brief 设置时间区间为无限时间区间
    void setInfinite()
    {
        epoch_ = 0;
        start_ = -std::numeric_limits<double>::infinity();
        stop_  = +std::numeric_limits<double>::infinity();
    }
    /// @brief 设置时间区间为零时间区间
    /// @warning 这将使时间区间无效，无法用于计算。
    void setZero()
    {
        epoch_ = 0;
        start_ = 0.0;
        stop_  = 0.0;
    }
public:
    /// @brief 时间区间的持续时间（秒）
    double duration() const{return stop_ - start_;}

public:
    /// @brief 将时间区间转换为字符串
    std::string toString() const{
        std::string strStart, strEnd;
        aTimeIntervalFormat(*this, strStart, strEnd);
        return strStart + " - " + strEnd;
    }
public:
    /// @brief 将时间区间离散化
    /// @param epoch 时间区间的基准时间点
    /// @param step 离散化步长（秒）
    /// @param times 输出离散化时间点
    /// @return errc_t 错误码
    AST_CORE_API
    errc_t discrete(const TimePoint& epoch, double step, std::vector<double>& times) const;

    /// @brief 将时间区间离散化
    /// @param step 离散化步长（秒）
    /// @param times 输出离散化时间点
    /// @return errc_t 错误码
    AST_CORE_API
    errc_t discrete(double step, std::vector<TimePoint>& times) const;

    class DiscreteTimePointRange;
    class DiscreteEpochSecondRange;

    /// @brief 将时间区间离散化
    /// @param step 离散化步长（秒）
    /// @return 离散化时间点范围
    DiscreteTimePointRange discrete(double step) const;

    /// @brief 将时间区间离散化
    /// @param epoch 时间区间的基准时间点
    /// @param step 离散化步长（秒）
    /// @return 离散化时间点范围
    DiscreteEpochSecondRange discrete(const TimePoint& epoch, double step) const;
public:
    /// @brief 合并时间区间
    /// @param other 要合并的时间区间
    /// @warning 如果时间区间与当前时间区间不重叠，合并操作将失败。
    /// @return errc_t 错误码
    errc_t merge(const TimeInterval& other);
private:
    int64_t epoch_{0};     ///< 时间区间的基准时间点（秒，从J2000.0 TAI 开始）
    double  start_{0};     ///< 相对开始时间(s)
    double  stop_{0};      ///< 相对结束时间(s)
};


/// @brief 离散化时间点范围
class TimeInterval::DiscreteTimePointRange {
public:
    DiscreteTimePointRange(const TimeInterval& interval, double step, size_t n)
        : interval_(interval), step_(step), n_(n) {}

    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = TimePoint;
        using difference_type   = ptrdiff_t;
        using pointer           = const TimePoint*;
        using reference         = const TimePoint&;

        iterator() = default;
        iterator(const DiscreteTimePointRange* range, size_t idx)
            : range_(range), idx_(idx), value_() {}

        reference operator*() const {
            if (idx_ == range_->n_ - 1) {
                value_ = range_->interval_.stop();
            } else {
                value_ = range_->interval_.start() + range_->step_ * idx_;
            }
            return value_;
        }

        iterator& operator++() { ++idx_; return *this; }
        iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        bool operator==(const iterator& other) const { return idx_ == other.idx_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        const DiscreteTimePointRange* range_ = nullptr;
        size_t idx_ = 0;
        mutable TimePoint value_{};
    };

    iterator begin() const { return iterator(this, 0); }
    iterator end()   const { return iterator(this, n_); }
    size_t size() const { return n_; }

private:
    TimeInterval interval_;
    double step_;
    size_t n_;
};

/// @brief 离散化历元秒范围
class TimeInterval::DiscreteEpochSecondRange {
public:
    DiscreteEpochSecondRange(double offset, double step, double stopOffset, size_t n)
        : offset_(offset), step_(step), stopOffset_(stopOffset), n_(n) {}

    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = double;
        using difference_type   = ptrdiff_t;
        using pointer           = const double*;
        using reference         = const double&;

        iterator() = default;
        iterator(const DiscreteEpochSecondRange* range, size_t idx)
            : range_(range), idx_(idx), value_() {}

        reference operator*() const {
            if (idx_ == range_->n_ - 1) {
                value_ = range_->stopOffset_;
            } else {
                value_ = range_->offset_ + range_->step_ * idx_;
            }
            return value_;
        }

        iterator& operator++() { ++idx_; return *this; }
        iterator operator++(int) { auto tmp = *this; ++*this; return tmp; }

        bool operator==(const iterator& other) const { return idx_ == other.idx_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }

    private:
        const DiscreteEpochSecondRange* range_ = nullptr;
        size_t idx_ = 0;
        mutable double value_{0.0};
    };

    iterator begin() const { return iterator(this, 0); }
    iterator end()   const { return iterator(this, n_); }
    size_t size() const { return n_; }
private:
    double offset_;
    double step_;
    double stopOffset_;
    size_t n_;
};


inline TimeInterval::DiscreteTimePointRange TimeInterval::discrete(double step) const
{
    double dur = duration();
    if (step <= 0.0 || dur <= 0.0) {
        return DiscreteTimePointRange(*this, step, 0);
    }
    size_t n = static_cast<size_t>(std::ceil(dur / step));
    return DiscreteTimePointRange(*this, step, n);
}

inline TimeInterval::DiscreteEpochSecondRange TimeInterval::discrete(const TimePoint& epoch, double step) const
{
    double dur = duration();
    if (step <= 0.0 || dur <= 0.0) {
        return DiscreteEpochSecondRange(0.0, step, 0.0, 0);
    }
    size_t n = static_cast<size_t>(std::ceil(dur / step));
    double offset = getStart() - epoch;
    double stopOffset = getStop() - epoch;
    return DiscreteEpochSecondRange(offset, step, stopOffset, n);
}

inline errc_t TimeInterval::merge(const TimeInterval &other)
{
    const TimePoint& thisStart = start();
    const TimePoint& otherStart = other.start();
    TimePoint thisStop = stop();
    TimePoint otherStop = other.stop();

    if (thisStart.durationFrom(otherStop) > 0 || otherStart.durationFrom(thisStop) > 0) 
    {
        aError("merge time interval failed, no overlap");
        return eErrorInvalidParam;
    }

    const TimePoint& mergedStart = (thisStart.durationFrom(otherStart) <= 0) ? thisStart : otherStart;
    const TimePoint& mergedStop = (thisStop.durationFrom(otherStop) >= 0) ? thisStop : otherStop;

    setStartStop(mergedStart, mergedStop);
    return eNoError;
}

/*! @} */



AST_NAMESPACE_END