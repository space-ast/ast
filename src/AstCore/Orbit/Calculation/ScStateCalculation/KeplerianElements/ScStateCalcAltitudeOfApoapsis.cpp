///
/// @file      ScStateCalcAltitudeOfApoapsis.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-02
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ScStateCalcAltitudeOfApoapsis.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t ScStateCalcAltitudeOfApoapsis::calculate(const SpacecraftState& scState, double& result)
{
    // 计算时的中心天体仅由计算量自身的配置决定，未配置时使用默认天体（地球）
    CelestialBody* body = this->body();
    if(!body)
    {
        body = aGetEarth();
        if(!body)
        {
            aError(_("未配置中心天体"));
            return eErrorNullPtr;
        }
    }

    // 将状态转换到中心天体的惯性系后再计算轨道根数
    CartState cartState;
    errc_t rc = scState.getStateInBodyInertial(body, cartState);
    if(rc != eNoError)
        return rc;

    ModOrbElem orbElem{};
    rc = aCartToModOrbElem(cartState.pos(), cartState.vel(), body->getGM(), orbElem);
    if(rc != eNoError)
        return rc;

    result = orbElem.getApoAlt(body->getRadius());
    return eNoError;
}

AST_NAMESPACE_END