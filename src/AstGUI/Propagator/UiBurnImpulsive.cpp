///
/// @file      UiBurnImpulsive.cpp
/// @brief     脉冲推力参数编辑界面实现
/// @author    axel
/// @date      2026-05-18
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

#include "UiBurnImpulsive.hpp"
#include "AstCore/Burn.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/Resolve.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/Unit.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

AST_NAMESPACE_BEGIN

static const char* kThrustAxesOptions[] = {
    "Satellite VNC(Earth)",
    "Earth Inertial",
    "ICRF",
    "J2000"
};

UiBurnImpulsive::UiBurnImpulsive(Object* object, QWidget* parent)
    : UiBurnImpulsive(parent)
{
    setBurn(aobject_cast<Burn*>(object));
}

UiBurnImpulsive::UiBurnImpulsive(QWidget* parent)
    : UiObject(parent)
{
    setObjectName("UiBurnImpulsive");
    setupUi();
}

void UiBurnImpulsive::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 推力轴
    auto* axesRow = new QHBoxLayout();
    axesRow->addWidget(new QLabel(tr("推力轴"), this));
    thrustAxesCombo_ = new QComboBox(this);
    for (const auto* opt : kThrustAxesOptions)
        thrustAxesCombo_->addItem(QString::fromLatin1(opt));
    axesRow->addWidget(thrustAxesCombo_);
    axesRow->addStretch();
    layout->addLayout(axesRow);

    auto dim = Dimension::Speed();

    // X
    auto* xRow = new QHBoxLayout();
    xRow->addWidget(new QLabel("X", this));
    xEdit_ = new UiQuantity(this);
    xEdit_->setDimension(dim);
    xRow->addWidget(xEdit_);
    layout->addLayout(xRow);

    // Y
    auto* yRow = new QHBoxLayout();
    yRow->addWidget(new QLabel("Y", this));
    yEdit_ = new UiQuantity(this);
    yEdit_->setDimension(dim);
    yRow->addWidget(yEdit_);
    layout->addLayout(yRow);

    // Z
    auto* zRow = new QHBoxLayout();
    zRow->addWidget(new QLabel("Z", this));
    zEdit_ = new UiQuantity(this);
    zEdit_->setDimension(dim);
    zRow->addWidget(zEdit_);
    layout->addLayout(zRow);

    layout->addStretch();

    // 即时写入
    connect(thrustAxesCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UiBurnImpulsive::apply);
    connect(xEdit_, &UiQuantity::quantityChanged,
            this, &UiBurnImpulsive::apply);
    connect(yEdit_, &UiQuantity::quantityChanged,
            this, &UiBurnImpulsive::apply);
    connect(zEdit_, &UiQuantity::quantityChanged,
            this, &UiBurnImpulsive::apply);

    refreshUi();
}

BurnImpulsive* UiBurnImpulsive::getBurnImpulsive() const
{
    return dynamic_cast<BurnImpulsive*>(getObject());
}

void UiBurnImpulsive::setBurn(Burn* burn)
{
    if (auto* imp = dynamic_cast<BurnImpulsive*>(burn))
    {
        setObject(imp);
        refreshUi();
    }
}

void UiBurnImpulsive::refreshUi()
{
    auto* imp = getBurnImpulsive();
    if (!imp) return;

    // 推力轴
    {
        auto* axes = imp->axes();
        QString name = axes ? QString::fromStdString(axes->name()) : QString();
        int idx = thrustAxesCombo_->findText(name);
        if (idx >= 0)
        {
            thrustAxesCombo_->blockSignals(true);
            thrustAxesCombo_->setCurrentIndex(idx);
            thrustAxesCombo_->blockSignals(false);
        }
    }

    xEdit_->blockSignals(true);
    xEdit_->setValueSI(imp->x());
    xEdit_->blockSignals(false);

    yEdit_->blockSignals(true);
    yEdit_->setValueSI(imp->y());
    yEdit_->blockSignals(false);

    zEdit_->blockSignals(true);
    zEdit_->setValueSI(imp->z());
    zEdit_->blockSignals(false);
}

void UiBurnImpulsive::apply()
{
    auto* imp = getBurnImpulsive();
    if (imp)
    {
        applyTo(imp);
        emit burnChanged(imp);
    }
}

void UiBurnImpulsive::applyTo(BurnImpulsive* burn)
{
    if (!burn) return;

    burn->setX(xEdit_->getValueSI());
    burn->setY(yEdit_->getValueSI());
    burn->setZ(zEdit_->getValueSI());

    QString axesName = thrustAxesCombo_->currentText();
    if (!axesName.isEmpty())
    {
        Axes* axes = aResolveAxes(axesName.toStdString());
        if (axes)
            burn->setAxes(axes);
    }
}

AST_NAMESPACE_END
