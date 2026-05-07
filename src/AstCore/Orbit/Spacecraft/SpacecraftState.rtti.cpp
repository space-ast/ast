#include "SpacecraftState.hpp"

// 自动生成的属性初始化代码
// 警告: 不要手动修改此文件

AST_NAMESPACE_BEGIN

Class SpacecraftState::staticType;

static bool SpacecraftState_ClassInited = (SpacecraftState::ClassInit(&SpacecraftState::staticType), true);

void SpacecraftState::ClassInit(Class* cls)
{

    cls->setName("SpacecraftState");
    cls->setDesc(u8R"(航天器状态，包含轨道状态、质量、面积、阻力系数、光压、密度、压力、温度等属性)");
    cls->addToRegistry();
    cls->setParent<ObjectNamed>();
    cls->setConstructor<SpacecraftState>();

    cls->addProperty("Mass", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getMass>());
    cls->addProperty("FuelMass", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getFuelMass, &SpacecraftState::setFuelMass>());
    cls->addProperty("DryMass", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getDryMass, &SpacecraftState::setDryMass>());
    cls->addProperty("Cd", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getCd, &SpacecraftState::setCd>());
    cls->addProperty("Cr", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getCr, &SpacecraftState::setCr>());
    cls->addProperty("DragArea", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getDragArea, &SpacecraftState::setDragArea>());
    cls->addProperty("SRPArea", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getSRPArea, &SpacecraftState::setSRPArea>());
    cls->addProperty("K1", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getK1, &SpacecraftState::setK1>());
    cls->addProperty("K2", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getK2, &SpacecraftState::setK2>());
    cls->addProperty("FuelDensity", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getFuelDensity, &SpacecraftState::setFuelDensity>());
    cls->addProperty("RadPressureArea", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getRadPressureArea, &SpacecraftState::setRadPressureArea>());
    cls->addProperty("RadPressureCoeff", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getRadPressureCoeff, &SpacecraftState::setRadPressureCoeff>());
    cls->addProperty("TankPressure", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getTankPressure, &SpacecraftState::setTankPressure>());
    cls->addProperty("TankTemperature", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getTankTemperature, &SpacecraftState::setTankTemperature>());
    cls->addProperty("OrbitState", aNewPropertyObject<SpacecraftState, State, &SpacecraftState::getOrbitState, &SpacecraftState::setOrbitState>());
}

AST_NAMESPACE_END