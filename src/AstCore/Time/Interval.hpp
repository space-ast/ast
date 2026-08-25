///
/// @file      Interval.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-13
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "DoubleRange.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Math.hpp"
#include <cstddef>
#include <cmath>
#include <limits>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 相对时间区间
/// @details 相对时间区间类，用于表示时间区间的开始时间和结束时间
/// @warning 在使用相对时间区间时，需要明确相对时间区间的基准时间点（epoch）
class Interval
{
public:
    /// @brief 空区间哨兵（{+∞, -∞}，IEEE 1788 / Boost.Interval 惯例）
    static Interval Empty();
public:
    double start() const{return start_;}
    double stop() const{return stop_;}
    double& start() {return start_;}
    double& stop() {return stop_;}
    double duration() const{return stop_ - start_;}
public:
    /// @brief 是否为空区间（start_ > stop_，弱序否定；反向 / {+∞,-∞} 哨兵 / NaN 边界 → true）
    bool isEmpty() const { return !(start_ <= stop_); }
    /// @brief 是否为点区间（start_ == stop_，恰含一个瞬时，非空）
    bool isPoint() const { return start_ == stop_; }
    /// @brief isPoint() 的别名
    bool isDegenerate() const { return isPoint(); }
    /// @brief 是否非空（start_ <= stop_，即点区间或有效区间）
    bool isValid() const { return start_ <= stop_; }
    /// @brief isValid() 的别名
    bool isNonEmpty() const { return isValid(); }
    /// @brief 是否包含给定瞬时 t（闭区间含端点）
    bool contains(double t) const { return start_ <= t && t <= stop_; }
public:
    /// @brief 设置时间区间的开始时间和结束时间
    /// @param start 开始时间
    /// @param stop 结束时间
    /// @note 不钳制 start > stop（反向区间作为空区间存储，见 isEmpty()）
    void setBounds(double start, double stop);

    /// @brief 设置为空区间（{+∞, -∞}）
    void setEmpty();

    /// @brief 设置为全区间（{-∞, +∞}）
    void setWhole();
public:
    /// @brief 将相对时间区间离散化
    /// @details 对 [start, stop] 闭区间按 step 步长采样，返回惰性可迭代的秒偏移范围。
    ///          内部相邻点间距恒为 step，但末尾强制把 stop 并入输出（最后一段间距可能
    ///          小于 step），因此并非均匀网格。当 step <= 0.0 或区间为空（isEmpty()）时
    ///          返回空范围；点区间（start == stop）返回 1 个采样点。
    /// @param step 离散化步长（秒）
    /// @return 离散化秒偏移范围
    DoubleRange discretize(double step) const;

    /// @brief 计算离散化采样点数量
    /// @details 返回 discretize(step) 所生成的采样点数（含首尾端点）。当
    ///          step > 0.0 且区间非空（!isEmpty()）时为 ceil(duration()/step) + 1，
    ///          点区间（start == stop）返回 1，否则返回 0。
    /// @param step 离散化步长（秒）
    /// @return 采样点数
    size_t discretizedCount(double step) const;


    /// @brief 原地并集：将另一个区间并入自身（凸包）
    /// @param other 另一个时间区间
    /// @return *this
    /// @warning 凸包会桥接不相交区间的空隙；如需保留空隙的集合并集，请使用 IntervalList::united。
    Interval& unite(const Interval& other);

    /// @brief 并集（返回副本）：凸包
    /// @param other 另一个时间区间
    /// @return 并集（不修改当前对象）
    Interval united(const Interval& other) const;

    /// @brief 原地交集
    /// @param other 另一个时间区间
    /// @return *this
    /// @note 不相交或 NaN 时结果为规范空区间（{+∞,-∞}，isEmpty()）；相切时结果为点区间（start == stop，非空）。
    Interval& intersect(const Interval& other);

    /// @brief 交集（返回副本）
    /// @param other 另一个时间区间
    /// @return 交集（不修改当前对象）
    /// @note 不相交或 NaN 时结果为规范空区间（{+∞,-∞}，isEmpty()）；相切时结果为点区间（start == stop，非空）。
    Interval intersected(const Interval& other) const;

    /// @brief 判断两个区间是否相交（非空交集，含仅相切产生的点区间）
    /// @param other 另一个时间区间
    /// @return 是否相交
    bool intersects(const Interval& other) const;

    /// @brief 原地并集（等价于 unite）
    Interval& operator|=(const Interval& other) { return unite(other); }

    /// @brief 并集（返回副本，等价于 united）
    Interval operator|(const Interval& other) const { return united(other); }

    /// @brief 原地交集（等价于 intersect）
    Interval& operator&=(const Interval& other) { return intersect(other); }

    /// @brief 交集（返回副本，等价于 intersected）
    Interval operator&(const Interval& other) const { return intersected(other); }
public:
    double start_;
    double stop_;
};

inline Interval Interval::Empty()
{
    return Interval{+(std::numeric_limits<double>::infinity()),
                    -(std::numeric_limits<double>::infinity()) }; 
}

inline void Interval::setBounds(double start, double stop)
{
    start_ = start;
    stop_ = stop;
}

inline void Interval::setEmpty()
{
    start_ = +std::numeric_limits<double>::infinity();
    stop_  = -std::numeric_limits<double>::infinity();
}

inline void Interval::setWhole()
{
    start_ = -std::numeric_limits<double>::infinity();
    stop_  = +std::numeric_limits<double>::infinity();
}

inline Interval& Interval::unite(const Interval &other)
{
    start_ = (std::min)(start_, other.start());
    stop_  = (std::max)(stop_,  other.stop());
    return *this;
}

inline Interval Interval::united(const Interval &other) const
{
    return Interval{(std::min)(start_, other.start()),
                    (std::max)(stop_,  other.stop())};
}

inline Interval& Interval::intersect(const Interval &other)
{
    double s = (propagate_nan::max)(start_, other.start_);
    double t = (propagate_nan::min)(stop_,  other.stop());
    if (!(s <= t)) {
        *this = Empty();   // 不相交 → 规范空区间（不钳回假点）
    } else {
        start_ = s;
        stop_  = t;
    }
    return *this;
}

inline Interval Interval::intersected(const Interval &other) const
{
    double s = (propagate_nan::max)(start_, other.start_);
    double t = (propagate_nan::min)(stop_,  other.stop());
    if (!(s <= t)) {
        return Empty();    // 不相交 → 规范空区间
    }
    return Interval{s, t};
}

inline bool Interval::intersects(const Interval &other) const
{
    // 与 intersected() 一致：非空交集（含相切产生的点区间）才算相交；NaN → false
    return (propagate_nan::max)(start_, other.start()) <= (propagate_nan::min)(stop_, other.stop());
}

inline DoubleRange Interval::discretize(double step) const
{
    double dur = duration();
    if(step > 0 && dur >= 0)
    {
        size_t n = static_cast<size_t>(std::ceil(dur / step)) + 1;
        return DoubleRange(start_, stop_, step, n);
    }
    // 如果存在 nan，duration()为 nan，if 条件为false, 返回空范围
    return DoubleRange(0.0, 0.0, step, 0);
}

inline size_t Interval::discretizedCount(double step) const
{
    double dur = duration();
    if(step > 0 && dur >= 0)
    {
        return static_cast<size_t>(std::ceil(dur / step)) + 1;
    }
    // 如果存在 nan，duration()为 nan，if 条件为false, 返回0
    return 0;
}


/*! @} */

AST_NAMESPACE_END
