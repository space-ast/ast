///
/// @file      UiFuelTank.cpp
/// @brief     燃料储罐编辑器实现
/// @author    axel
/// @date      2026-05-17
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

#include "UiFuelTank.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstUtil/Unit.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AST_NAMESPACE_BEGIN

using namespace units;

UiFuelTank::UiFuelTank(Object* object, QWidget* parent)
    : UiFuelTank(parent)
{
    setSpacecraftState(dynamic_cast<SpacecraftState*>(object));
}

UiFuelTank::UiFuelTank(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiFuelTank::~UiFuelTank() = default;

void UiFuelTank::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // Tank Pressure
    auto* pressureLayout = new QHBoxLayout();
    pressureLayout->addWidget(new QLabel(tr(u8"储罐压力"), this));
    tankPressureEdit_ = new UiQuantity(this);
    tankPressureEdit_->setQuantity(Quantity(5000, units::Pa));
    tankPressureEdit_->setFixedWidth(180);
    pressureLayout->addWidget(tankPressureEdit_);
    layout->addLayout(pressureLayout);

    // Tank Temperature
    auto* tempLayout = new QHBoxLayout();
    tempLayout->addWidget(new QLabel(tr(u8"储罐温度"), this));
    tankTemperatureEdit_ = new UiQuantity(this);
    tankTemperatureEdit_->setQuantity(Quantity(293.15, units::K));
    tankTemperatureEdit_->setFixedWidth(180);
    tempLayout->addWidget(tankTemperatureEdit_);
    layout->addLayout(tempLayout);

    // Fuel Density
    auto* densityLayout = new QHBoxLayout();
    densityLayout->addWidget(new QLabel(tr(u8"燃料密度"), this));
    fuelDensityEdit_ = new UiQuantity(this);
    fuelDensityEdit_->setQuantity(Quantity(1000, units::kg / (units::m * units::m * units::m)));
    fuelDensityEdit_->setFixedWidth(180);
    densityLayout->addWidget(fuelDensityEdit_);
    layout->addLayout(densityLayout);

    // Fuel Mass
    auto* fuelMassLayout = new QHBoxLayout();
    fuelMassLayout->addWidget(new QLabel(tr(u8"燃料质量"), this));
    fuelMassEdit_ = new UiQuantity(this);
    fuelMassEdit_->setQuantity(Quantity(500, units::kg));
    fuelMassEdit_->setFixedWidth(180);
    fuelMassLayout->addWidget(fuelMassEdit_);
    layout->addLayout(fuelMassLayout);

    layout->addStretch();

    // 连接信号槽 - 即时写入
    connect(tankPressureEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setTankPressure(tankPressureEdit_->getValueSI());
        }
    });
    connect(tankTemperatureEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setTankTemperature(tankTemperatureEdit_->getValueSI());
        }
    });
    connect(fuelDensityEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setFuelDensity(fuelDensityEdit_->getValueSI());
        }
    });
    connect(fuelMassEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setFuelMass(fuelMassEdit_->getValueSI());
        }
    });
}

void UiFuelTank::setSpacecraftState(SpacecraftState* state)
{
    if (state)
    {
        setObject(state);
        refreshFromState(state);
    }
}

SpacecraftState* UiFuelTank::getSpacecraftState() const
{
    return dynamic_cast<SpacecraftState*>(getObject());
}

void UiFuelTank::refreshFromState(SpacecraftState* state)
{
    if (!state)
        return;

    tankPressureEdit_->blockSignals(true);
    tankPressureEdit_->setQuantity(Quantity(state->getTankPressure(), units::Pa));
    tankPressureEdit_->blockSignals(false);

    tankTemperatureEdit_->blockSignals(true);
    tankTemperatureEdit_->setQuantity(Quantity(state->getTankTemperature(), units::K));
    tankTemperatureEdit_->blockSignals(false);

    fuelDensityEdit_->blockSignals(true);
    fuelDensityEdit_->setQuantity(Quantity(state->getFuelDensity(), units::kg / (units::m * units::m * units::m)));
    fuelDensityEdit_->blockSignals(false);

    fuelMassEdit_->blockSignals(true);
    fuelMassEdit_->setQuantity(Quantity(state->getFuelMass(), units::kg));
    fuelMassEdit_->blockSignals(false);
}

AST_NAMESPACE_END
