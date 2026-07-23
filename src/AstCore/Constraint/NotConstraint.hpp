///
/// @file      NotConstraint.hpp
/// @brief     非约束组合器
/// @details   对子约束取反：正值变负值、负值变正值，零点不变。
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

#include "AccessConstraint.hpp"

AST_NAMESPACE_BEGIN

/// @brief 非约束组合器
/// @details evaluate() 返回 -子约束值。
///          子约束满足时为负，子约束违反时为正。
class AST_CORE_API NotConstraint : public AccessConstraint
{
public:
    AST_OBJECT(NotConstraint)

    NotConstraint() = default;
    explicit NotConstraint(AccessConstraint* constraint);

    void setConstraint(AccessConstraint* c) { constraint_ = c; }

    double evaluate(const TimePoint& time) const override;

private:
    SharedPtr<AccessConstraint> constraint_{};
};

AST_NAMESPACE_END
