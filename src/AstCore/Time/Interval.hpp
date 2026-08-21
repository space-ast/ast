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

    /// @brief 合并两个时间区间
    /// @param other 另一个时间区间
    /// @warning 如果时间区间与当前时间区间不重叠，合并操作将失败。
    /// @return errc_t 错误码
    errc_t merge(const Interval& other);
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

inline errc_t Interval::merge(const Interval &other)
{
    if (start_ > other.stop() || other.start() > stop_)
    {
        aError("merge interval failed, no overlap");
        return eErrorInvalidParam;
    }
    start_ = (std::min)(start_, other.start());
    stop_ = (std::max)(stop_, other.stop());
    return eNoError;
}


/*! @} */

AST_NAMESPACE_END
