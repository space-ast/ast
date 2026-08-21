///
/// @file      OrbitPathFilter.cpp
/// @brief     轨道路径预过滤实现
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

#include "OrbitPathFilter.hpp"
#include "AstCore/CloseApproachOptions.hpp"
#include "AstCore/CloseApproachGeometry.hpp"
#include "AstCore/OrbitGeometry.hpp"
#include "AstCore/TLE.hpp"

AST_NAMESPACE_BEGIN

OrbitPathFilter::OrbitPathFilter(const CloseApproachOptions& opts, const CloseApproachContext& ctx)
    : referenceOrbit_(ctx.refElem)
    , threshold_(opts.orbitPathPad + opts.threshold)
{
}

bool OrbitPathFilter::keep(const TLE& candidate) const
{
    const OrbElem candElem = aTLEToOrbElem(candidate, gm_);

    // 过滤器必须保守：允许假阳性，但不得误删真实交会（假阴性）。
    double dPlus = 0.0, dMinus = 0.0;
    if (aOrbitNodeLineDistances(referenceOrbit_, candElem, dPlus, dMinus) != eNoError)
    {
        // 近共面：公共交线退化，一维径向回退给出的只是距离上界，可能高估而误删，
        // 故保守保留。
        return true;
    }
    const double dist = std::min(dPlus, dMinus);
    return dist <= threshold_;
}

AST_NAMESPACE_END
