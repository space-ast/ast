///
/// @file      StateSpherical.cpp
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

#include "StateSpherical.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

PStateSpherical StateSpherical::New()
{
    return new StateSpherical();
}

HStateSpherical StateSpherical::MakeShared()
{
    return new StateSpherical();
}

PStateSpherical StateSpherical::New(const SphericalElem& sphericalElem)
{
    return new StateSpherical(sphericalElem);
}

HStateSpherical StateSpherical::MakeShared(const SphericalElem& sphericalElem)
{
    return new StateSpherical(sphericalElem);
}

StateSpherical::StateSpherical(const SphericalElem& sphericalElem)
    : sphericalElem_(sphericalElem)
{
}

StateSpherical::StateSpherical(const State& state)
    : State{state}
{
    CartState cartState;
    errc_t rc = state.getState(cartState);
    if (rc == eNoError)
        setState(cartState);
    else
        aWarning(_("由状态构造球坐标状态失败"));
}

errc_t StateSpherical::getState(CartState& state) const
{
    return aSphericalToCart(sphericalElem_, state.pos(), state.vel());
}

errc_t StateSpherical::getState(ModOrbElem& orbElem) const
{
    CartState cartState;
    errc_t rc = aSphericalToCart(sphericalElem_, cartState.pos(), cartState.vel());
    if (rc != eNoError)
        return rc;
    return aCartToModOrbElem(cartState.pos(), cartState.vel(), this->getGM(), orbElem);
}

errc_t StateSpherical::setState(const CartState& state)
{
    return aCartToSpherical(state.pos(), state.vel(), sphericalElem_);
}

errc_t StateSpherical::setState(const ModOrbElem& orbElem)
{
    CartState cartState;
    errc_t rc = aModOrbElemToCart(orbElem, this->getGM(), cartState.pos(), cartState.vel());
    if (rc != eNoError)
        return rc;
    return setState(cartState);
}

errc_t StateSpherical::setState(const SphericalElem& sphericalElem)
{
    sphericalElem_ = sphericalElem;
    return eNoError;
}

AST_NAMESPACE_END
