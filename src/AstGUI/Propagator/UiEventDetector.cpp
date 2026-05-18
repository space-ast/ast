///
/// @file      UiEventDetector.cpp
/// @brief     事件检测器基类编辑器实现
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

#include "UiEventDetector.hpp"
#include "AstCore/EventDetector.hpp"
#include "AstGUI/UiQuantity.hpp"
#include "AstGUI/UiInteger.hpp"
#include "AstUtil/Unit.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>

AST_NAMESPACE_BEGIN

using namespace units;

UiEventDetector::UiEventDetector(Object* object, QWidget* parent)
    : UiEventDetector(parent)
{
    setEventDetector(dynamic_cast<EventDetector*>(object));
}

UiEventDetector::UiEventDetector(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiEventDetector::~UiEventDetector() = default;

void UiEventDetector::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // 激活
    activeCheck_ = new QCheckBox(tr("激活"), this);
    activeCheck_->setChecked(true);
    layout->addWidget(activeCheck_);

    // 重复次数
    auto* repeatLayout = new QHBoxLayout();
    repeatLayout->addWidget(new QLabel(tr("重复次数"), this));
    repeatCountEdit_ = new UiInteger(this);
    repeatCountEdit_->setValue(1);
    repeatCountEdit_->setFixedWidth(180);
    repeatLayout->addWidget(repeatCountEdit_);
    layout->addLayout(repeatLayout);

    // 方向
    auto* dirLayout = new QHBoxLayout();
    dirLayout->addWidget(new QLabel(tr("方向"), this));
    directionCombo_ = new QComboBox(this);
    directionCombo_->addItem(tr("双向"), static_cast<int>(EventDetector::EDirection::eBoth));
    directionCombo_->addItem(tr("递增"), static_cast<int>(EventDetector::EDirection::eIncrease));
    directionCombo_->addItem(tr("递减"), static_cast<int>(EventDetector::EDirection::eDecrease));
    directionCombo_->setFixedWidth(180);
    dirLayout->addWidget(directionCombo_);
    layout->addLayout(dirLayout);

    // 阈值
    auto* thresholdLayout = new QHBoxLayout();
    thresholdLayout->addWidget(new QLabel(tr("阈值"), this));
    thresholdEdit_ = new UiQuantity(this);
    thresholdEdit_->setQuantity(Quantity(1e-10));
    thresholdEdit_->setFixedWidth(180);
    thresholdLayout->addWidget(thresholdEdit_);
    layout->addLayout(thresholdLayout);

    // 目标值
    auto* goalLayout = new QHBoxLayout();
    goalLayout->addWidget(new QLabel(tr("目标值"), this));
    goalEdit_ = new UiQuantity(this);
    goalEdit_->setQuantity(Quantity(0.0));
    goalEdit_->setFixedWidth(180);
    goalLayout->addWidget(goalEdit_);
    layout->addLayout(goalLayout);

    layout->addStretch();

    // 连接 — 即时写入
    connect(activeCheck_, &QCheckBox::toggled, this, [this](bool checked) {
        if (auto* det = getEventDetector()) {
            det->setActive(checked);
            emit detectorChanged(det);
        }
    });
    connect(repeatCountEdit_, &UiInteger::valueChanged, this, [this](int val) {
        if (auto* det = getEventDetector()) {
            det->setRepeatCount(val);
            emit detectorChanged(det);
        }
    });
    connect(directionCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        if (auto* det = getEventDetector()) {
            det->setDirection(static_cast<EventDetector::EDirection>(directionCombo_->currentData().toInt()));
            emit detectorChanged(det);
        }
    });
    connect(thresholdEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* det = getEventDetector()) {
            det->setThreshold(thresholdEdit_->getValueSI());
            emit detectorChanged(det);
        }
    });
    connect(goalEdit_, &UiQuantity::quantityChanged, this, [this]() {
        if (auto* det = getEventDetector()) {
            det->setGoal(goalEdit_->getValueSI());
            emit detectorChanged(det);
        }
    });
}

void UiEventDetector::setEventDetector(EventDetector* det)
{
    if (!det)
        return;
    setObject(det);
    refreshFromDetector(det);
}

EventDetector* UiEventDetector::getEventDetector() const
{
    return dynamic_cast<EventDetector*>(getObject());
}

void UiEventDetector::refreshFromDetector(EventDetector* det)
{
    if (!det)
        return;

    activeCheck_->blockSignals(true);
    activeCheck_->setChecked(det->active());
    activeCheck_->blockSignals(false);

    repeatCountEdit_->blockSignals(true);
    repeatCountEdit_->setValue(det->repeatCount());
    repeatCountEdit_->blockSignals(false);

    directionCombo_->blockSignals(true);
    int dirIdx = directionCombo_->findData(static_cast<int>(det->direction()));
    if (dirIdx >= 0)
        directionCombo_->setCurrentIndex(dirIdx);
    directionCombo_->blockSignals(false);

    thresholdEdit_->blockSignals(true);
    thresholdEdit_->setQuantity(Quantity(det->threshold()));
    thresholdEdit_->blockSignals(false);

    goalEdit_->blockSignals(true);
    goalEdit_->setQuantity(Quantity(det->goal()));
    goalEdit_->blockSignals(false);
}

AST_NAMESPACE_END
