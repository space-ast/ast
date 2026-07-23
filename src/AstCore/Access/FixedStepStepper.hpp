///
/// @file      FixedStepStepper.hpp
/// @brief     固定步长步进器
/// @details   以固定时间步长在时间范围内生成采样点。
/// @author    axel
/// @date      2026-07-22
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "AccessStepper.hpp"

AST_NAMESPACE_BEGIN

/// @brief 固定步长步进器
class AST_CORE_API FixedStepStepper : public AccessStepper
{
public:
    AST_OBJECT(FixedStepStepper)

    FixedStepStepper() = default;

    /// @brief 构造固定步长步进器
    /// @param stepSize 步长（秒）
    explicit FixedStepStepper(double stepSize);

    void init(const TimePoint& start, const TimePoint& stop) override;
    bool next(TimePoint& tp) override;

    void setStepSize(double step) { stepSize_ = step; }
    double stepSize() const { return stepSize_; }

private:
    double     stepSize_{60.0};
    TimePoint  start_{};
    TimePoint  stop_{};
    TimePoint  current_{};
    bool       initDone_{false};
};

AST_NAMESPACE_END
