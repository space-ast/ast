///
/// @file      BuiltinAxes.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "BuiltinAxes.hpp"
#include "AstCore/Coordinate.hpp"
#include "AstCore/BuiltinAxesRegistry.hpp"
#include "AstCore/MoonFrame.hpp"
#include "AstUtil/StringView.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN


static inline void aJ2000ToB1950SpiceTransform(Rotation& rotation)
{
    aJ2000ToB1950Transform_SPICE(rotation);
}

static inline void aJ2000ToB1950ValladoTransform(Rotation& rotation)
{
    aJ2000ToB1950Transform_Vallado(rotation);
}

static inline void aICRFToPATransform(const TimePoint& tp, Rotation& rotation)
{
    aICRFToMoonPrincipalAxesTransform(tp, rotation);
}

static inline void aICRFToPATransform(const TimePoint& tp, KinematicRotation& rotation)
{
    aICRFToMoonPrincipalAxesTransform(tp, rotation);
}

static inline void aICRFToMeanEarthTransform(const TimePoint& tp, Rotation& rotation)
{
    aICRFToMoonMeanEarthTransform(tp, rotation);
}

static inline void aICRFToMeanEarthTransform(const TimePoint& tp, KinematicRotation& rotation)
{
    aICRFToMoonMeanEarthTransform(tp, rotation);
}

static inline void aMeanEarthToPA403Transform(Rotation& rotation)
{
    aMoonMeanEarthToPA403Transform(rotation);
}

static inline void aMeanEarthToPA418Transform(Rotation& rotation)
{
    aMoonMeanEarthToPA418Transform(rotation);
}

static inline void aMeanEarthToPA421Transform(Rotation& rotation)
{
    aMoonMeanEarthToPA421Transform(rotation);
}

static inline void aMeanEarthToPA430Transform(Rotation& rotation)
{
    aMoonMeanEarthToPA430Transform(rotation);
}




using AxesB1950 = class AxesB1950Spice;

_AST_IMPL_AXES_DYNAMIC(ECF, ICRF)
_AST_IMPL_AXES_DYNAMIC(J2000, ECF)
_AST_IMPL_AXES_PSEUDO_STATIC(MOD, J2000)
_AST_IMPL_AXES_PSEUDO_STATIC(TOD, MOD)
_AST_IMPL_AXES_DYNAMIC(GTOD, TOD)
_AST_IMPL_AXES_PSEUDO_STATIC(TEME, TOD)
_AST_IMPL_AXES_STATIC(B1950Vallado, J2000)
_AST_IMPL_AXES_STATIC(B1950Spice, J2000)
_AST_IMPL_AXES_STATIC(FK4, B1950)
_AST_IMPL_AXES_STATIC(DE118, B1950)
_AST_IMPL_AXES_STATIC(DE96, B1950)
_AST_IMPL_AXES_STATIC(DE102, B1950)
_AST_IMPL_AXES_STATIC(DE108, B1950)
_AST_IMPL_AXES_STATIC(DE111, B1950)
_AST_IMPL_AXES_STATIC(DE114, B1950)
_AST_IMPL_AXES_STATIC(DE122, B1950)
_AST_IMPL_AXES_STATIC(DE125, B1950)
_AST_IMPL_AXES_STATIC(DE130, B1950)
_AST_IMPL_AXES_STATIC(Galactic, FK4)
_AST_IMPL_AXES_STATIC(DE200, J2000)
_AST_IMPL_AXES_STATIC(DE202, J2000)
_AST_IMPL_AXES_STATIC(MarsIAU, J2000)
_AST_IMPL_AXES_STATIC(EclipJ2000, J2000)
_AST_IMPL_AXES_STATIC(EclipB1950, B1950)
_AST_IMPL_AXES_STATIC(DE140, J2000)
_AST_IMPL_AXES_STATIC(DE142, J2000)
_AST_IMPL_AXES_STATIC(DE143, J2000)
_AST_IMPL_AXES_DYNAMIC(PA, ICRF)
_AST_IMPL_AXES_DYNAMIC(MeanEarth, ICRF)
_AST_IMPL_AXES_STATIC(PA403, MeanEarth)
_AST_IMPL_AXES_STATIC(PA418, MeanEarth)
_AST_IMPL_AXES_STATIC(PA421, MeanEarth)
_AST_IMPL_AXES_STATIC(PA430, MeanEarth)



Axes *aGetAxes(StringView name)
{
    return BuiltinAxesRegistry::Instance().getAxes(name);
}


AST_NAMESPACE_END
