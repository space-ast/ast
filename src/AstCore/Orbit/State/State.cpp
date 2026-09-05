///
/// @file      State.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#include "State.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstCore/EventTimeExplicit.hpp"
#include "AstCore/FrameWithEpoch.hpp"
#include "AstCore/FrameAssembly.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/StateKeplerian.hpp"
#include "AstCore/Resolve.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstUtil/ObjectLinker.hpp"

AST_NAMESPACE_BEGIN

PState State::NewDefault()
{
    return StateKeplerian::NewDefault();
}


HState State::MakeShared(EStateType type)
{
    return New(type);
}

HState State::MakeShared(State &state, EStateType type)
{
    return New(state, type);
}

PState State::New(EStateType type)
{
    switch (type)
    {
    case EStateType::eCartesian:
        return new StateCartesian();
        break;
    case EStateType::eKeplerian:
        return new StateKeplerian();
    default:
        break;
    }
    aError("unknown state type");
    return nullptr;
}

PState State::New(State &state, EStateType type)
{
    switch (type)
    {
    case EStateType::eCartesian:
        return new StateCartesian(state);
        break;
    case EStateType::eKeplerian:
        return new StateKeplerian(state);
    default:
        break;
    }
    aError("unknown state type");
    return nullptr;
}

State::State(const State& state)
    : frame_(), gm_{state.gm_}, stateEpoch_()
{
    auto frame = state.frame_;
    auto epoch = state.stateEpoch_;
    if(frame)
    {
        auto parentScope = frame->getParentScope();
        if(parentScope == &state || parentScope == nullptr)
        {
            auto origin = frame->getOrigin();
            auto axes = frame->getAxes();
            auto newframe = FrameAssembly::New(origin, axes);
            frame_ = newframe;
            newframe->setName(frame->getName());
            newframe->setParentScope(this);
        }
        else
        {
            frame_ = frame;
        }
    }
    if(epoch)
    {
        auto parentScope = epoch->getParentScope();
        if(parentScope == &state || parentScope == nullptr)
        {
            TimePoint tp;
            epoch->getTime(tp);
            this->setStateEpoch(tp);
        }
        else
        {
            stateEpoch_ = epoch;
        }
    }

}


void State::setFrame(Frame *frame)
{
    frame_ = frame;
    gm_ = frame_->getGM();
}

errc_t State::setFrameByName(StringView frameName)
{
    std::string name(frameName);
    State* state = this;
    auto resolveFunc = [name, state]() -> errc_t {
        auto frame = aResolveFrame(name);
        if(frame)
        {
            state->setFrame(frame);
            return 0;
        }
        else
        {
            aWarning("failed to resolve frame '%s'", name.c_str());
            return -1;
        }
    };
    errc_t rc = resolveFunc();
    if(!rc)
    {
        addDelayedLink(resolveFunc);
        return eNoError;
    }
    return eNoError;
}

errc_t State::changeFrame(Frame *frame)
{
    if (!frame || !this->frame_)
        return eErrorNullPtr;
    if(frame_ == frame)
        return eNoError;
    KinematicTransform transform;
    TimePoint epoch{};
    errc_t rc = getStateEpoch(epoch);   AST_CHECK_ERRCODE(rc, "failed to get state epoch");
    rc = aFrameTransform(this->frame_, frame, epoch, transform); AST_CHECK_ERRCODE(rc, "failed to transform frame transform");
    
    CartState state{};
    this->getState(state);
    state = transform.transformPositionVelocity(state);
    this->setState(state);
    this->setFrame(frame);
    return eNoError;
}

void State::setStateEpoch(EventTime *stateEpoch)
{
    stateEpoch_ = stateEpoch;
}



void State::setStateEpoch(const TimePoint &stateEpoch)
{
    stateEpoch_ = EventTimeExplicit::New(stateEpoch);
    stateEpoch_->setParentScope(this);
}

errc_t State::getStateEpoch(TimePoint &stateEpoch) const
{
    if(!stateEpoch_)
        return eErrorNullPtr;
    return stateEpoch_->getTime(stateEpoch);
}

TimePoint State::getStateEpoch_TimePoint() const
{
    TimePoint tp{};
    errc_t rc = getStateEpoch(tp);
    if(rc)
        aWarning("failed to get state epoch");
    return tp;
}

