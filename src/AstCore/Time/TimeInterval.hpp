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
#include "Interval.hpp"
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
/// @param precision 时间点格式化精度
/// @return errc_t 错误码
AST_CORE_CAPI errc_t aTimeIntervalFormat(const TimeInterval& interval, std::string& strStart, std::string& strEnd, int precision = kTimePointDefaultFormatPrecision);


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
    static TimeInterval Parse(StringView strStart, StringView strEnd)
    {
        TimeInterval interval;
        aTimeIntervalParse(strStart, strEnd, interval);
        return interval;
    }

    TimeInterval() 
        : start_{}
        , stop_{} 
    {}
    ~TimeInterval() = default;

    /// @brief 构造函数
    /// @param start 开始时间点
    /// @param stop 结束时间点
    TimeInterval(const TimePoint& start, const TimePoint& stop)
        : start_(start), stop_(stop) {}

    /// @brief 构造函数
    /// @param epoch 时间区间的基准时间点
    /// @param start 开始时间点（相对基准时间点的秒数）
    /// @param stop 结束时间点（相对基准时间点的秒数）
    TimeInterval(const TimePoint& epoch, double start, double stop)
        : start_(epoch + start), stop_(epoch + stop) {}

    /// @brief 时间区间的开始时间点
    const TimePoint& start() const{return start_;}

    /// @brief 时间区间的开始时间点
    const TimePoint& getStart() const{return start_;}

    /// @brief 设置时间区间的开始时间点
    void setStart(const TimePoint& start){start_ = start;}

    /// @brief 时间区间的结束时间点
    const TimePoint& stop() const{return stop_;}

    /// @brief 时间区间的结束时间点
    const TimePoint& getStop() const{return stop_;}

    /// @brief 设置时间区间的结束时间点
    void setStop(const TimePoint& stop){stop_ = stop;}

    /// @brief 设置时间区间的开始时间点和结束时间点
    /// @param start 开始时间点
    /// @param stop 结束时间点
    void setBounds(const TimePoint& start, const TimePoint& stop){
        start_ = start;
        stop_  = stop;
    }

    /// @brief 设置时间区间的开始时间点和结束时间点
    /// @param epoch 时间区间的基准时间点
    /// @param start 开始时间点（相对基准时间点的秒数）
    /// @param stop 结束时间点（相对基准时间点的秒数）
    void setBounds(const TimePoint& epoch, double start, double stop){
        start_ = epoch + start;
        stop_  = epoch + stop;
    }

    /// @brief 设置时间区间的开始时间点和结束时间点
    /// @param epoch 时间区间的基准时间点
    /// @param interval 相对基准时间点的时间区间
    void setBounds(const TimePoint& epoch, const Interval& interval){
        start_ = epoch + interval.start();
        stop_  = epoch + interval.stop();
    }

    /// @brief 设置时间区间为无限时间区间
    void setInfinite()
    {
        start_ = {0, -std::numeric_limits<double>::infinity()};
        stop_  = {0, +std::numeric_limits<double>::infinity()};
    }
    /// @brief 设置时间区间为零时间区间
    /// @warning 这将使时间区间无效，无法用于计算。
    void setZero()
    {
        start_ = {0, 0.0};
        stop_  = {0, 0.0};
    }
public:
    /// @brief 时间区间的持续时间（秒）
    double duration() const{return stop_ - start_;}

