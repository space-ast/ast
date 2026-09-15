///
/// @file      StateBPlane.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-13
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

#include "StateBPlane.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/AxesICRF.hpp"
#include "AstMath/Rotation.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

PStateBPlane StateBPlane::New()
{
    return new StateBPlane();
}

HStateBPlane StateBPlane::MakeShared()
{
    return new StateBPlane();
}

PStateBPlane StateBPlane::New(const BPlaneElem& bPlaneElem)
{
    return new StateBPlane(bPlaneElem);
}

HStateBPlane StateBPlane::MakeShared(const BPlaneElem& bPlaneElem)
{
    return new StateBPlane(bPlaneElem);
}

StateBPlane::StateBPlane(const BPlaneElem& bPlaneElem)
    : bPlaneElem_(bPlaneElem)
{
}

StateBPlane::StateBPlane(const State& state)
    : State{state}
{
    CartState cartState;
    errc_t rc = state.getState(cartState);
    if (rc == eNoError)
        setState(cartState);
    else
        aWarning(_("由状态构造B平面状态失败"));
}

void StateBPlane::setRefVector(EBPlaneRefVector refVectorType, const Vector3d& refVector)
{
    refVectorType_ = refVectorType;
    refVector_ = refVector;
}

errc_t StateBPlane::getRefVector(Vector3d& refVector, const CartState* cartState) const
{
    switch (refVectorType_)
    {
    case EBPlaneRefVector::eBodyOrbitNormal:
    {
        auto frame = this->getFrame();  AST_CHECK_NULLPTR(frame);
        auto axes = frame->getAxes();   AST_CHECK_NULLPTR(axes);
        auto body = frame->getBody();   AST_CHECK_NULLPTR(body);
        TimePoint tp{};
        errc_t rc = getStateEpoch(tp);
        if (rc != eNoError)
        {
            aError(_("未设置状态历元, 无法获取天体轨道法向"));
            return rc;
        }
        return aBodyOrbitNormalIn(*axes, *body, tp, refVector);
    }
    case EBPlaneRefVector::eSpacecraftOrbitNormal:
    {
        // 该类型下B平面参数依赖于参考向量, 只有已知直角坐标状态时才能确定参考向量
        if (!cartState)
        {
            aError(_("缺少直角坐标状态, 无法确定航天器轨道法向"));
            return eErrorInvalidParam;
        }
        auto hv = cross(cartState->pos(), cartState->vel());
        double hmag = norm(hv);
        if (hmag <= 0)
        {
            aError(_("角动量为零, 无法确定航天器轨道法向"));
            return eErrorInvalidParam;
        }
        refVector = hv / hmag;
        return eNoError;
    }
    case EBPlaneRefVector::eBodyPole:
    {
        auto body = this->getBody();
        auto frame = this->getFrame();
        if (!body || !frame)
        {
            aError(_("未设置中心天体, 无法获取天体极轴"));
            return eErrorNullPtr;
        }
        auto axesFixed = body->getAxesFixed();
        if (!axesFixed)
        {
            aError(_("缺少天体固连轴系, 无法获取天体极轴"));
            return eErrorNullPtr;
        }
        TimePoint tp{};
        errc_t rc = getStateEpoch(tp);
        if (rc != eNoError)
        {
            aError(_("未设置状态历元, 无法获取天体极轴"));
            return rc;
        }
        Rotation rotation;
        rc = aAxesTransform(axesFixed, frame->getAxes(), tp, rotation);
        if (rc != eNoError)
        {
            aError(_("计算天体轴系转换失败"));
            return rc;
        }
        // 天体极轴即天体固连系的z轴
        refVector = rotation.transformVector(Vector3d{0.0, 0.0, 1.0});
        return eNoError;
    }
    case EBPlaneRefVector::eCustom:
        refVector = refVector_;
        return eNoError;
    default:
        break;
    }
    aError(_("未知的B平面参考向量类型"));
    return eErrorInvalidParam;
}

errc_t StateBPlane::getCartesianState(CartState& state) const
{
    Vector3d refVector;
    errc_t rc = getRefVector(refVector);
    if (rc != eNoError)
        return rc;
    return aBPlaneToCart(bPlaneElem_, this->getGM(), refVector, state.pos(), state.vel());
}

errc_t StateBPlane::getState(CartState& state) const
{
    return getCartesianState(state);
}

errc_t StateBPlane::getState(ModOrbElem& orbElem) const
{
    CartState cartState;
    errc_t rc = getCartesianState(cartState);
    if (rc != eNoError)
        return rc;
    return aCartToModOrbElem(cartState.pos(), cartState.vel(), this->getGM(), orbElem);
}

errc_t StateBPlane::setState(const CartState& state)
{
    Vector3d refVector;
    errc_t rc = getRefVector(refVector, &state);
    if (rc != eNoError)
        return rc;
    return aCartToBPlane(state.pos(), state.vel(), this->getGM(), refVector, bPlaneElem_);
}

errc_t StateBPlane::setState(const ModOrbElem& orbElem)
{
    CartState cartState;
    errc_t rc = aModOrbElemToCart(orbElem, this->getGM(), cartState.pos(), cartState.vel());
    if (rc != eNoError)
        return rc;
    return setState(cartState);
}

errc_t StateBPlane::setState(const BPlaneElem& bPlaneElem)
{
    bPlaneElem_ = bPlaneElem;
    return eNoError;
}

AST_NAMESPACE_END
