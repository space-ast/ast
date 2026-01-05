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


/*

参考文献：
[1] Vallado D A .Fundamentals of Astrodynamics and Applications, 4th ed.
9.6节 Linearized Perturbations and Effects里的J2长期项表达式

*/


static ModOrbElem aCartToModOrbElem(const Vector3d& r, const Vector3d& v, double gm)
{
    ModOrbElem modOrbElem;
    aCartToModOrbElem(r, v, gm, modOrbElem);
    return modOrbElem;
}


J2Analytical::J2Analytical(const ModOrbElem &modOrbElem, const TimePoint &epoch, double gm, double j2, double re)
    : modOrbElem_(modOrbElem), epoch_(epoch), gm_(gm), j2_(j2), re_(re)
    , raanDot_{0}
    , argPeriDot_{0}
    , meanAnomalyDot_{0}
{
    double n = modOrbElem_.getMeanMotion(gm_);                      ///< 平均运动数
    double p = modOrbElem_.getP();                                  ///< 半通径
    double p2 = p * p;                                              ///< 半通径的平方
    double sqrt1me2 = sqrt(1 - modOrbElem_.e()*modOrbElem_.e());    ///< 1 - e^2 的平方根
    double re2 = re_ * re_;                                         ///< 地球半径的平方
    double sini, cosi;
    sincos(modOrbElem_.i(), &sini, &cosi);
    double sini2 = sini * sini;                                     ///< 轨道倾角的正弦值的平方

    meanAnomalyDot_ =  n - 3*n*re2*j2_ * sqrt1me2 / (4 * p2) * (3 * sini2 - 2);
    raanDot_ = -3 * j2_ * re2 * n * cosi / (2 * p2);
    argPeriDot_ = 3 * n * j2_ * re2 * (4 - 5 * sini2) / (4 * p2);
}


J2Analytical::J2Analytical(const Vector3d &pos, const Vector3d &vel, const TimePoint &epoch, double gm, double j2, double re)
    : J2Analytical(aCartToModOrbElem(pos, vel, gm), epoch, gm, j2, re)
{
}

err_t J2Analytical::prop(double duration, Vector3d &r, Vector3d &v)
{
    err_t err = this->propNoNormalize(duration, modOrbElem_);
    aModOrbElemToCart(modOrbElem_, gm_, r, v);
    return err;
}

err_t J2Analytical::prop(double duration, ModOrbElem &modOrbElem)
{
    err_t err = this->propNoNormalize(duration, modOrbElem);
    modOrbElem.trueA() = aNormalizeAngle0To2Pi(modOrbElem.trueA());
    modOrbElem.raan() = aNormalizeAngle0To2Pi(modOrbElem.raan());
    modOrbElem.argper() = aNormalizeAngle0To2Pi(modOrbElem.argper());
    return err;
}

err_t J2Analytical::prop(const TimePoint &epoch, Vector3d &r, Vector3d &v)
{
    double duration = (epoch - epoch_);
    return this->prop(duration, r, v);
}

err_t J2Analytical::prop(const TimePoint &epoch, ModOrbElem &modOrbElem)
{
    double duration = (epoch - epoch_);
    return this->prop(duration, modOrbElem);
}

err_t J2Analytical::propNoNormalize(double duration, ModOrbElem &modOrbElem)
{
    modOrbElem = this->modOrbElem_;
    modOrbElem.trueA() += meanAnomalyDot_ * duration;
    modOrbElem.raan() += raanDot_ * duration;
    modOrbElem.argper() += argPeriDot_ * duration;
    return 0;
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

