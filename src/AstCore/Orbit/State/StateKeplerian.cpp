///
/// @file      StateKeplerian.cpp
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

#include "StateKeplerian.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstCore/RunTime.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Literals.hpp"
#include <climits>

AST_NAMESPACE_BEGIN


/*
轨道参数的依赖关系

SMA, MeanMotion, Period:     
ApoAlt, ApoRad:             
PeriAlt, PeriRad:         

Ecc:                        

Inc: 

RAAN:
LAN: RAAN, TrueAnomaly, ArgPeri, SMA, Ecc

ArgPeri:                   

TrueAnomaly:               
MeanAnomaly: TrueAnomaly, Ecc
EccAnomaly: TrueAnomaly, Ecc
ArgLat: TrueAnomaly, ArgPeri                  
TimePastPeri, TimeOfPeriPassage: TrueAnomaly, SMA, Ecc
TimePastAscNode, TimeOfAscNodePassage: TrueAnomaly, ArgPeri, SMA, Ecc

*/


PStateKeplerian StateKeplerian::New()
{
    return new StateKeplerian();
}

HStateKeplerian StateKeplerian::MakeShared()
{
    return new StateKeplerian();
}

PStateKeplerian StateKeplerian::New(const ModOrbElem &modOrbElem)
{
    return new StateKeplerian(modOrbElem);
}

HStateKeplerian StateKeplerian::MakeShared(const ModOrbElem &modOrbElem)
{
    return new StateKeplerian(modOrbElem);
}

static void initDefault(StateKeplerian &stateKeplerian)
{
    auto body = aGetEarth();
    if(body)
    {
        auto frame = body->makeFrameInertial();
        frame->setName("Inertial");
        stateKeplerian.setStateEpoch(TimePoint::Default());
        stateKeplerian.setFrame(frame);
        stateKeplerian.setState(ModOrbElem{
            body->getRadius() + 300_km,
            0,
            28.5_deg,
            0,
            0,
            0
        });
    }
}

PStateKeplerian StateKeplerian::NewDefault()
{
    auto stateKeplerian = new StateKeplerian();
    #if 0
    initDefault(*stateKeplerian);
    #endif
    return stateKeplerian;
}

StateKeplerian::StateKeplerian()
{
    initDefault(*this);
}

StateKeplerian::StateKeplerian(const ModOrbElem &modOrbElem)
    : modOrbElem_(modOrbElem)
{
}

StateKeplerian::StateKeplerian(const State &state)
    : State{state}
{
    if (state.getStateType() == EStateType::eKeplerian)
    {
        static_cast<const StateKeplerian&>(state).getState(modOrbElem_);
    }else{
        CartState cartState{};
        errc_t rc = state.getState(cartState);
        if(rc) return;
        this->setState(cartState);
    }
}

errc_t StateKeplerian::getState(CartState &state) const
{
    return aModOrbElemToCart(modOrbElem_, getGM(), state.pos(), state.vel());
}

errc_t StateKeplerian::setState(const CartState &state)
{
    return aCartToModOrbElem(state.pos(), state.vel(), getGM(), modOrbElem_);
}

void StateKeplerian::setState(OrbElem &orbElem)
{
    return coe2moe(orbElem.data(), modOrbElem_.data());
}

void StateKeplerian::getState(OrbElem &orbElem) const
{
    errc_t rc = moe2coe(modOrbElem_.data(), orbElem.data());
    A_UNUSED(rc);
}

void StateKeplerian::getInnerRepresentationState(array6d &stateInRepresentation) const
{
    return getStateInRepresentation(
        this->sizeType_, this->shapeType_, 
        this->orientationType_, this->positionType_, 
        stateInRepresentation
    );
}

