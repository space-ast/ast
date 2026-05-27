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
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "UiPropagate.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/HPOP.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstGUI/UiEventDetectorList.hpp"
#include "AstGUI/UiHPOPForceModel.hpp"
#include "AstGUI/UiODEIntegratorEditor.hpp"
#include "AstUtil/Unit.hpp"
#include "AstUtil/Quantity.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>

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
    rootLayout->setContentsMargins(0, 0, 0, 0);

    // ---- 轨道预报器 ----
    auto* propGroup = new QGroupBox(tr(u8"轨道预报器"), this);
    auto* propLayout = new QGridLayout(propGroup);

    forceModelBtn_ = new QPushButton(tr(u8"力模型配置..."), this);
    integratorBtn_ = new QPushButton(tr(u8"积分器配置..."), this);

    auto* btnRow = new QHBoxLayout();
    btnRow->addWidget(forceModelBtn_);
    btnRow->addWidget(integratorBtn_);
    btnRow->addStretch();
    propLayout->addLayout(btnRow, 0, 0, 1, 2);

    useMaxTimeCheck_ = new QCheckBox(this);
    useMaxTimeCheck_->setChecked(true);
    maxTimeEdit_ = new UiQuantity(this);
    maxTimeEdit_->setQuantity(Quantity(86400, s));

    auto* timeRow = new QHBoxLayout();
    timeRow->setSpacing(4);
    timeRow->addWidget(useMaxTimeCheck_);
    timeRow->addWidget(new QLabel(tr(u8"最大预报时间"), this));
    timeRow->addWidget(maxTimeEdit_);
    propLayout->addLayout(timeRow, 1, 0, 1, 2);

    rootLayout->addWidget(propGroup);

    // ---- 事件检测器 ----
    auto* eventGroup = new QGroupBox(tr(u8"停止条件 / 事件检测器"), this);
    auto* eventLayout = new QVBoxLayout(eventGroup);
    eventLayout->setContentsMargins(0, 0, 0, 0);

    eventDetectorList_ = new UiEventDetectorList(this);
    eventLayout->addWidget(eventDetectorList_);

    rootLayout->addWidget(eventGroup);

    // ---- 连接 ----
    connect(forceModelBtn_, &QPushButton::clicked,
            this, &UiPropagate::onConfigureForceModel);
    connect(integratorBtn_, &QPushButton::clicked,
            this, &UiPropagate::onConfigureIntegrator);
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
    eventDetectorList_->setPropagate(prop);
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

void UiPropagate::onConfigureForceModel()
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    auto* hpop = prop->propagator();
    if (!hpop)
        return;

    QDialog dlg(this);
    dlg.setWindowTitle(tr(u8"力模型配置"));
    auto* dlgLayout = new QVBoxLayout(&dlg);

    auto* editor = new UiHPOPForceModel(&dlg);
    editor->setHPOPForceModel(&hpop->forceModel());
    dlgLayout->addWidget(editor);

    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    dlgLayout->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted)
        editor->apply();
}

void UiPropagate::onConfigureIntegrator()
{
    auto* prop = getPropagate();
    if (!prop)
        return;

    auto* hpop = prop->propagator();
    if (!hpop)
        return;

    auto* integrator = hpop->getIntegrator();
    if (!integrator)
        return;

    QDialog dlg(this);
    dlg.setWindowTitle(tr(u8"积分器配置"));
    auto* dlgLayout = new QVBoxLayout(&dlg);

    auto* editor = new UiODEIntegratorEditor(&dlg);
    editor->setIntegrator(integrator);
    dlgLayout->addWidget(editor);

    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    dlgLayout->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted)
        editor->apply();
}

AST_NAMESPACE_END
