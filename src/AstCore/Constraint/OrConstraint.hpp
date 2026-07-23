///
/// @file      OrConstraint.hpp
/// @brief     或约束组合器
/// @details   max(所有子约束值)，任一满足就算满足，短路计算。
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
#include <vector>
#include <limits>

AST_NAMESPACE_BEGIN

/// @brief 或约束组合器
/// @details 任一子约束满足就算满足。
///          evaluate() 返回 max(子约束值)，遇到正立即短路。
class AST_CORE_API OrConstraint : public AccessConstraint
{
public:
    AST_OBJECT(OrConstraint)

    OrConstraint() = default;

    /// @brief 添加子约束
    void add(AccessConstraint* constraint) { constraints_.push_back(constraint); }

    /// @brief 获取子约束数量
    size_t size() const { return constraints_.size(); }

    double evaluate(const TimePoint& time) const override;

private:
    std::vector<SharedPtr<AccessConstraint>> constraints_{};
};

AST_NAMESPACE_END
