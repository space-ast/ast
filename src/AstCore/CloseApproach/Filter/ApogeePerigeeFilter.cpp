///
/// @file      ApogeePerigeeFilter.cpp
/// @brief     近/远地点预过滤实现
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

#include "ApogeePerigeeFilter.hpp"
#include "AstCore/CloseApproachOptions.hpp"
#include "AstCore/CloseApproachGeometry.hpp"

AST_NAMESPACE_BEGIN

ApogeePerigeeFilter::ApogeePerigeeFilter(const CloseApproachOptions& opts, const CloseApproachContext& ctx)
{
    perigeeRadius_ = ctx.refElem.a_ * (1.0 - ctx.refElem.e_);
    apogeeRadius_ = ctx.refElem.a_ * (1.0 + ctx.refElem.e_);
    threshold_ = opts.apogeePerigeePad + opts.threshold;
    gm_ = ctx.gm;
}

bool ApogeePerigeeFilter::keep(const TLE& candidate) const
{
    const double a  = aMeanMotionToSMA(candidate.meanMotion(), gm_);
    const double e  = candidate.eccentricity();
    const double rp = a * (1.0 - e);
    const double ra = a * (1.0 + e);

    const double rpRef = perigeeRadius_;
    const double raRef = apogeeRadius_;

    const double threshold = threshold_;

    // 区间 [rp, ra] 与 [rpRef, raRef] 是否重叠
    return (rpRef - ra <= threshold && rp - raRef <= threshold);
}

AST_NAMESPACE_END