public:
    /// @brief 将时间区间转换为字符串
    /// @param precision 时间点格式化精度
    /// @return 字符串表示
    std::string toString(int precision = kTimePointDefaultFormatPrecision) const{
        std::string strStart, strEnd;
        aTimeIntervalFormat(*this, strStart, strEnd, precision);
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

    /// @brief 将时间区间离散化
    /// @param step 离散化步长（秒）
    /// @param times 输出离散化时间点列表
    /// @return errc_t 错误码
    AST_CORE_API
    errc_t discrete(double step, TimeList& times) const;

    class DiscreteTimePointRange;
    class DiscreteEpochSecondRange;

    /// @brief 将时间区间离散化为时间点
    /// @param step 离散化步长（秒）
    /// @return 离散化时间点范围
    DiscreteTimePointRange discrete(double step) const;

    /// @brief 将时间区间离散化为相对秒
    /// @param epoch 时间区间的基准时间点
    /// @param step 离散化步长（秒）
    /// @return 离散化时间点范围
    DiscreteEpochSecondRange discrete(const TimePoint& epoch, double step) const;
public:
    /// @brief 原地并集：将另一个时间区间并入自身（凸包）
    /// @param other 要合并的时间区间
    /// @return *this
    /// @warning 凸包会桥接不相交区间的空隙；如需保留空隙的集合并集，请使用 IntervalList::united。
    TimeInterval& unite(const TimeInterval& other);

    /// @brief 并集（返回副本）：凸包
    /// @param other 要合并的时间区间
    /// @return 并集（不修改当前对象）
    TimeInterval united(const TimeInterval& other) const;

    /// @brief 原地交集
    /// @param other 要合并的时间区间
    /// @return *this
    /// @note 不相交时结果为零长度区间（start == stop），语义为空。
    TimeInterval& intersect(const TimeInterval& other);

    /// @brief 交集（返回副本）
    /// @param other 要合并的时间区间
    /// @return 交集（不修改当前对象）
    /// @note 不相交时结果为零长度区间（start == stop），语义为空。
    TimeInterval intersected(const TimeInterval& other) const;

    /// @brief 判断两个时间区间是否相交（有正长度交集，不含仅相切）
    /// @param other 另一个时间区间
    /// @return 是否相交
    bool intersects(const TimeInterval& other) const;

    /// @brief 原地并集（等价于 unite）
    TimeInterval& operator|=(const TimeInterval& other) { return unite(other); }

    /// @brief 并集（返回副本，等价于 united）
    TimeInterval operator|(const TimeInterval& other) const { return united(other); }

    /// @brief 原地交集（等价于 intersect）
    TimeInterval& operator&=(const TimeInterval& other) { return intersect(other); }

    /// @brief 交集（返回副本，等价于 intersected）
    TimeInterval operator&(const TimeInterval& other) const { return intersected(other); }
private:
    TimePoint start_;     ///< 时间区间的开始时间点
    TimePoint stop_;      ///< 时间区间的结束时间点
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
    size_t n = static_cast<size_t>(std::ceil(dur / step)) + 1;
    return DiscreteTimePointRange(*this, step, n);
}

inline TimeInterval::DiscreteEpochSecondRange TimeInterval::discrete(const TimePoint& epoch, double step) const
{
    double dur = duration();
    if (step <= 0.0 || dur <= 0.0) {
        return DiscreteEpochSecondRange(0.0, step, 0.0, 0);
    }
    size_t n = static_cast<size_t>(std::ceil(dur / step)) + 1;
    double offset = getStart() - epoch;
    double stopOffset = getStop() - epoch;
    return DiscreteEpochSecondRange(offset, step, stopOffset, n);
}

inline TimeInterval& TimeInterval::unite(const TimeInterval &other)
{
    const TimePoint& thisStart  = this->start();
    const TimePoint& thisStop   = this->stop();
    const TimePoint& otherStart = other.start();
    const TimePoint& otherStop  = other.stop();

    if(otherStart < thisStart)
    {
        this->setStart(otherStart);
    }
    if(otherStop > thisStop)
    {
        this->setStop(otherStop);
    }
    return *this;
}

inline TimeInterval TimeInterval::united(const TimeInterval &other) const
{
    TimeInterval result = *this;
    result.unite(other);
    return result;
}

inline TimeInterval& TimeInterval::intersect(const TimeInterval &other)
{
    const TimePoint& thisStart  = this->start();
    const TimePoint& thisStop   = this->stop();
    const TimePoint& otherStart = other.start();
    const TimePoint& otherStop  = other.stop();

    if(otherStart > thisStart)
    {
        this->setStart(otherStart);
    }
    if(otherStop < thisStop)
    {
        this->setStop(otherStop);
    }

    // 空交集 → 零长度区间（避免负 duration）
    if(this->start() > this->stop())
    {
        this->setStop(this->start());
    }
    return *this;
}

inline TimeInterval TimeInterval::intersected(const TimeInterval &other) const
{
    TimeInterval result = *this;
    result.intersect(other);
    return result;
}

inline bool TimeInterval::intersects(const TimeInterval &other) const
{
    // 零长度区间（start == stop）语义为空，不应与任何区间相交
    auto& start = (std::max)(start_, other.start());
    auto& stop = (std::min)(stop_, other.stop());
    return start < stop;
}

/*! @} */



AST_NAMESPACE_END