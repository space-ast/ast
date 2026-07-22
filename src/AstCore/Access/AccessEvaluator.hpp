///
/// @file      AccessEvaluator.hpp
/// @brief     访问评估器
/// @details   基于步进策略采样约束，发现符号翻转后用 Brent 求根精化边界，
///            输出满足约束的时间段集合。
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

#include "AstGlobal.h"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstCore/TimeIntervalList.hpp"

AST_NAMESPACE_BEGIN

class AccessConstraint;
class AccessStepper;

/// @brief 访问评估器
/// @details 在指定时间范围内，按步进策略采样单条约束，搜索满足区间并精化边界。
class AST_CORE_API AccessEvaluator : public ObjectNamed
{
public:
    AST_OBJECT(AccessEvaluator)

    AccessEvaluator();
    virtual ~AccessEvaluator();
    A_DISABLE_COPY(AccessEvaluator)

    /// @brief 设置约束
    void setConstraint(AccessConstraint* constraint) { constraint_ = constraint; }
    AccessConstraint* constraint() const { return constraint_; }

    /// @brief 设置步进策略
    void setStepper(AccessStepper* stepper) { stepper_ = stepper; }
    AccessStepper* stepper() const { return stepper_; }

    /// @brief 计算访问时间段
    /// @param interval 搜索时间区间
    /// @param result 满足约束的时间段列表
    /// @return 错误码
    errc_t evaluate(const TimeInterval& interval, TimeIntervalList& result);

    /// @brief 单点判断是否可访问
    bool check(const TimePoint& time) const;

private:
    AccessConstraint*  constraint_{};
    AccessStepper*     stepper_{};
};

/// @brief 评估访问约束的时间段
/// @param constraint 访问约束
/// @param stepper 步进策略
/// @param interval 搜索时间区间
/// @param result 满足约束的时间段列表
/// @return 错误码
AST_CORE_API errc_t aEvaluateAccess(
    const AccessConstraint* constraint,
    AccessStepper* stepper,
    const TimeInterval& interval,
    TimeIntervalList& result
);

AST_NAMESPACE_END
