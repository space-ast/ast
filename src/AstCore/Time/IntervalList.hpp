///
/// @file      IntervalList.hpp
/// @brief     相对时段列表
/// @details   相对时段列表类，用于表示一组相对时间区间（不含历元）。
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
#include "Interval.hpp"
#include "TimePoint.hpp"
#include <vector>
#include <initializer_list>

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Time
    @{
*/


class TimeList;

/// @brief 相对时段列表
/// @details 相对时段列表以 std::vector&lt;Interval&gt; 的形式存储一组相对时间区间。
///
/// 存储模型：
/// - intervals_ : std::vector&lt;Interval&gt; — 相对时间区间列表
///
/// 不假设列表有序或非重叠。使用方式与 std::vector&lt;Interval&gt; 一致，
/// 同时提供集合运算方法（合并、交集、并集、差集）。
///
/// 典型用途：
/// - 覆盖分析（卫星何时可见）
/// - 约束满足（多约束交集）
/// - 可用窗口（通信时段）
/// - 间隙分析
///
/// @note 列表可以无序。集合运算（merged、intersect 等）内部会先排序合并。
/// @warning 在使用相对时段列表时，需要明确基准时间点（epoch）。
class IntervalList
{
public:

    // ————————————————————————
    // 工厂方法
    // ————————————————————————

    /// @brief 从区间向量构造
    /// @param intervals 区间向量
    /// @return IntervalList
    AST_CORE_API
    static IntervalList FromIntervals(const std::vector<Interval>& intervals);

public:

    // ————————————————————————
    // 构造与生命周期
    // ————————————————————————

    /// @brief 默认构造函数
    IntervalList() = default;

    /// @brief 析构函数
    ~IntervalList() = default;

    /// @brief 拷贝构造
    IntervalList(const IntervalList&) = default;

    /// @brief 拷贝赋值
    IntervalList& operator=(const IntervalList&) = default;

    /// @brief 移动构造
    IntervalList(IntervalList&&) noexcept = default;

    /// @brief 移动赋值
    IntervalList& operator=(IntervalList&&) noexcept = default;

    /// @brief 从初始化列表构造
    /// @param il 初始化列表
    IntervalList(std::initializer_list<Interval> il)
        : intervals_(il) {}

    explicit IntervalList(const std::vector<Interval>& intervals)
        : intervals_(intervals) 
    {}

    explicit IntervalList(std::vector<Interval>&& intervals)
        : intervals_(std::move(intervals)) 
    {}
    
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

    /// @brief 访问第 i 个区间（无边界检查）
    /// @param i 索引
    Interval& operator[](size_t i) { return intervals_[i]; }

    /// @brief 访问第 i 个区间（无边界检查）
    /// @param i 索引
    const Interval& operator[](size_t i) const { return intervals_[i]; }

    /// @brief 访问第 i 个区间（边界检查）
    /// @param i 索引
    /// @return 第 i 个区间
    Interval& at(size_t i)             { return intervals_.at(i); }

    /// @brief 访问第 i 个区间（边界检查）
    /// @param i 索引
    /// @return 第 i 个区间
    const Interval& at(size_t i) const { return intervals_.at(i); }

    // ————————————————————————
    // 迭代器
    // ————————————————————————

    using iterator               = std::vector<Interval>::iterator;
    using const_iterator         = std::vector<Interval>::const_iterator;

    iterator begin()             { return intervals_.begin(); }
    iterator end()               { return intervals_.end(); }
    const_iterator begin() const { return intervals_.begin(); }
    const_iterator end()   const { return intervals_.end(); }

    // ————————————————————————
    // 修改
    // ————————————————————————

    /// @brief 追加一个区间
    /// @param interval 区间
    void push_back(const Interval& interval) { intervals_.push_back(interval); }

    /// @brief 追加一个区间
    /// @param start 开始时间（秒）
    /// @param stop 结束时间（秒）
    void push_back(double start, double stop) { intervals_.push_back({start, stop}); }

    /// @brief 移除最后一个区间
    /// @warning 调用前应确保列表非空，否则行为未定义（与 std::vector 一致）
    void pop_back() { intervals_.pop_back(); }

    // ————————————————————————
    // 底层容器访问
    // ————————————————————————

    /// @brief 获取底层区间向量（可修改）
    std::vector<Interval>& intervals() noexcept { return intervals_; }

    /// @brief 获取底层区间向量（只读）
    const std::vector<Interval>& intervals() const noexcept { return intervals_; }

    // ————————————————————————
    // 查询
    // ————————————————————————

    /// @brief 总时长（所有区间 duration 之和，不考虑重叠；反向区间贡献为 0）
    /// @return 总时长（秒）
    double totalDuration() const
    {
        double total = 0.0;
        for (const auto& iv : intervals_)
        {
            double d = iv.duration();
            total += (d > 0.0) ? d : 0.0;
        }
        return total;
    }

    /// @brief 检查时间点 t 是否落在任一区间内
    /// @param t 时间点（相对秒数）
    /// @return 是否在任一区间内
    bool contains(double t) const
    {
        for (const auto& iv : intervals_)
        {
            if (t >= iv.start_ && t <= iv.stop_)
                return true;
        }
        return false;
    }

    // ————————————————————————
    // 集合运算
    // ————————————————————————

    /// @brief 合并重叠/相邻区间
    /// @details 将列表中的区间按 start 排序，合并重叠或相邻的区间，
    ///          返回排序后无重叠的区间列表。
    /// @return 合并后的 IntervalList
    AST_CORE_API
    IntervalList merged() const;

    /// @brief 原地合并重叠/相邻区间
    /// @details 直接修改当前对象，将其区间按 start 排序后合并重叠或相邻的区间。
    ///          合并后列表有序且无重叠。
    /// @note 若需保留原始数据，请使用 merged() 获取副本。
    AST_CORE_API
    void mergeInPlace();

    /// @brief 原地交集：将自身替换为「同时属于当前列表和 other 的时段」
    /// @param other 另一个时段列表
    /// @return *this
    /// @note 若需保留原始数据，请使用 intersected() 获取副本。
    AST_CORE_API
    IntervalList& intersect(const IntervalList& other);

    /// @brief 原地并集：将自身替换为「属于当前列表或 other 的时段」
    /// @param other 另一个时段列表
    /// @return *this
    /// @note 若需保留原始数据，请使用 united() 获取副本。
    AST_CORE_API
    IntervalList& unite(const IntervalList& other);

    /// @brief 原地差集：将自身替换为「属于当前列表但不属于 other 的时段」
    /// @param other 另一个时段列表
    /// @return *this
    /// @note 若需保留原始数据，请使用 subtracted() 获取副本。
    AST_CORE_API
    IntervalList& subtract(const IntervalList& other);

    /// @brief 交集（返回副本）：同时属于当前列表和 other 的时段
    /// @param other 另一个时段列表
    /// @return 交集（不修改当前对象）
    AST_CORE_API
    IntervalList intersected(const IntervalList& other) const;

    /// @brief 判断两个时段列表是否相交（存在正长度重叠区间）
    /// @param other 另一个时段列表
    /// @return 是否相交
    AST_CORE_API
    bool intersects(const IntervalList& other) const;

    /// @brief 并集（返回副本）：属于当前列表或 other 的时段
    /// @param other 另一个时段列表
    /// @return 并集（不修改当前对象）
    AST_CORE_API
    IntervalList united(const IntervalList& other) const;

    /// @brief 差集（返回副本）：属于当前列表但不属于 other 的时段
    /// @param other 另一个时段列表
    /// @return 差集（不修改当前对象）
    AST_CORE_API
    IntervalList subtracted(const IntervalList& other) const;

    // ————————————————————————
    // 运算符重载（Qt / Boost.ICL 风格）
    // ————————————————————————

    /// @brief 原地交集（等价于 intersect）
    IntervalList& operator&=(const IntervalList& other) { return intersect(other); }

    /// @brief 原地并集（等价于 unite）
    IntervalList& operator|=(const IntervalList& other) { return unite(other); }

    /// @brief 原地差集（等价于 subtract）
    IntervalList& operator-=(const IntervalList& other) { return subtract(other); }

    /// @brief 交集（返回副本，等价于 intersected）
    IntervalList operator&(const IntervalList& other) const { return intersected(other); }

    /// @brief 并集（返回副本，等价于 united）
    IntervalList operator|(const IntervalList& other) const { return united(other); }

    /// @brief 差集（返回副本，等价于 subtracted）
    IntervalList operator-(const IntervalList& other) const { return subtracted(other); }

    // ————————————————————————
    // 与 TimeList 互转
    // ————————————————————————

    /// @brief 将每个区间按步长离散化，拼接为 TimeList
    /// @details 区间为闭区间 [start, stop]，两端点均会被包含在输出中。
    ///          对每个区间，生成序列 start, start+step, start+2*step, ... 直至 stop
    ///          （最后一个点始终为 stop，即使它不落在步长网格上）。
    ///          例如：[0, 10] step=3 → {0, 3, 6, 9, 10}
    ///                [0, 10] step=2 → {0, 2, 4, 6, 8, 10}
    /// @param epoch 参考历元
    /// @param step 离散化步长（秒，必须 > 0）
    /// @return TimeList 离散化后的时间点列表
    AST_CORE_API
    TimeList discretize(const TimePoint& epoch, double step) const;

private:
    std::vector<Interval> intervals_{};   ///< 相对时间区间列表
};


/*! @} */


AST_NAMESPACE_END
