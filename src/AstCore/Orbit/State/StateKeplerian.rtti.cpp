#include "StateKeplerian.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class StateKeplerian::staticType;

static bool StateKeplerian_ClassInited = (StateKeplerian::ClassInit(&StateKeplerian::staticType), true);

void StateKeplerian::ClassInit(Class* cls)
{

    cls->setName("StateKeplerian");
    cls->setDesc(u8R"(开普勒轨道根数状态)");
    cls->addToRegistry();
    cls->setParent<State>();
    cls->setConstructor<StateKeplerian>();

    cls->addProperty("SMA", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getSMA, &StateKeplerian::setSMA>(Dimension::Length()));
    cls->addProperty("Period", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriod, &StateKeplerian::setPeriod>(Dimension::Time()));
    cls->addProperty("MeanMotion", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getMeanMotion, &StateKeplerian::setMeanMotion>(Dimension::AngularVelocity()));
    cls->addProperty("ApoRadForSize", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoRadForSize, &StateKeplerian::setApoRadForSize>(Dimension::Length()));
    cls->addProperty("ApoAltForSize", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoAltForSize, &StateKeplerian::setApoAltForSize>(Dimension::Length()));
    cls->addProperty("PeriRadForSize", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriRadForSize, &StateKeplerian::setPeriRadForSize>(Dimension::Length()));
    cls->addProperty("PeriAltForSize", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriAltForSize, &StateKeplerian::setPeriAltForSize>(Dimension::Length()));
    cls->addProperty("Ecc", aNewPropertyDouble<StateKeplerian, &StateKeplerian::getEcc, &StateKeplerian::setEcc>());
    cls->addProperty("ApoRadForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoRadForShape, &StateKeplerian::setApoRadForShape>(Dimension::Length()));
    cls->addProperty("ApoAltForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getApoAltForShape, &StateKeplerian::setApoAltForShape>(Dimension::Length()));
    cls->addProperty("PeriRadForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriRadForShape, &StateKeplerian::setPeriRadForShape>(Dimension::Length()));
    cls->addProperty("PeriAltForShape", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getPeriAltForShape, &StateKeplerian::setPeriAltForShape>(Dimension::Length()));
    cls->addProperty("Inc", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getInc, &StateKeplerian::setInc>(Dimension::Angle()));
    cls->addProperty("RAAN", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getRAAN, &StateKeplerian::setRAAN>(Dimension::Angle()));
    cls->addProperty("LAN", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getLAN, &StateKeplerian::setLAN>(Dimension::Angle()));
    cls->addProperty("ArgPeri", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getArgPeri, &StateKeplerian::setArgPeri>(Dimension::Angle()));
    cls->addProperty("TrueAnomaly", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTrueAnomaly, &StateKeplerian::setTrueAnomaly>(Dimension::Angle()));
    cls->addProperty("MeanAnomaly", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getMeanAnomaly, &StateKeplerian::setMeanAnomaly>(Dimension::Angle()));
    cls->addProperty("EccAnomaly", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getEccAnomaly, &StateKeplerian::setEccAnomaly>(Dimension::Angle()));
    cls->addProperty("ArgLat", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getArgLat, &StateKeplerian::setArgLat>(Dimension::Angle()));
    cls->addProperty("TimePastPeri", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTimePastPeri, &StateKeplerian::setTimePastPeri>(Dimension::Time()));
    cls->addProperty("TimePastAscNode", aNewPropertyQuantity<StateKeplerian, &StateKeplerian::getTimePastAscNode, &StateKeplerian::setTimePastAscNode>(Dimension::Time()));
}

AST_NAMESPACE_END