///
/// @file      TimeIntervalList.hpp
/// @brief     绝对时段列表
/// @details   绝对时段列表类，以 TimePoint 历元 + IntervalList 的形式存储一组绝对时间区间。
///            集合运算委托给 IntervalList，跨历元运算时自动执行历元转换。
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

#pragma once

#include "AstGlobal.h"
#include "TimePoint.hpp"
#include "IntervalList.hpp"
#include "TimeInterval.hpp"
#include "TimeList.hpp"
#include <vector>
#include <iterator>     // for std::random_access_iterator_tag

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Time
    @{
*/


/// @brief 绝对时段列表
/// @details 绝对时段列表以 TimePoint 历元 + IntervalList 的形式存储一组绝对时间区间。
///
/// 存储模型：
/// - epoch_ : TimePoint — 参考历元
/// - intervals_ : IntervalList — 相对时间区间列表（相对于 epoch_ 的秒偏移量）
///
/// 集合运算委托给 IntervalList，跨历元运算时自动将 other 转换到当前历元。
///
/// 典型用途：
/// - 卫星可见窗口分析
/// - 多约束交集计算
/// - 通信时段管理
/// - 任务时间线规划
///
/// @note 列表可以无序。集合运算（merged、intersect 等）内部会先排序合并。
/// @warning 跨历元集合运算需执行 O(n) 的区间转换，性能略低于同历元运算。
class TimeIntervalList
{
public:

    // ————————————————————————
    // 工厂方法
    // ————————————————————————

    AST_CORE_API
    static TimeIntervalList FromTimeInterval(const TimeInterval& interval);

    /// @brief 从绝对区间向量构造
    /// @details 以第一个区间的 start 作为 epoch，所有区间转换为相对偏移。
    ///          若 intervals 为空，返回空列表（默认 epoch）。
    /// @param intervals 绝对区间向量
    /// @return TimeIntervalList
    AST_CORE_API
    static TimeIntervalList FromTimeIntervals(const std::vector<TimeInterval>& intervals);

    /// @brief 从绝对区间向量构造（指定历元）
    /// @param intervals 绝对区间向量
    /// @param epoch 参考历元
    /// @return TimeIntervalList
    AST_CORE_API
    static TimeIntervalList FromTimeIntervals(const std::vector<TimeInterval>& intervals, const TimePoint& epoch);

    /// @brief 从相对区间向量 + 历元构造
    /// @param intervals 相对区间向量
    /// @param epoch 参考历元
    /// @return TimeIntervalList
    AST_CORE_API
    static TimeIntervalList FromIntervals(const std::vector<Interval>& intervals, const TimePoint& epoch);

    /// @brief 从相对区间列表 + 历元构造
    /// @param list 相对区间列表
    /// @param epoch 参考历元
    /// @return TimeIntervalList
    AST_CORE_API
    static TimeIntervalList FromIntervalList(const IntervalList& list, const TimePoint& epoch);

public:

    // ————————————————————————
    // 构造与生命周期
    // ————————————————————————

    /// @brief 默认构造函数（空列表，默认 epoch）
    TimeIntervalList() = default;

    /// @brief 析构函数
    ~TimeIntervalList() = default;

    /// @brief 拷贝构造
    TimeIntervalList(const TimeIntervalList&) = default;

    /// @brief 拷贝赋值
    TimeIntervalList& operator=(const TimeIntervalList&) = default;

    /// @brief 移动构造
    TimeIntervalList(TimeIntervalList&&) noexcept = default;

    /// @brief 移动赋值
    TimeIntervalList& operator=(TimeIntervalList&&) noexcept = default;

    /// @brief 从参考历元构造空列表
    /// @param epoch 参考历元
    TimeIntervalList(const TimePoint& epoch)
        : epoch_(epoch) {}

    // ————————————————————————
    // 容量
    // ————————————————————————

    /// @brief 返回区间数量
    size_t size() const noexcept { return intervals_.size(); }

    /// @brief 判断列表是否为空
    bool empty() const noexcept { return intervals_.empty(); }

    /// @brief 预留容量
    void reserve(size_t n) { intervals_.reserve(n); }

    /// @brief 清空列表
    void clear() { intervals_.clear(); }

    // ————————————————————————
    // 元素访问
    // ————————————————————————

    /// @brief 访问第 i 个绝对区间（无边界检查）
    /// @param i 索引
    /// @return TimeInterval 由 epoch_ + 相对偏移计算得到（按值返回）
    AST_CORE_API
    TimeInterval operator[](size_t i) const;

    /// @brief 访问第 i 个绝对区间（边界检查）
    /// @param i 索引
    /// @return TimeInterval 由 epoch_ + 相对偏移计算得到（按值返回）
    /// @throws std::out_of_range 若 i >= size()
    AST_CORE_API
    TimeInterval at(size_t i) const;

    // ————————————————————————
    // 修改
    // ————————————————————————

    /// @brief 追加一个绝对区间
    /// @param ti 绝对时间区间
    AST_CORE_API
    void push_back(const TimeInterval& ti);

    /// @brief 通过起止时间点追加绝对区间
    /// @param start 开始时间点
    /// @param stop 结束时间点
    AST_CORE_API
    void push_back(const TimePoint& start, const TimePoint& stop);

    /// @brief 移除最后一个区间
    /// @warning 调用前应确保列表非空，否则行为未定义（与 std::vector 一致）
    void pop_back() { intervals_.pop_back(); }

    // ————————————————————————
    // 历元
    // ————————————————————————

    /// @brief 获取参考历元
    const TimePoint& epoch() const noexcept { return epoch_; }

    /// @brief 设置参考历元
    /// @warning 仅当列表为空时调用才是安全的。若列表非空，
    ///          已有的相对偏移仍相对于旧历元，将导致元素访问返回错误结果。
    void setEpoch(const TimePoint& epoch) { epoch_ = epoch; }

    // ————————————————————————
    // 底层容器访问
    // ————————————————————————

    /// @brief 获取底层相对区间列表（可修改）
    /// @warning 修改底层列表不会自动调整 epoch，调用者需自行保证一致性。
    IntervalList& intervals() noexcept { return intervals_; }

    /// @brief 获取底层相对区间列表（只读）
    const IntervalList& intervals() const noexcept { return intervals_; }

    // ————————————————————————
    // 查询
    // ————————————————————————

    /// @brief 总时长（所有区间 duration 之和，不考虑重叠）
    double totalDuration() const { return intervals_.totalDuration(); }

    /// @brief 检查绝对时间点是否落在任一区间内
    /// @param t 绝对时间点
    /// @return 是否在任一区间内
    AST_CORE_API
    bool contains(const TimePoint& t) const;

    // ————————————————————————
    // 集合运算
    // ————————————————————————

    /// @brief 合并重叠/相邻区间
    /// @details 返回排序后无重叠的区间列表。
    /// @return 合并后的 TimeIntervalList
    AST_CORE_API
    TimeIntervalList merged() const;

    /// @brief 原地合并重叠/相邻区间
    /// @details 直接修改当前对象，将其区间排序后合并重叠或相邻的区间。
    ///          合并后列表有序且无重叠。
    /// @note 若需保留原始数据，请使用 merged() 获取副本。
    AST_CORE_API
    void mergeInPlace();

    /// @brief 原地交集：将自身替换为「同时属于当前列表和 other 的时段」
    /// @param other 另一个绝对时段列表
    /// @return *this
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    ///       若需保留原始数据，请使用 intersected() 获取副本。
    AST_CORE_API
    TimeIntervalList& intersect(const TimeIntervalList& other);

    /// @brief 原地并集：将自身替换为「属于当前列表或 other 的时段」
    /// @param other 另一个绝对时段列表
    /// @return *this
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    ///       若需保留原始数据，请使用 united() 获取副本。
    AST_CORE_API
    TimeIntervalList& unite(const TimeIntervalList& other);

    /// @brief 原地差集：将自身替换为「属于当前列表但不属于 other 的时段」
    /// @param other 另一个绝对时段列表
    /// @return *this（*this - other）
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    ///       若需保留原始数据，请使用 subtracted() 获取副本。
    AST_CORE_API
    TimeIntervalList& subtract(const TimeIntervalList& other);

    /// @brief 交集（返回副本）：同时属于当前列表和 other 的时段
    /// @param other 另一个绝对时段列表
    /// @return 交集（不修改当前对象）
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    AST_CORE_API
    TimeIntervalList intersected(const TimeIntervalList& other) const;

    /// @brief 判断两个绝对时段列表是否相交（存在正长度重叠区间）
    /// @param other 另一个绝对时段列表
    /// @return 是否相交
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    AST_CORE_API
    bool intersects(const TimeIntervalList& other) const;

    /// @brief 并集（返回副本）：属于当前列表或 other 的时段
    /// @param other 另一个绝对时段列表
    /// @return 并集（不修改当前对象）
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    AST_CORE_API
    TimeIntervalList united(const TimeIntervalList& other) const;

    /// @brief 差集（返回副本）：属于当前列表但不属于 other 的时段
    /// @param other 另一个绝对时段列表
    /// @return 差集（*this - other，不修改当前对象）
    /// @note 若 other 的历元不同，自动转换到当前历元后再运算。
    AST_CORE_API
    TimeIntervalList subtracted(const TimeIntervalList& other) const;

    // ————————————————————————
    // 运算符重载（Qt / Boost.ICL 风格）
    // ————————————————————————

    /// @brief 原地交集（等价于 intersect）
    TimeIntervalList& operator&=(const TimeIntervalList& other) { return intersect(other); }

    /// @brief 原地并集（等价于 unite）
    TimeIntervalList& operator|=(const TimeIntervalList& other) { return unite(other); }

    /// @brief 原地差集（等价于 subtract）
    TimeIntervalList& operator-=(const TimeIntervalList& other) { return subtract(other); }

    /// @brief 交集（返回副本，等价于 intersected）
    TimeIntervalList operator&(const TimeIntervalList& other) const { return intersected(other); }

    /// @brief 并集（返回副本，等价于 united）
    TimeIntervalList operator|(const TimeIntervalList& other) const { return united(other); }

    /// @brief 差集（返回副本，等价于 subtracted）
    TimeIntervalList operator-(const TimeIntervalList& other) const { return subtracted(other); }

    // ————————————————————————
    // 转换
    // ————————————————————————

    /// @brief 转换为相对区间列表
    /// @return 当前 intervals_ 的副本
    IntervalList toIntervalList() const { return intervals_; }

    /// @brief 离散化（使用当前 epoch）
    /// @details 区间为闭区间 [start, stop]，两端点均会被包含在输出中。
    ///          对每个区间按步长生成时间点序列，末端点始终被强制包含。
    ///          详见 IntervalList::discrete。
    /// @param step 步长（秒，必须 > 0）
    /// @return TimeList 离散化后的时间列表
    TimeList discrete(double step) const { return intervals_.discrete(epoch_, step); }

    /// @brief 离散化（指定历元）
    /// @details 区间为闭区间 [start, stop]，两端点均会被包含在输出中。
    ///          对每个区间按步长生成时间点序列，末端点始终被强制包含。
    ///          详见 IntervalList::discrete。
    /// @param epoch 输出时间列表的参考历元
    /// @param step 步长（秒，必须 > 0）
    /// @return TimeList 离散化后的时间列表
    TimeList discrete(const TimePoint& epoch, double step) const { return intervals_.discrete(epoch, step); }

    /// @brief 转换为字符串进行展示
    /// @param precision 时间点格式化精度
    /// @return 字符串表示
    AST_CORE_API
    std::string toString(int precision = kTimePointDefaultFormatPrecision) const;

    // ————————————————————————
    // 迭代器
    // ————————————————————————

    /// @brief 随机访问常量迭代器
    /// @details 内部持有相对区间指针和历元指针，
    ///          解引用时计算 TimeInterval(epoch + data_->start_, epoch + data_->stop_) 并返回。
    class const_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = TimeInterval;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const TimeInterval*;
        using reference         = TimeInterval;  // 按值返回

        const_iterator() = default;

        const_iterator(const Interval* data, const TimePoint* epoch)
            : data_(data), epoch_(epoch) {}

        /// @brief 解引用：计算 TimeInterval(epoch_->shiftedBySecond(data_->start_), epoch_->shiftedBySecond(data_->stop_))
        TimeInterval operator*() const
        {
            return TimeInterval(epoch_->shiftedBySecond(data_->start_), epoch_->shiftedBySecond(data_->stop_));
        }

        /// @brief 下标访问
        TimeInterval operator[](difference_type n) const
        {
            return TimeInterval(epoch_->shiftedBySecond(data_[n].start_), epoch_->shiftedBySecond(data_[n].stop_));
        }

        const_iterator& operator++()                           { ++data_; return *this; }
        const_iterator  operator++(int)                        { auto tmp = *this; ++data_; return tmp; }
        const_iterator& operator--()                           { --data_; return *this; }
        const_iterator  operator--(int)                        { auto tmp = *this; --data_; return tmp; }
        const_iterator& operator+=(difference_type n)          { data_ += n; return *this; }
        const_iterator& operator-=(difference_type n)          { data_ -= n; return *this; }

        const_iterator  operator+(difference_type n)  const    { return {data_ + n, epoch_}; }
        const_iterator  operator-(difference_type n)  const    { return {data_ - n, epoch_}; }
        difference_type operator-(const const_iterator& o) const { return data_ - o.data_; }

        bool operator==(const const_iterator& o) const { return data_ == o.data_; }
        bool operator!=(const const_iterator& o) const { return data_ != o.data_; }
        bool operator< (const const_iterator& o) const { return data_ <  o.data_; }
        bool operator> (const const_iterator& o) const { return data_ >  o.data_; }
        bool operator<=(const const_iterator& o) const { return data_ <= o.data_; }
        bool operator>=(const const_iterator& o) const { return data_ >= o.data_; }

    private:
        const Interval* data_  = nullptr;
        const TimePoint* epoch_ = nullptr;
    };

    /// @brief difference_type + const_iterator（非成员运算符）
    friend const_iterator operator+(const_iterator::difference_type n, const const_iterator& it)
    {
        return it + n;
    }

    using iterator               = const_iterator;
    using value_type             = TimeInterval;
    using size_type              = size_t;
    using difference_type        = std::ptrdiff_t;

    iterator       begin()       { return {intervals_.intervals().data(), &epoch_}; }
    iterator       end()         { return {intervals_.intervals().data() + intervals_.size(), &epoch_}; }
    const_iterator begin() const { return {intervals_.intervals().data(), &epoch_}; }
    const_iterator end()   const { return {intervals_.intervals().data() + intervals_.size(), &epoch_}; }
    const_iterator cbegin()const { return begin(); }
    const_iterator cend()  const { return end(); }

private:
    TimePoint    epoch_{};   ///< 参考历元
    IntervalList intervals_{}; ///< 相对时间区间列表（相对于 epoch_）
};


/*! @} */


AST_NAMESPACE_END
