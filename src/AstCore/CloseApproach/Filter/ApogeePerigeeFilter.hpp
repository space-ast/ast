///
/// @file      ApogeePerigeeFilter.hpp
/// @brief     近/远地点预过滤
/// @author    axel
/// @date      2026-08-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "CloseApproachFilter.hpp"
#include "AstUtil/Constants.hpp"

AST_NAMESPACE_BEGIN

/// @brief 近/远地点预过滤
/// @details 剔除近/远地点半径区间（含 padding）与参考对象不重叠的候选对象。
class AST_CORE_API ApogeePerigeeFilter final : public CloseApproachFilter
{
public:
    ApogeePerigeeFilter(const CloseApproachOptions& opts, const CloseApproachContext& ctx);

    bool keep(const TLE& candidate) const override;

private:
    double apogeeRadius_{};
    double perigeeRadius_{};
    double gm_{kEarthGrav};
    double threshold_{};
};

AST_NAMESPACE_END
