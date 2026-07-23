///
/// @file      OrConstraint.cpp
/// @brief     或约束组合器实现
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


#include "OrConstraint.hpp"

AST_NAMESPACE_BEGIN

double OrConstraint::evaluate(const TimePoint& time) const
{
    if (constraints_.empty()) { return -1.0; }

    double result = -std::numeric_limits<double>::max();
    for (auto& c : constraints_) {
        double v = c->evaluate(time);
        if (v > 0.0) { return v; }    // 已满足，短路
        if (v > result) { result = v; }
    }
    return result;
}

AST_NAMESPACE_END
