///
/// @file      PlanetPoint.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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

#include "PlanetPoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

#define _AST_IMPL_PLANET_POINT(NAME) \
errc_t a##NAME##PosInICRF(const TimePoint &tp, Vector3d &pos) \
{ \
    auto body = aGet##NAME(); \
    if(!body){ \
        aError(#NAME " is null, please ensure it is initialized by `aInitialize()`"); \
        return eErrorInvalidParam; \
    } \
    return body->getPosICRF(tp, pos); \
}\
errc_t a##NAME##PosVelInICRF(const TimePoint &tp, Vector3d &pos, Vector3d &vel) \
{ \
    auto body = aGet##NAME(); \
    if(!body){ \
        aError(#NAME " is null, please ensure it is initialized by `aInitialize()`"); \
        return eErrorInvalidParam; \
    } \
    return body->getPosVelICRF(tp, pos, vel); \
}

_AST_IMPL_PLANET_POINT(Mercury)
_AST_IMPL_PLANET_POINT(Venus)
_AST_IMPL_PLANET_POINT(Earth)
_AST_IMPL_PLANET_POINT(Mars)
_AST_IMPL_PLANET_POINT(Jupiter)
_AST_IMPL_PLANET_POINT(Saturn)
_AST_IMPL_PLANET_POINT(Uranus)
_AST_IMPL_PLANET_POINT(Neptune)
_AST_IMPL_PLANET_POINT(Pluto)
_AST_IMPL_PLANET_POINT(Moon)
_AST_IMPL_PLANET_POINT(Sun)
_AST_IMPL_PLANET_POINT(EMBarycenter)






AST_NAMESPACE_END

