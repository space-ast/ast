///
/// @file      StateGeodetic.cpp
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

#include "StateGeodetic.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

PStateGeodetic StateGeodetic::New()
{
    return new StateGeodetic();
}

HStateGeodetic StateGeodetic::MakeShared()
{
    return new StateGeodetic();
}

PStateGeodetic StateGeodetic::New(const GeodeticElem& geodeticElem)
{
    return new StateGeodetic(geodeticElem);
}

HStateGeodetic StateGeodetic::MakeShared(const GeodeticElem& geodeticElem)
{
    return new StateGeodetic(geodeticElem);
}

StateGeodetic::StateGeodetic(const GeodeticElem& geodeticElem)
    : geodeticElem_(geodeticElem)
{
}

StateGeodetic::StateGeodetic(const State& state)
    : State{state}
{
    CartState cartState;
    errc_t rc = state.getState(cartState);
    if (rc == eNoError)
        setState(cartState);
    else
        aWarning(_("由状态构造大地坐标状态失败"));
}

BodyShape* StateGeodetic::getShape() const
{
    auto body = this->getBody();
    if (body)
        return body->getShape();
    return nullptr;
}

Frame* StateGeodetic::getFixedFrame() const
{
    auto body = this->getBody();
    if (!body)
        return nullptr;
    auto fixedFrame = body->getFrameFixed();
    if (!fixedFrame)
        aError(_("中心天体未提供固连坐标系"));
    return fixedFrame;
}

errc_t StateGeodetic::getFrameToFixed(KinematicTransform& frameToFixed) const
{
    auto frame = this->getFrame();          AST_CHECK_NULLPTR(frame);
    auto fixedFrame = getFixedFrame();      AST_CHECK_NULLPTR(fixedFrame);
    TimePoint tp{};
    errc_t rc = getStateEpoch(tp);
    if (rc != eNoError)
    {
        aError(_("未设置状态历元, 无法获取固连系变换"));
        return rc;
    }
    // 必须使用运动学变换: 大地坐标的速度是固连系速度, 需要固连系的牵连速度和角速度
    rc = aFrameTransform(frame, fixedFrame, tp, frameToFixed);
    if (rc != eNoError)
    {
        aError(_("获取固连系变换失败"));
        return rc;
    }
    return eNoError;
}

errc_t StateGeodetic::getFixedToFrame(KinematicTransform& fixedToFrame) const
{
    auto frame = this->getFrame();          AST_CHECK_NULLPTR(frame);
    auto fixedFrame = getFixedFrame();      AST_CHECK_NULLPTR(fixedFrame);
    TimePoint tp{};
    errc_t rc = getStateEpoch(tp);
    if (rc != eNoError)
    {
        aError(_("未设置状态历元, 无法获取固连系变换"));
        return rc;
    }
    rc = aFrameTransform(fixedFrame, frame, tp, fixedToFrame);
    if (rc != eNoError)
    {
        aError(_("获取固连系变换失败"));
        return rc;
    }
    return eNoError;
}

errc_t StateGeodetic::getState(CartState& state) const
{
    BodyShape* shape = getShape();
    if (!shape)
    {
        aError(_("中心天体未提供有效的参考椭球"));
        return eErrorNullPtr;
    }

    // 位置与速度: 先由大地坐标在固连系下正算
    Vector3d posFixed{}, velFixed{};
    errc_t rc = aGeodeticToCart(geodeticElem_, *shape, posFixed, velFixed);
    if (rc != eNoError)
        return rc;

    // 再由固连系换算到参考坐标系
    KinematicTransform fixedToFrame;
    rc = getFixedToFrame(fixedToFrame);
    if (rc != eNoError)
        return rc;
    fixedToFrame.transformPositionVelocity(posFixed, velFixed, state.pos(), state.vel());
    return eNoError;
}

errc_t StateGeodetic::getState(ModOrbElem& orbElem) const
{
    CartState cartState;
    errc_t rc = getState(cartState);
    if (rc != eNoError)
        return rc;
    return aCartToModOrbElem(cartState.pos(), cartState.vel(), this->getGM(), orbElem);
}

errc_t StateGeodetic::setState(const CartState& state)
{
    BodyShape* shape = getShape();
    if (!shape)
    {
        aError(_("中心天体未提供有效的参考椭球"));
        return eErrorNullPtr;
    }

    // 先由参考坐标系换算到固连系
    KinematicTransform frameToFixed;
    errc_t rc = getFrameToFixed(frameToFixed);
    if (rc != eNoError)
        return rc;
    Vector3d posFixed{}, velFixed{};
    frameToFixed.transformPositionVelocity(state.pos(), state.vel(), posFixed, velFixed);

    // 再由固连系下的位置与速度反解大地坐标及其变化率
    return aCartToGeodetic(posFixed, velFixed, *shape, geodeticElem_);
}

errc_t StateGeodetic::setState(const ModOrbElem& orbElem)
{
    CartState cartState;
    errc_t rc = aModOrbElemToCart(orbElem, this->getGM(), cartState.pos(), cartState.vel());
    if (rc != eNoError)
        return rc;
    return setState(cartState);
}

errc_t StateGeodetic::setState(const GeodeticElem& geodeticElem)
{
    geodeticElem_ = geodeticElem;
    return eNoError;
}

AST_NAMESPACE_END