void StateKeplerian::getStateInRepresentation(
    ESizeType sizeType, EShapeType shapeType, 
    EOrientationType orientationType, EPositionType positionType, 
    array6d &stateInRepresentation) const
{
    stateInRepresentation[0] = getSizeParam(sizeType);
    stateInRepresentation[1] = getShapeParam(shapeType);
    stateInRepresentation[2] = getInc();
    stateInRepresentation[3] = getOrientationParam(orientationType);
    stateInRepresentation[4] = getArgPeri();
    stateInRepresentation[5] = getPositionParam(positionType);
}

double StateKeplerian::getStateParam(int index)
{
    switch (index)
    {
    case 0:
        return getSizeParam();
    case 1:
        return getShapeParam();
    case 2:
        return getInc();
    case 3:
        return getOrientationParam();
    case 4:
        return getArgPeri();
    case 5:
        return getPositionParam();
    default:
        break;
    }
    return std::numeric_limits<double>::quiet_NaN();
}

void StateKeplerian::setStateParam(int index, double value)
{
    switch (index)
    {
    case 0:
        setSizeParam(value);
        break;
    case 1:
        setShapeParam(value);
        break;
    case 2:
        setInc(value);
        break;
    case 3:
        setOrientationParam(value);
        break;
    case 4:
        setArgPeri(value);
        break;
    case 5:
        setPositionParam(value);
        break;
    default:
        break;
    }
}

double StateKeplerian::getSizeParam() const
{
    return getSizeParam(this->sizeType_);
}

