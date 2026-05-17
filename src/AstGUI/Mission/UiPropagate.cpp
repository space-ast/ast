///
/// @file      UiPropagate.cpp
/// @brief     Propagate 段编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiPropagate.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/HPOP.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QListWidget>

AST_NAMESPACE_BEGIN

using namespace units;

UiPropagate::UiPropagate(Object* object, QWidget* parent)
    : UiPropagate(parent)
{
    setPropagate(aobject_cast<Propagate*>(object));
}

UiPropagate::UiPropagate(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiPropagate::~UiPropagate() = default;

void UiPropagate::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);

    // ---- 预报器选择 ----
    auto* propGroup = new QGroupBox(tr("轨道预报器"), this);
    auto* propLayout = new QGridLayout(propGroup);

    propLayout->addWidget(new QLabel(tr("类型"), this), 0, 0);
    propagatorTypeCombo_ = new QComboBox(this);
    propagatorTypeCombo_->addItem(tr("HPOP (高精度轨道预报)"), 0);
    propLayout->addWidget(propagatorTypeCombo_, 0, 1);

    rootLayout->addWidget(propGroup);

    // ---- 时间设置 ----
    auto* timeGroup = new QGroupBox(tr("预报时间"), this);
    auto* timeLayout = new QGridLayout(timeGroup);

    timeLayout->addWidget(new QLabel(tr("最小预报时间"), this), 0, 0);
    minTimeEdit_ = new UiQuantity(this);
    minTimeEdit_->setQuantity(Quantity(0, s));
    timeLayout->addWidget(minTimeEdit_, 0, 1);

    timeLayout->addWidget(new QLabel(tr("最大预报时间"), this), 1, 0);
    maxTimeEdit_ = new UiQuantity(this);
    maxTimeEdit_->setQuantity(Quantity(86400, s));
    timeLayout->addWidget(maxTimeEdit_, 1, 1);

    useMaxTimeCheck_ = new QCheckBox(tr("启用最大预报时间"), this);
    useMaxTimeCheck_->setChecked(true);
    timeLayout->addWidget(useMaxTimeCheck_, 2, 0, 1, 2);

    rootLayout->addWidget(timeGroup);

    // ---- 事件检测器 ----
    auto* eventGroup = new QGroupBox(tr("停止条件 / 事件检测器"), this);
    auto* eventLayout = new QVBoxLayout(eventGroup);

    eventDetectorList_ = new QListWidget(this);
    eventDetectorList_->setMaximumHeight(120);
    eventLayout->addWidget(eventDetectorList_);

    rootLayout->addWidget(eventGroup);

    rootLayout->addStretch();

    // ---- 连接 ----
    connect(minTimeEdit_, &UiQuantity::quantityChanged,
            this, &UiPropagate::onMinTimeChanged);
    connect(maxTimeEdit_, &UiQuantity::quantityChanged,
            this, &UiPropagate::onMaxTimeChanged);
    connect(useMaxTimeCheck_, &QCheckBox::toggled,
            this, &UiPropagate::onMaxTimeEnabledChanged);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiPropagate::setPropagate(Propagate* prop)
{
    if (!prop)
        return;
    setObject(prop);
    refreshFromPropagate();
}

Propagate* UiPropagate::getPropagate() const
{
    return aobject_cast<Propagate*>(getObject());
}

void UiPropagate::refreshFromPropagate()
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    minTimeEdit_->blockSignals(true);
    minTimeEdit_->setQuantity(Quantity(prop->minPropTime(), s));
    minTimeEdit_->blockSignals(false);

    maxTimeEdit_->blockSignals(true);
    maxTimeEdit_->setQuantity(Quantity(prop->maxPropTime(), s));
    maxTimeEdit_->blockSignals(false);

    useMaxTimeCheck_->blockSignals(true);
    useMaxTimeCheck_->setChecked(prop->useMaxPropTime());
    useMaxTimeCheck_->blockSignals(false);

    maxTimeEdit_->setEnabled(prop->useMaxPropTime());
}

// ============================================================================
// 槽 — 即时写入
// ============================================================================

void UiPropagate::onMinTimeChanged()
{
    if (auto* prop = getPropagate())
        prop->setMinPropTime(minTimeEdit_->getValueSI());
}

void UiPropagate::onMaxTimeChanged()
{
    if (auto* prop = getPropagate())
        prop->setMaxPropTime(maxTimeEdit_->getValueSI());
}

void UiPropagate::onMaxTimeEnabledChanged(bool checked)
{
    if (auto* prop = getPropagate())
        prop->setUseMaxPropTime(checked);
    maxTimeEdit_->setEnabled(checked);
}

AST_NAMESPACE_END