CelestialBody *State::getBody() const
{
    if(frame_)
        return frame_->getBody();
    return nullptr;
}

double State::getBodyRadius() const
{
    if(auto body = getBody())
        return body->getRadius();
    aError("failed to get body radius");
    return 0.0;
}

errc_t State::getStateIn(Body *body, GeodeticPoint &geodetic) const
{
    if(!body)
        return eErrorNullInput;
    auto shape = body->getShape();
    if(!shape)
        return eErrorNullPtr;
    CartState state;
    errc_t rc = getStateIn(body->getFrameFixed(), state);
    if(rc) return rc;
    shape->transform(state.pos(), geodetic);
    return eNoError;
}

errc_t State::getStateIn(Frame *frame, ModOrbElem &orbElem) const
{
    if(frame == frame_)
        return getState(orbElem);
    if(!frame)
        return eErrorNullInput;
    CartState cartState;
    errc_t rc = getState(cartState);
    if(rc) return rc;
    KinematicTransform transform;
    rc = aFrameTransform(frame_, frame, getStateEpoch_TimePoint(), transform);
    if(rc) return rc;
    cartState = transform.transformPositionVelocity(cartState);
    if(rc) return rc;
    aCartToModOrbElem(cartState.pos(), cartState.vel(), frame->getGM(), orbElem);
    return eNoError;
}


errc_t State::getStateIn(Frame *frame, CartState &state) const
{
    errc_t rc = getState(state);
    if(rc) return rc;
    if(frame == frame_)
        return eNoError;
    if(!frame)
        return eErrorNullInput;
    KinematicTransform transform;
    rc = aFrameTransform(frame_, frame, getStateEpoch_TimePoint(), transform);
    if(rc) return rc;
    state = transform.transformPositionVelocity(state);
    if(rc) return rc;
    return eNoError;
}


errc_t State::getStateInBodyInertial(Body *body, CartState&state) const
{
    if(!body)
        return eErrorNullInput;
    auto frameInertial = body->getFrameInertial();
    return getStateIn(frameInertial, state);
}


#if 0 

void State::setCoordEpoch(const TimePoint &coordEpoch)
{
    return setCoordEpoch(EventTimeExplicit::New(coordEpoch));
}

void State::setCoordEpoch(EventTime *coordEpoch)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setEpoch(coordEpoch);
    }else{
        auto point = frame_->getOrigin();
        auto axes = frame_->getAxes();
        auto frozenFrame = FrameWithEpoch::MakeShared(point, axes, coordEpoch);
        this->setFrame(frozenFrame.get());
    }
}

errc_t State::getCoordEpoch(TimePoint &coordEpoch) const
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getEpoch(coordEpoch);
    }
    return eErrorNullPtr;
}

bool State::getUseCoordEpoch() const
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getUseEpoch();
    }
    return false;
}

void State::setUseCoordEpoch(bool useCoordEpoch)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setUseEpoch(useCoordEpoch);
    }else{
        auto point = frame_->getOrigin();
        auto axes = frame_->getAxes();
        auto frozenFrame = FrameWithEpoch::MakeShared(point, axes, stateEpoch_);
        this->setFrame(frozenFrame.get());
    }
}

void State::setCoordAxes(Axes *axes)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        framewithepoch->setSourceAxes(axes);
    }else{
        auto point = frame_->getOrigin();
        auto newframe = FrameAssembly::MakeShared(point, axes);
        this->setFrame(newframe.get());
    }
}

errc_t State::changeCoordAxes(Axes *axes)
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        auto newframe = FrameWithEpoch::MakeShared(frame_->getOrigin(), axes, framewithepoch->getEpoch());
        newframe->setUseEpoch(framewithepoch->getUseEpoch());
        return changeFrame(newframe.get());
    }else{
        auto point = frame_->getOrigin();
        auto newframe = FrameAssembly::MakeShared(point, axes);
        return changeFrame(newframe.get());
    }
}

Axes *State::getCoordAxes() const
{
    if(auto framewithepoch = dynamic_cast<FrameWithEpoch*>(frame_.get()))
    {
        return framewithepoch->getSourceAxes();
    }
    return frame_->getAxes();
}
#endif

AST_NAMESPACE_END