double StateKeplerian::getSizeParam(ESizeType sizeType) const
{
    switch (sizeType)
    {
    case ESizeType::eSMA:
        return this->getSMA();
    case ESizeType::eApoAlt:
        return this->getApoAlt();
    case ESizeType::eApoRad:
        return this->getApoRad();
    case ESizeType::ePeriAlt:
        return this->getPeriAlt();
    case ESizeType::ePeriRad:
        return this->getPeriRad();
    case ESizeType::ePeriod:
        return this->getPeriod();
    case ESizeType::eMeanMotion:
        return this->getMeanMotion();
    default:
        aError("unknown sizeType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setSizeParam(double sizeParam, ESizeType sizeType)
{
    switch (sizeType)
    {
    case ESizeType::eSMA:
        this->setSMA(sizeParam);
        break;
    case ESizeType::eApoAlt:
        this->setApoAltForSize(sizeParam);
        break;
    case ESizeType::eApoRad:
        this->setApoRadForSize(sizeParam);
        break;
    case ESizeType::ePeriAlt:
        this->setPeriAltForSize(sizeParam);
        break;
    case ESizeType::ePeriRad:
        this->setPeriRadForSize(sizeParam);
        break;
    case ESizeType::ePeriod:
        this->setPeriod(sizeParam);
        break;
    case ESizeType::eMeanMotion:
        this->setMeanMotion(sizeParam);
        break;
    default:
        aError("unknown sizeType");
        break;
    }
}

void StateKeplerian::setSizeParam(double sizeParam)
{
    this->setSizeParam(sizeParam, this->sizeType_);
}

void StateKeplerian::setSizeType(ESizeType sizeType)
{
    if(sizeType == ESizeType::eApoAlt || sizeType == ESizeType::eApoRad)
    {
        if(this->shapeType_ == EShapeType::eApoAlt || this->shapeType_ == EShapeType::eApoRad)
        {
            aError("sizeType %d is not supported for shapeType %d", sizeType, this->shapeType_);
            return;
        }
    }
    if(sizeType == ESizeType::ePeriAlt || sizeType == ESizeType::ePeriRad)
    {
        if(this->shapeType_ == EShapeType::ePeriAlt || this->shapeType_ == EShapeType::ePeriRad)
        {
            aError("sizeType %d is not supported for shapeType %d", sizeType, this->shapeType_);
            return;
        }
    }
    this->sizeType_ = sizeType;
}


void StateKeplerian::setShapeType(EShapeType shapeType)
{
    if(shapeType == EShapeType::eApoAlt || shapeType == EShapeType::eApoRad)
    {
        if(this->sizeType_ == ESizeType::eApoAlt || this->sizeType_ == ESizeType::eApoRad)
        {
            aError("shapeType %d is not supported for sizeType %d", shapeType, this->sizeType_);
            return;
        }
    }
    if(shapeType == EShapeType::ePeriAlt || shapeType == EShapeType::ePeriRad)
    {
        if(this->sizeType_ == ESizeType::ePeriAlt || this->sizeType_ == ESizeType::ePeriRad)
        {
            aError("shapeType %d is not supported for sizeType %d", shapeType, this->sizeType_);
            return;
        }
    }
    this->shapeType_ = shapeType;
}


double StateKeplerian::getShapeParam() const
{
    return getShapeParam(this->shapeType_);
}

double StateKeplerian::getShapeParam(EShapeType shapeType) const
{
    switch (shapeType)
    {
    case EShapeType::eApoAlt:
        return this->getApoAlt();
    case EShapeType::ePeriAlt:
        return this->getPeriAlt();
    case EShapeType::eEcc:
        return this->getEcc();
    case EShapeType::eApoRad:
        return this->getApoRad();
    case EShapeType::ePeriRad:
        return this->getPeriRad();
    default:
        aError("unknown shapeType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setShapeParam(double shapeParam, EShapeType shapeType)
{
    switch (shapeType)
    {
    case EShapeType::eApoAlt:
        this->setApoAltForShape(shapeParam);
        break;
    case EShapeType::ePeriAlt:
        this->setPeriAltForShape(shapeParam);
        break;
    case EShapeType::eEcc:
        this->setEcc(shapeParam);
        break;
    case EShapeType::eApoRad:
        this->setApoRadForShape(shapeParam);
        break;
    case EShapeType::ePeriRad:
        this->setPeriRadForShape(shapeParam);
        break;
    default:
        aError("unknown shapeType");
        break;
    }
}

void StateKeplerian::setShapeParam(double shapeParam)
{
    this->setShapeParam(shapeParam, this->shapeType_);
}

double StateKeplerian::getOrientationParam() const
{
    return getOrientationParam(this->orientationType_);
}

double StateKeplerian::getOrientationParam(EOrientationType orientationType) const
{
    switch (orientationType)
    {
    case EOrientationType::eRAAN:
        return this->getRAAN();
    case EOrientationType::eLAN:
        return this->getLAN();
    default:
        aError("unknown orientationType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setOrientationParam(double orientationParam, EOrientationType orientationType)
{
    switch (orientationType)
    {
    case EOrientationType::eRAAN:
        this->setRAAN(orientationParam);
        break;
    case EOrientationType::eLAN:
        this->setLAN(orientationParam);
        break;
    default:
        aError("unknown orientationType");
        break;
    }
}

void StateKeplerian::setOrientationParam(double orientationParam)
{
    this->setOrientationParam(orientationParam, this->orientationType_);
}

void StateKeplerian::setOrientationType(EOrientationType orientationType)
{
    this->orientationType_ = orientationType;
}

double StateKeplerian::getPositionParam() const
{
    return getPositionParam(this->positionType_);
}

double StateKeplerian::getPositionParam(EPositionType positionType) const
{
    switch (positionType)
    {
    case EPositionType::eTrueAnomaly:
        return this->getTrueAnomaly();
    case EPositionType::eMeanAnomaly:
        return this->getMeanAnomaly();
    case EPositionType::eEccAnomaly:
        return this->getEccAnomaly();
    case EPositionType::eArgLat:
        return this->getArgLat();
    case EPositionType::eTimePastAscNode:
        return this->getTimePastAscNode();
    case EPositionType::eTimePastPeri:
        return this->getTimePastPeri();
    case EPositionType::eTimeOfAscNodePassage:
        return this->getTimeOfAscNodePassage().toEpochSecond();
    case EPositionType::eTimeOfPeriPassage:
        return this->getTimeOfPeriPassage().toEpochSecond();
    default:
        aError("unknown positionType");
        return std::numeric_limits<double>::quiet_NaN();
    }
}

void StateKeplerian::setPositionParam(double positionParam, EPositionType positionType)
{
    switch (positionType)
    {
    case EPositionType::eTrueAnomaly:
        this->setTrueAnomaly(positionParam);
        break;
    case EPositionType::eMeanAnomaly:
        this->setMeanAnomaly(positionParam);
        break;
    case EPositionType::eEccAnomaly:
        this->setEccAnomaly(positionParam);
        break;
    case EPositionType::eArgLat:
        this->setArgLat(positionParam);
        break;
    case EPositionType::eTimePastAscNode:
        this->setTimePastAscNode(positionParam);
        break;
    case EPositionType::eTimePastPeri:
        this->setTimePastPeri(positionParam);
        break;
    case EPositionType::eTimeOfAscNodePassage:
        this->setTimeOfAscNodePassage(positionParam);
        break;
    case EPositionType::eTimeOfPeriPassage:
        this->setTimeOfPeriPassage(positionParam);
        break;
    default:
        aError("unknown positionType");
        break;
    }
}

void StateKeplerian::setPositionParam(double positionParam)
{
    this->setPositionParam(positionParam, this->positionType_);
}

void StateKeplerian::setPositionType(EPositionType positionType)
{
    this->positionType_ = positionType;
}

void StateKeplerian::setSMA(double sma)
{
    const ModOrbElem originalElem = modOrbElem_;
    switch (this->shapeType_)
    {
    case EShapeType::eApoAlt:
    case EShapeType::eApoRad:
    {
        double apoRad = this->getApoRad();
        if(sma > apoRad)
        {
            aError("sma must not be greater than apoRad");
            return;
        }
        double ecc = (this->getApoRad() - sma) / sma;
        double periRad = aSMAToPeriRad(sma, ecc);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case EShapeType::ePeriAlt:
    case EShapeType::ePeriRad:
    {
        double periRad = this->getPeriRad();
        if(sma < periRad)
        {
            aError("sma must not be less than periRad");
            return;
        }
        double ecc = (sma - periRad) / sma;
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case EShapeType::eEcc:
    {
        double ecc = this->getEcc();
        double periRad = aSMAToPeriRad(sma, ecc);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        break;
    }
    default:
        aError("unknown shapeType");
        break;
    }
}

double StateKeplerian::getMeanMotion() const
{
    return modOrbElem_.getMeanMotion(getGM());
}

void StateKeplerian::setMeanMotion(double meanMotion)
{
    double sma = aMeanMotionToSMA(meanMotion, getGM());
    return this->setSMA(sma);
}

double StateKeplerian::getPeriod() const
{
    return modOrbElem_.getPeriod(getGM());
}

void StateKeplerian::setPeriod(double period)
{
    double sma = aPeriodToSMA(period, getGM());
    return this->setSMA(sma);
}

double StateKeplerian::getApoAlt() const
{
    return modOrbElem_.getApoAlt(getBodyRadius());
}

void StateKeplerian::setApoAltForSize(double apoAlt)
{
    double apoRad = aApoAltToApoRad(apoAlt, getBodyRadius());
    return this->setApoRadForSize(apoRad);
}

void StateKeplerian::setApoAltForShape(double apoAlt)
{
    double apoRad = aApoAltToApoRad(apoAlt, getBodyRadius());
    return this->setApoRadForShape(apoRad);
}

double StateKeplerian::getApoRad() const
{
    return modOrbElem_.getApoRad();
}

void StateKeplerian::setApoRadForSize(double apoRad)
{
    const ModOrbElem originalElem = modOrbElem_;
    switch (shapeType_)
    {
    case EShapeType::eApoAlt:
    case EShapeType::eApoRad:
    {
        aError("invalid combination of sizeType and shapeType");
        break;
    }
    case EShapeType::ePeriAlt:
    case EShapeType::ePeriRad:
    {
        double periRad = this->getPeriRad();
        if(periRad > apoRad){
            aError("periRad must not be greater than apoRad");
            return;
        }
        double ecc = aPeriRadApoRadToEcc(this->getPeriRad(), apoRad);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case EShapeType::eEcc:
    {
        double ecc = modOrbElem_.getEcc();
        double rp = aApoRadToPeriRad(apoRad, ecc);
        this->changePeriRadHoldingOthers(rp, originalElem);
        break;
    }
    default:
        break;
    }
}

void StateKeplerian::setApoRadForShape(double apoRad)
{
    const ModOrbElem originalElem = modOrbElem_;
    switch (this->sizeType_)
    {
    case ESizeType::eApoAlt:
    case ESizeType::eApoRad:
    {
        aError("invalid combination of sizeType and shapeType");
        break;
    }
    case ESizeType::ePeriAlt:
    case ESizeType::ePeriRad:
    {
        double periRad = this->getPeriRad();
        if(periRad > apoRad){
            aError("periRad must not be greater than apoRad");
            return;
        }
        double ecc = aPeriRadApoRadToEcc(periRad, apoRad);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case ESizeType::eSMA:
    case ESizeType::ePeriod:
    case ESizeType::eMeanMotion:
    {
        double sma = this->getSMA();
        if(apoRad < sma)
        {
            aError("apoRad must not be less than sma");
            return;
        }
        double ecc = aApoRadToEcc(apoRad, sma);
        double periRad = 2 * sma - apoRad;
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    default:
        break;
    }
}

double StateKeplerian::getPeriAlt() const
{
    return modOrbElem_.getPeriAlt(getBodyRadius());
}

void StateKeplerian::setPeriAltForSize(double periAlt)
{
    double periRad = aPeriAltToPeriRad(periAlt, getBodyRadius());
    return this->setPeriRadForSize(periRad);
}

void StateKeplerian::setPeriAltForShape(double periAlt)
{
    double periRad = aPeriAltToPeriRad(periAlt, getBodyRadius());
    return this->setPeriRadForShape(periRad);
}

double StateKeplerian::getPeriRad() const
{
    return modOrbElem_.getPeriRad();
}

void StateKeplerian::setPeriRadForSize(double periRad)
{
    const ModOrbElem originalElem = modOrbElem_;
    switch (this->shapeType_)
    {
    case EShapeType::eApoAlt:
    case EShapeType::eApoRad:
    {
        double apoRad = this->getApoRad();
        if(periRad > apoRad)
        {
            aError("periRad must not be greater than apoRad");
            return;
        }
        double ecc = aPeriRadApoRadToEcc(periRad, apoRad);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case EShapeType::ePeriAlt:
    case EShapeType::ePeriRad:
    {
        aError("invalid combination of sizeType and shapeType");
        break;
    }
    case EShapeType::eEcc:
        this->changePeriRadHoldingOthers(periRad, originalElem);
        break;
    default:
        aError("unknown shapeType");
        break;
    }
}

void StateKeplerian::setPeriRadForShape(double periRad)
{
    const ModOrbElem originalElem = modOrbElem_;
    switch (sizeType_)
    {
    case ESizeType::eApoAlt:
    case ESizeType::eApoRad:
    {
        double apoRad = this->getApoRad();
        if(periRad > apoRad)
        {
            aError("periRad must not be greater than apoRad");
            return;
        }

        double ecc = aPeriRadApoRadToEcc(periRad, this->getApoRad());
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case ESizeType::ePeriAlt:
    case ESizeType::ePeriRad:
    {
        aError("invalid combination of sizeType and shapeType");
        break;
    }
    case ESizeType::eMeanMotion:
    case ESizeType::ePeriod:
    case ESizeType::eSMA:
    {
        double sma = this->getSMA();
        if(periRad > sma)
        {
            aError("periRad must not be greater than sma");
            return;
        }
        double ecc = aPeriRadToEcc(periRad, sma);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    default:
        break;
    }
}

double StateKeplerian::getEcc() const
{
    return modOrbElem_.getEcc();
}

void StateKeplerian::setEcc(double ecc)
{
    ModOrbElem originalElem = modOrbElem_;
    switch (sizeType_)
    {
    case ESizeType::eApoAlt:
    case ESizeType::eApoRad:
    {
        double apoRad = this->getApoRad();
        double periRad = aApoRadToPeriRad(apoRad, ecc);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    case ESizeType::ePeriAlt:
    case ESizeType::ePeriRad:
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    case ESizeType::eMeanMotion:
    case ESizeType::ePeriod:
    case ESizeType::eSMA:
    {
        double periRad = aSMAToPeriRad(this->getSMA(), ecc);
        this->changePeriRadHoldingOthers(periRad, originalElem);
        this->changeEccHoldingOthers(ecc, originalElem);
        break;
    }
    default:
        break;
    }
}

double StateKeplerian::getInc() const
{
    return modOrbElem_.getInc();
}

void StateKeplerian::setInc(double inc)
{
    modOrbElem_.i_ = inc;
}

double StateKeplerian::getRAAN() const
{
    return modOrbElem_.getRAAN();
}

void StateKeplerian::setRAAN(double raan)
{
    modOrbElem_.raan_ = raan;
}

double StateKeplerian::getLAN() const
{
    auto body = this->getBody();
    if(!body)
    {
        aError("failed to get body");
        return std::numeric_limits<double>::quiet_NaN();
    }

    TimePoint timeOfAscNodePassage{};
    this->getTimeOfAscNodePassage(timeOfAscNodePassage);
    return modOrbElem_.getLAN(frame_->getAxes(), timeOfAscNodePassage, body->getAxesFixed());
}

void StateKeplerian::setLAN(double lan)
{
    auto body = this->getBody();
    if(!body)
    {
        aError("failed to get body");
        return;
    }

    TimePoint timeOfAscNodePassage{};
    this->getTimeOfAscNodePassage(timeOfAscNodePassage);
    double raan = aLANToRAAN(lan, body->getAxesFixed(), timeOfAscNodePassage, frame_->getAxes());
    modOrbElem_.raan_ = raan;
}

double StateKeplerian::getSMA() const
{
    return modOrbElem_.getSMA();
}

double StateKeplerian::getArgPeri() const
{
    return modOrbElem_.getArgPeri();
}

void StateKeplerian::setArgPeri(double argPeri)
{
    const ModOrbElem originalElem = modOrbElem_;
    changeArgPeriHoldingOthers(argPeri, originalElem);
}

double StateKeplerian::getTrueAnomaly() const
{
    return modOrbElem_.getTrueAnomaly();
}

void StateKeplerian::setTrueAnomaly(double trueAnomaly)
{
    modOrbElem_.trueA_ = trueAnomaly;
}

double StateKeplerian::getMeanAnomaly() const
{
    return modOrbElem_.getMeanAnomaly();
}

void StateKeplerian::setMeanAnomaly(double meanAnomaly)
{
    double ecc = this->getEcc();
    double trueAnomaly = aMeanToTrue(meanAnomaly, ecc);
    modOrbElem_.trueA_ = trueAnomaly;
}

double StateKeplerian::getEccAnomaly() const
{
    return modOrbElem_.getEccAnomaly();
}

void StateKeplerian::setEccAnomaly(double eccAnomaly)
{
    double ecc = this->getEcc();
    double trueAnomaly = aEccToTrue(eccAnomaly, ecc);
    modOrbElem_.trueA_ = trueAnomaly;
}

double StateKeplerian::getArgLat() const
{
    return modOrbElem_.getArgLat();
}

void StateKeplerian::setArgLat(double argLat)
{
    double argPeri = modOrbElem_.getArgPeri();
    double trueAnomaly = argLat - argPeri;
    modOrbElem_.trueA_ = trueAnomaly;
}

double StateKeplerian::getTimePastPeri() const
{
    return modOrbElem_.getTimePastPeri(getGM());
}

void StateKeplerian::setTimePastPeri(double timePastPeri)
{
    double sma = this->getSMA();
    double ecc = this->getEcc();
    double trueAnomaly = aTimePastPeriToTrue(timePastPeri, sma, ecc, getGM());
    modOrbElem_.trueA_ = trueAnomaly;
}

double StateKeplerian::getTimePastAscNode() const
{
    return modOrbElem_.getTimePastAscNode(getGM());
}

void StateKeplerian::setTimePastAscNode(double timePastAscNode)
{
    double argPeri = this->getArgPeri();
    double sma = this->getSMA();
    double ecc = this->getEcc();
    double trueAnomaly = aTimePastAscNodeToTrue(timePastAscNode, argPeri, sma, ecc, getGM());
    modOrbElem_.trueA_ = trueAnomaly;
}

void StateKeplerian::getTimeOfPeriPassage(TimePoint &tp) const
{
    TimePoint stateEpoch{};
    errc_t rc = this->getStateEpoch(stateEpoch);
    A_UNUSED(rc);
    tp = stateEpoch - this->getTimePastPeri();
}

TimePoint StateKeplerian::getTimeOfPeriPassage() const
{
    TimePoint tp{};
    this->getTimeOfPeriPassage(tp);
    return tp;
}

void StateKeplerian::setTimeOfPeriPassage(const TimePoint &tp)
{
    TimePoint stateEpoch{};
    errc_t rc = this->getStateEpoch(stateEpoch);
    if(rc) return;
    double timePastPeri = stateEpoch - tp;
    return setTimePastPeri(timePastPeri);
}

void StateKeplerian::setTimeOfPeriPassage(double epochsecond)
{
    aError("setTimeOfPeriPassage not implemented");
}

void StateKeplerian::getTimeOfAscNodePassage(TimePoint &tp) const
{
    TimePoint stateEpoch{};
    errc_t rc = this->getStateEpoch(stateEpoch);
    A_UNUSED(rc);
    tp = stateEpoch - this->getTimePastAscNode();
}

TimePoint StateKeplerian::getTimeOfAscNodePassage() const
{
    TimePoint tp{};
    this->getTimeOfAscNodePassage(tp);
    return tp;
}

void StateKeplerian::setTimeOfAscNodePassage(const TimePoint &tp)
{
    TimePoint stateEpoch{};
    errc_t rc = this->getStateEpoch(stateEpoch);
    if(rc) return;
    double timePastAscNode = stateEpoch - tp;
    return setTimePastAscNode(timePastAscNode);
}

void StateKeplerian::setTimeOfAscNodePassage(double epochsecond)
{
    TimePoint tp{};
    aEpochSecondToTimePoint(epochsecond, tp);
    this->setTimeOfAscNodePassage(tp);
}

void StateKeplerian::changeEccHoldingOthers(double ecc, const ModOrbElem& originalElem)
{
    modOrbElem_.e() = ecc;
    switch(positionType_)
    {
    case EPositionType::eMeanAnomaly:
    {
        this->holdMeanAnomaly(originalElem);
        break;
    }
    case EPositionType::eEccAnomaly:
    {
        this->holdEccAnomaly(originalElem);
        break;
    }
    case EPositionType::eArgLat:
    {
        this->holdArgLat(originalElem);
        break;
    }
    case EPositionType::eTimePastAscNode:
    case EPositionType::eTimeOfAscNodePassage:
    {
        this->holdTimePastAscNode(originalElem);
        break;
    }
    case EPositionType::eTimePastPeri:
    case EPositionType::eTimeOfPeriPassage:
    {
        this->holdTimePastPeri(originalElem);
        break;
    }
    case EPositionType::eTrueAnomaly:
    default:
        break;
    }
    if(orientationType_ == EOrientationType::eLAN)
    {
        holdLAN(originalElem);
    }
}

void StateKeplerian::changePeriRadHoldingOthers(double periRad, const ModOrbElem &originalElem)
{
    modOrbElem_.rp_ = periRad;
    switch (positionType_)
    {
    case EPositionType::eTimePastAscNode:
    case EPositionType::eTimeOfAscNodePassage:
    {
        this->holdTimePastAscNode(originalElem);
        break;
    }
    case EPositionType::eTimePastPeri:
    case EPositionType::eTimeOfPeriPassage:
    {
        this->holdTimePastPeri(originalElem);
        break;
    }
    default:
        break;
    }
    if(orientationType_ == EOrientationType::eLAN)
    {
        holdLAN(originalElem);
    }
}

void StateKeplerian::changeArgPeriHoldingOthers(double argPeri, const ModOrbElem &originalElem)
{
    modOrbElem_.argper_ = argPeri;
    switch (positionType_)
    {
    case EPositionType::eTimeOfAscNodePassage:
    case EPositionType::eTimePastAscNode:
    {
        this->holdTimePastAscNode(originalElem);
        break;
    }
    default:
        break;
    }
    if(orientationType_ == EOrientationType::eLAN)
    {
        holdLAN(originalElem);
    }
}

void StateKeplerian::holdLAN(const ModOrbElem &originalElem)
{
    auto body = this->getBody();
    if(!body)
    {
        aError("failed to get body");
        return;
    }
    TimePoint stateEpoch = this->getStateEpoch_TimePoint();
    TimePoint timeOfAscNodePassage = originalElem.getTimeOfAscNodePassage(stateEpoch, getGM());
    auto inertialAxes = frame_->getAxes();
    auto bodyFixedAxes = body->getAxesFixed();
    double lan = originalElem.getLAN(inertialAxes, timeOfAscNodePassage, bodyFixedAxes);
    // printf("lan: %.15g\n", lan);
    double raan = aLANToRAAN(lan, bodyFixedAxes, this->getTimeOfAscNodePassage(), inertialAxes);
    modOrbElem_.raan() = raan;
}

void StateKeplerian::holdMeanAnomaly(const ModOrbElem &originalElem)
{
    double meanAnomaly = originalElem.getMeanAnomaly();
    double trueAnomaly = aMeanToTrue(meanAnomaly, this->getEcc());
    modOrbElem_.trueA() = trueAnomaly;
}

void StateKeplerian::holdEccAnomaly(const ModOrbElem &originalElem)
{
    double eccAnomaly = originalElem.getEccAnomaly();
    double trueAnomaly = aEccToTrue(eccAnomaly, this->getEcc());
    modOrbElem_.trueA() = trueAnomaly;
}

void StateKeplerian::holdArgLat(const ModOrbElem &originalElem)
{
    double argLat = originalElem.getArgLat();
    double trueAnomaly = aArgLatToTrue(argLat, this->getArgPeri());
    modOrbElem_.trueA() = trueAnomaly;
}

void StateKeplerian::holdTimePastAscNode(const ModOrbElem &originalElem)
{
    double timePastAscNode = originalElem.getTimePastAscNode(getGM());
    double trueAnomaly = aTimePastAscNodeToTrue(
        timePastAscNode, this->getArgPeri(), 
        this->getSMA(), this->getEcc(), getGM());
    modOrbElem_.trueA() = trueAnomaly;
}

void StateKeplerian::holdTimePastPeri(const ModOrbElem &originalElem)
{
    double timePastPeri = originalElem.getTimePastPeri(getGM());
    double trueAnomaly = aTimePastPeriToTrue(
        timePastPeri, this->getSMA(), this->getEcc(), getGM());
    modOrbElem_.trueA() = trueAnomaly;
}

AST_NAMESPACE_END
