///
/// @file      UiSpacecraftParams.cpp
/// @brief     航天器参数编辑器实现
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

#include "UiSpacecraftParams.hpp"
#include "AstCore/SpacecraftState.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstUtil/Unit.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

AST_NAMESPACE_BEGIN

using namespace units;

UiSpacecraftParams::UiSpacecraftParams(Object* object, QWidget* parent)
    : UiSpacecraftParams(parent)
{
    setSpacecraftState(dynamic_cast<SpacecraftState*>(object));
}

UiSpacecraftParams::UiSpacecraftParams(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiSpacecraftParams::~UiSpacecraftParams() = default;

void UiSpacecraftParams::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    // ---------- 干质量 ----------
    auto* massLayout = new QHBoxLayout();
    massLayout->addWidget(new QLabel(tr("干重"), this));
    dryMassEdit_ = new UiQuantity(this);
    dryMassEdit_->setQuantity(Quantity(500, units::kg));
    dryMassEdit_->setFixedWidth(180);
    massLayout->addWidget(dryMassEdit_);
    layout->addLayout(massLayout);

    // ---------- 阻力 ----------
    auto* dragGroup = new QGroupBox(tr("阻力"), this);
    auto* dragLayout = new QVBoxLayout(dragGroup);

    // Cd
    auto* cdLayout = new QHBoxLayout();
    cdLayout->addWidget(new QLabel(tr("阻力系数 (Cd)"), this));
    cdEdit_ = new UiQuantity(this);
    cdEdit_->setQuantity(Quantity(2.2));
    cdEdit_->setFixedWidth(180);
    cdLayout->addWidget(cdEdit_);
    dragLayout->addLayout(cdLayout);

    // Drag Area
    auto* dragAreaLayout = new QHBoxLayout();
    dragAreaLayout->addWidget(new QLabel(tr("阻力面积"), this));
    dragAreaEdit_ = new UiQuantity(this);
    dragAreaEdit_->setQuantity(Quantity(20, units::m * units::m));
    dragAreaEdit_->setFixedWidth(180);
    dragAreaLayout->addWidget(dragAreaEdit_);
    dragLayout->addLayout(dragAreaLayout);

    layout->addWidget(dragGroup);

    // ---------- 太阳光压 ----------
    auto* srpGroup = new QGroupBox(tr("太阳光压"), this);
    auto* srpLayout = new QVBoxLayout(srpGroup);

    // Cr
    auto* crLayout = new QHBoxLayout();
    crLayout->addWidget(new QLabel(tr("太阳光压系数 (Cr)"), this));
    crEdit_ = new UiQuantity(this);
    crEdit_->setQuantity(Quantity(1.0));
    crEdit_->setFixedWidth(180);
    crLayout->addWidget(crEdit_);
    srpLayout->addLayout(crLayout);

    // SRP Area
    auto* srpAreaLayout = new QHBoxLayout();
    srpAreaLayout->addWidget(new QLabel(tr("太阳光压面积"), this));
    srpAreaEdit_ = new UiQuantity(this);
    srpAreaEdit_->setQuantity(Quantity(20, units::m * units::m));
    srpAreaEdit_->setFixedWidth(180);
    srpAreaLayout->addWidget(srpAreaEdit_);
    srpLayout->addLayout(srpAreaLayout);

    layout->addWidget(srpGroup);

    // ---------- 反照/热辐射压 ----------
    auto* radGroup = new QGroupBox(tr("反照/热辐射压"), this);
    auto* radLayout = new QVBoxLayout(radGroup);

    // Ck
    auto* ckLayout = new QHBoxLayout();
    ckLayout->addWidget(new QLabel(tr("反照/热辐射压系数 (Ck)"), this));
    ckEdit_ = new UiQuantity(this);
    ckEdit_->setQuantity(Quantity(1.0));
    ckEdit_->setFixedWidth(180);
    ckLayout->addWidget(ckEdit_);
    radLayout->addLayout(ckLayout);

    // Rad Area
    auto* radAreaLayout = new QHBoxLayout();
    radAreaLayout->addWidget(new QLabel(tr("反照/热辐射压面积"), this));
    radPressureAreaEdit_ = new UiQuantity(this);
    radPressureAreaEdit_->setQuantity(Quantity(20, units::m * units::m));
    radPressureAreaEdit_->setFixedWidth(180);
    radAreaLayout->addWidget(radPressureAreaEdit_);
    radLayout->addLayout(radAreaLayout);

    layout->addWidget(radGroup);

    // ---------- GPS 太阳光压（预留）----------
    auto* gpsGroup = new QGroupBox(tr("GPS 太阳光压"), this);
    gpsGroup->hide();
    auto* gpsLayout = new QVBoxLayout(gpsGroup);

    // K1
    auto* k1Layout = new QHBoxLayout();
    k1Layout->addWidget(new QLabel(("K1"), this));
    k1Edit_ = new UiQuantity(this);
    k1Edit_->setQuantity(Quantity(1.0));
    k1Edit_->setFixedWidth(180);
    k1Layout->addWidget(k1Edit_);
    gpsLayout->addLayout(k1Layout);

    // K2
    auto* k2Layout = new QHBoxLayout();
    k2Layout->addWidget(new QLabel(("K2"), this));
    k2Edit_ = new UiQuantity(this);
    k2Edit_->setQuantity(Quantity(1.0));
    k2Edit_->setFixedWidth(180);
    k2Layout->addWidget(k2Edit_);
    gpsLayout->addLayout(k2Layout);

    layout->addWidget(gpsGroup);
    layout->addStretch();

    // 连接信号槽 - 即时写入
    connect(dryMassEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setDryMass(dryMassEdit_->getValueSI());
        }
    });
    connect(cdEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setCd(cdEdit_->getValueSI());
        }
    });
    connect(dragAreaEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setDragArea(dragAreaEdit_->getValueSI());
        }
    });
    connect(crEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setCr(crEdit_->getValueSI());
        }
    });
    connect(srpAreaEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setSRPArea(srpAreaEdit_->getValueSI());
        }
    });
    connect(ckEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setRadPressureCoeff(ckEdit_->getValueSI());
        }
    });
    connect(radPressureAreaEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setRadPressureArea(radPressureAreaEdit_->getValueSI());
        }
    });
    connect(k1Edit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setK1(k1Edit_->getValueSI());
        }
    });
    connect(k2Edit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* state = getSpacecraftState()) {
            state->setK2(k2Edit_->getValueSI());
        }
    });
}

