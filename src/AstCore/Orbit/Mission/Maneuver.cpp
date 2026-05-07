///
/// @file      Maneuver.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "Maneuver.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/AxesLinkTo.hpp"
#include "AstCore/LocalOrbitFrame.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

errc_t Maneuver::execute()
{
    const auto inputState = this->getInputState();  AST_CHECK_NULLPTR(inputState);
    auto outputState = this->getOutputState();      AST_CHECK_NULLPTR(outputState);
    auto burn = this->burn();
    if(burn == nullptr)
    {
        aWarning("burn is nullptr");
    }
    else if(auto impulsive = aobject_cast<BurnImpulsive*>(burn))
    {
        Vector3d impulse = impulsive->impulse();
        Axes* thrustAxes = impulsive->axes();
        if(thrustAxes == nullptr)
            aWarning("thrustAxes is nullptr");
        else
        {
            if(AxesLinkTo* axesLink = aobject_cast<AxesLinkTo*>(thrustAxes))
            {
                std::string name = axesLink->name();
                
                // aInfo("thrustAxes is '%s'", name.c_str());

                if(name == "VNC(Earth)")
                {
                    CartState cartState;
                    errc_t rc = inputState->getStateInBodyInertial(aGetEarth(), cartState);
                    if(rc)
                        aWarning("failed to get state in body inertial");
                    Rotation rotation;
                    aVNCToFrameTransform(cartState.pos(), cartState.vel(), rotation);
                    // @todo: 这里只能用于地球预报器
                    impulse = rotation.transformVector(impulse);   // 从VNC(Earth)转换到 Earth Inertial
                    cartState.vel() += impulse;
                    TimePoint tp;
                    inputState->getStateEpoch(tp);
                    outputState->setStateEpoch(tp);
                    outputState->setState(cartState);
                    return 0;
                }
            }
            else
            {
                aWarning("unsupported thrustAxes type");
            }
        }
    }
    else
    {
        aWarning("unsupported burn type");
    }
    outputState->copyFrom(*inputState);
    return 0;
}

void Maneuver::setBurn(Burn* burn)
{
    if(burn)
        burn_ = burn;
}


AST_NAMESPACE_END


