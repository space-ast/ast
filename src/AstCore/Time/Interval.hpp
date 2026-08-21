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
#include "AstUtil/Logger.hpp"

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
    static Interval Zero();
    double start() const{return start_;}
    double stop() const{return stop_;}
    double& start() {return start_;}
    double& stop() {return stop_;}
    double duration() const{return stop_ - start_;}

    /// @brief 设置时间区间的开始时间和结束时间
    /// @param start 开始时间
    /// @param stop 结束时间
    void setBounds(double start, double stop);

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
    /// @note 不相交时结果为零长度区间（start == stop），语义为空。
    Interval& intersect(const Interval& other);

    /// @brief 交集（返回副本）
    /// @param other 另一个时间区间
    /// @return 交集（不修改当前对象）
    /// @note 不相交时结果为零长度区间（start == stop），语义为空。
    Interval intersected(const Interval& other) const;

    /// @brief 判断两个区间是否相交（有正长度交集，不含仅相切）
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


inline Interval Interval::Zero()
{
    return Interval{0.0, 0.0};
}

inline void Interval::setBounds(double start, double stop)
{
    start_ = start;
    stop_ = stop;
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
    double s = (std::max)(start_, other.start());
    double t = (std::min)(stop_,  other.stop());
    start_ = s;
    stop_  = (std::max)(t, s);   // 空交集 → 零长度区间，避免负 duration
    return *this;
}

inline Interval Interval::intersected(const Interval &other) const
{
    double s = (std::max)(start_, other.start());
    double t = (std::min)(stop_,  other.stop());
    return Interval{s, (std::max)(t, s)};
}

inline bool Interval::intersects(const Interval &other) const
{
    // 零长度区间（start == stop）语义为空，不应与任何区间相交
    return (std::max)(start_, other.start()) < (std::min)(stop_, other.stop());
}


/*! @} */

AST_NAMESPACE_END
