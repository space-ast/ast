///
/// @file      StateMixedSpherical.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-14
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
/// 您使用本软件所产生的风险，需由您自行承担。

#include "StateMixedSpherical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/Transform.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

PStateMixedSpherical StateMixedSpherical::New()
{
    return new StateMixedSpherical();
}

HStateMixedSpherical StateMixedSpherical::MakeShared()
{
    return new StateMixedSpherical();
}

PStateMixedSpherical StateMixedSpherical::New(const MixedSphericalElem& mixedSphericalElem)
{
    return new StateMixedSpherical(mixedSphericalElem);
}

HStateMixedSpherical StateMixedSpherical::MakeShared(const MixedSphericalElem& mixedSphericalElem)
{
    return new StateMixedSpherical(mixedSphericalElem);
}

StateMixedSpherical::StateMixedSpherical(const MixedSphericalElem& mixedSphericalElem)
    : mixedSphericalElem_(mixedSphericalElem)
{
}

StateMixedSpherical::StateMixedSpherical(const State& state)
    : State{state}
{
    CartState cartState;
    errc_t rc = state.getState(cartState);
    if (rc == eNoError)
        setState(cartState);
    else
        aWarning(_("由状态构造混合球坐标状态失败"));
}

BodyShape* StateMixedSpherical::getShape() const
{
    auto body = this->getBody();
    if (body)
        return body->getShape();
    return nullptr;
}

errc_t StateMixedSpherical::getInertialToFixed(Rotation& inertialToFixed) const
{
    auto frame = this->getFrame();      AST_CHECK_NULLPTR(frame);
    auto body = this->getBody();        AST_CHECK_NULLPTR(body);
    auto fixedFrame = body->getFrameFixed();
    if (!fixedFrame)
    {
        aError(_("中心天体未提供固连坐标系"));
        return eErrorNullPtr;
    }
    TimePoint tp{};
    errc_t rc = getStateEpoch(tp);
    if (rc != eNoError)
    {
        aError(_("未设置状态历元, 无法获取固连系旋转"));
        return rc;
    }
    rc = aAxesTransform(frame->getAxes(), fixedFrame->getAxes(), tp, inertialToFixed);
    if (rc != eNoError)
    {
        aError(_("获取固连系旋转失败"));
        return rc;
    }
    return eNoError;
}

errc_t StateMixedSpherical::getState(CartState& state) const
{
    BodyShape* shape = nullptr;
    shape = getShape();
    if (!shape)
    {
        aError(_("中心天体未提供有效的参考椭球"));
        return eErrorNullPtr;
    }
    Rotation inertialToFixed;
    errc_t rc = getInertialToFixed(inertialToFixed);
    if (rc != eNoError)
        return rc;
    return aMixedSphericalToCart(mixedSphericalElem_, inertialToFixed.inverse(), *shape, state.pos(), state.vel());
}

errc_t StateMixedSpherical::getState(ModOrbElem& orbElem) const
{
    CartState cartState;
    errc_t rc = getState(cartState);
    if (rc != eNoError)
        return rc;
    return aCartToModOrbElem(cartState.pos(), cartState.vel(), this->getGM(), orbElem);
}

errc_t StateMixedSpherical::setState(const CartState& state)
{
    BodyShape* shape = nullptr;
    shape = getShape();
    if (!shape)
    {
        aError(_("中心天体未提供有效的参考椭球"));
        return eErrorNullPtr;
    }
    Rotation inertialToFixed;
    errc_t rc = getInertialToFixed(inertialToFixed);
    if (rc != eNoError)
        return rc;
    return aCartToMixedSpherical(state.pos(), state.vel(), inertialToFixed, *shape, mixedSphericalElem_);
}

errc_t StateMixedSpherical::setState(const ModOrbElem& orbElem)
{
    CartState cartState;
    errc_t rc = aModOrbElemToCart(orbElem, this->getGM(), cartState.pos(), cartState.vel());
    if (rc != eNoError)
        return rc;
    return setState(cartState);
}

errc_t StateMixedSpherical::setState(const MixedSphericalElem& mixedSphericalElem)
{
    mixedSphericalElem_ = mixedSphericalElem;
    return eNoError;
}

AST_NAMESPACE_END