void UiSpacecraftParams::setSpacecraftState(SpacecraftState* state)
{
    if (state)
    {
        setObject(state);
        refreshFromState(state);
    }
}

SpacecraftState* UiSpacecraftParams::getSpacecraftState() const
{
    return dynamic_cast<SpacecraftState*>(getObject());
}

void UiSpacecraftParams::refreshFromState(SpacecraftState* state)
{
    if (!state)
        return;

    dryMassEdit_->blockSignals(true);
    dryMassEdit_->setQuantity(Quantity(state->getDryMass(), units::kg));
    dryMassEdit_->blockSignals(false);

    cdEdit_->blockSignals(true);
    cdEdit_->setQuantity(Quantity(state->getCd()));
    cdEdit_->blockSignals(false);

    dragAreaEdit_->blockSignals(true);
    dragAreaEdit_->setQuantity(Quantity(state->getDragArea(), units::m * units::m));
    dragAreaEdit_->blockSignals(false);

    crEdit_->blockSignals(true);
    crEdit_->setQuantity(Quantity(state->getCr()));
    crEdit_->blockSignals(false);

    srpAreaEdit_->blockSignals(true);
    srpAreaEdit_->setQuantity(Quantity(state->getSRPArea(), units::m * units::m));
    srpAreaEdit_->blockSignals(false);

    ckEdit_->blockSignals(true);
    ckEdit_->setQuantity(Quantity(state->getRadPressureCoeff()));
    ckEdit_->blockSignals(false);

    radPressureAreaEdit_->blockSignals(true);
    radPressureAreaEdit_->setQuantity(Quantity(state->getRadPressureArea(), units::m * units::m));
    radPressureAreaEdit_->blockSignals(false);

    k1Edit_->blockSignals(true);
    k1Edit_->setQuantity(Quantity(state->getK1()));
    k1Edit_->blockSignals(false);

    k2Edit_->blockSignals(true);
    k2Edit_->setQuantity(Quantity(state->getK2()));
    k2Edit_->blockSignals(false);
}

AST_NAMESPACE_END
