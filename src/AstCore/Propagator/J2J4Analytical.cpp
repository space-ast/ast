///
/// @file      J2J4Analytical.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "J2J4Analytical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Math.hpp"

AST_NAMESPACE_BEGIN


errc_t J2J4Analytical::prop(double duration, Vector3d &r, Vector3d &v)
{
    errc_t err = this->propNoNormalize(duration, modOrbElem_);
    aModOrbElemToCart(modOrbElem_, gm_, r, v);
    return err;
}

errc_t J2J4Analytical::prop(double duration, ModOrbElem &modOrbElem)
{
    errc_t err = this->propNoNormalize(duration, modOrbElem);
    modOrbElem.trueA() = aNormalizeAngle0To2Pi(modOrbElem.trueA());
    modOrbElem.raan() = aNormalizeAngle0To2Pi(modOrbElem.raan());
    modOrbElem.argper() = aNormalizeAngle0To2Pi(modOrbElem.argper());
    return err;
}

errc_t J2J4Analytical::prop(const TimePoint &epoch, Vector3d &r, Vector3d &v)
{
    double duration = (epoch - epoch_);
    return this->prop(duration, r, v);
}

errc_t J2J4Analytical::prop(const TimePoint &epoch, ModOrbElem &modOrbElem)
{
    double duration = (epoch - epoch_);
    return this->prop(duration, modOrbElem);
}

errc_t J2J4Analytical::propNoNormalize(double duration, ModOrbElem &modOrbElem)
{
    modOrbElem = this->modOrbElem_;
    double meanA = modOrbElem.getMeanAnomaly();
    modOrbElem.trueA() = aMeanToTrue(meanA + meanAnomalyDot_ * duration, modOrbElem.getEcc());
    modOrbElem.raan() += raanDot_ * duration;
    modOrbElem.argper() += argPeriDot_ * duration;
    return 0;
}


AST_NAMESPACE_END
