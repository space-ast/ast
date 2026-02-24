///
/// @file      OrbitDesigner.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

#include "OrbitDesigner.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/RunTime.hpp"

AST_NAMESPACE_BEGIN

OrbitDesigner::OrbitDesigner()
    : OrbitDesigner{getDefaultCelestialBody()}
{
}


OrbitDesigner::OrbitDesigner(CelestialBody *body)
    : celestialBody_{body}
{
}

CelestialBody *OrbitDesigner::getDefaultCelestialBody()
{
    return aGetEarth();
}

err_t OrbitDesigner::getOrbitState(CartState &cartState) const
{
    ModOrbElem orbElem;
    err_t rc = getOrbitState(orbElem);
    if(rc != eNoError){
        return rc;
    }
    return aModOrbElemToCart(orbElem, getGM(), cartState.pos(), cartState.vel());
}

err_t OrbitDesigner::getOrbitState(OrbElem &orbElem) const
{
    ModOrbElem modOrbElem;
    err_t rc = getOrbitState(modOrbElem);
    if(rc != eNoError){
        return rc;
    }
    moe2coe(modOrbElem.data(), orbElem.data());
    return eNoError;
}

err_t OrbitDesigner::getCoordFrame(bool &useCoordEpoch, TimePoint &coordEpoch, SharedPtr<Frame> &coordFrame) const
{
    auto body = getCelestialBody();
    if(!body)
        return eErrorNullInput;
    useCoordEpoch = false;
    // coordFrame = body->getTODFrame();
    return eNoError;
}

AST_NAMESPACE_END


