///
/// @file      J2Analytical.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-31
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "J2Analytical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN

/// @see J4Analytical.cpp
extern void aGetJ2AnalyticalParams(const ModOrbElem &modOrbElem, double gm, double j2, double re, double &meanAnomalyDot, double &raanDot, double &argPeriDot);


J2Analytical::J2Analytical(const ModOrbElem &modOrbElem, const TimePoint &epoch, double gm, double j2, double re)
    : J2J4Analytical(modOrbElem, epoch, gm)
    , j2_(j2), re_(re)
{
    aGetJ2AnalyticalParams(modOrbElem, gm, j2, re, meanAnomalyDot_, raanDot_, argPeriDot_);
}


J2Analytical::J2Analytical(const Vector3d &pos, const Vector3d &vel, const TimePoint &epoch, double gm, double j2, double re)
    : J2Analytical(aCartToModOrbElem(pos, vel, gm), epoch, gm, j2, re)
{
}

err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, Vector3d& r, Vector3d& v)
{
    TimePoint epoch{};      // @fixme 这里的epoch是个问题，应该根据实际情况传入，目前采用默认值
    J2Analytical j2Analytical(r, v, epoch, gm, j2, re);
    return j2Analytical.prop(duration, r, v);
}


err_t aJ2AnalyticalProp(double duration, double gm, double j2, double re, ModOrbElem& modOrbElem)
{
    TimePoint epoch{};      // @fixme 这里的epoch是个问题，应该根据实际情况传入，目前采用默认值
    J2Analytical j2Analytical(modOrbElem, epoch, gm, j2, re);
    return j2Analytical.prop(duration, modOrbElem);
}



AST_NAMESPACE_END

