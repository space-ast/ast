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

    cls->addProperty("Frame", aNewPropertyObject<SpacecraftState, Frame, &SpacecraftState::getFrame, &SpacecraftState::setFrame>());
    cls->addProperty("Mass", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getMass>(Dimension::Mass()));
    cls->addProperty("FuelMass", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getFuelMass, &SpacecraftState::setFuelMass>(Dimension::Mass()));
    cls->addProperty("DryMass", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getDryMass, &SpacecraftState::setDryMass>(Dimension::Mass()));
    cls->addProperty("Cd", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getCd, &SpacecraftState::setCd>());
    cls->addProperty("Cr", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getCr, &SpacecraftState::setCr>());
    cls->addProperty("DragArea", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getDragArea, &SpacecraftState::setDragArea>(Dimension::Area()));
    cls->addProperty("SRPArea", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getSRPArea, &SpacecraftState::setSRPArea>(Dimension::Area()));
    cls->addProperty("K1", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getK1, &SpacecraftState::setK1>());
    cls->addProperty("K2", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getK2, &SpacecraftState::setK2>());
    cls->addProperty("FuelDensity", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getFuelDensity, &SpacecraftState::setFuelDensity>(Dimension::Density()));
    cls->addProperty("RadPressureArea", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getRadPressureArea, &SpacecraftState::setRadPressureArea>(Dimension::Area()));
    cls->addProperty("RadPressureCoeff", aNewPropertyDouble<SpacecraftState, &SpacecraftState::getRadPressureCoeff, &SpacecraftState::setRadPressureCoeff>());
    cls->addProperty("TankPressure", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getTankPressure, &SpacecraftState::setTankPressure>(Dimension::Pressure()));
    cls->addProperty("TankTemperature", aNewPropertyQuantity<SpacecraftState, &SpacecraftState::getTankTemperature, &SpacecraftState::setTankTemperature>(Dimension::Temperature()));
    cls->addProperty("OrbitState", aNewPropertyObject<SpacecraftState, State, &SpacecraftState::getOrbitState, &SpacecraftState::setOrbitState>());
}

AST_NAMESPACE_END