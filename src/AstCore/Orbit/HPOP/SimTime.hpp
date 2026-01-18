///
/// @file      SimTime.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
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
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/// @brief 仿真时间
/// @details 仿真时间类，包含仿真时间点和相对于参考时间点已经过的时间。
class SimTime{
public:
    /// @brief 获取仿真时间点
    /// @return 仿真时间点
    const TimePoint& timePoint() const { return timePoint_; }
    /// @brief 获取已经过时间
    /// @return 已经过时间
    double elapsedTime() const { return elapsedTime_; }
    /// @brief 设置仿真时间点
    /// @param timePoint 仿真时间点
    void setTimePoint(const TimePoint& timePoint) { timePoint_ = timePoint; }
    /// @brief 设置已经过时间
    /// @param elapsedTime 已经过时间
    void setElapsedTime(double elapsedTime) { elapsedTime_ = elapsedTime; }
public:
    double elapsedTime_{0.0};       ///< 已经过时间(相对于参考时间点)
    TimePoint timePoint_{};         ///< 仿真时间点
};


AST_NAMESPACE_END
