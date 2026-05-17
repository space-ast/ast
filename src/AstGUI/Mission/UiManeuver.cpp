///
/// @file      UiManeuver.cpp
/// @brief     Maneuver 段编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiManeuver.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Burn.hpp"
#include <QVBoxLayout>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiManeuver::UiManeuver(Object* object, QWidget* parent)
    : UiManeuver(parent)
{
    setManeuver(aobject_cast<Maneuver*>(object));
}

UiManeuver::UiManeuver(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiManeuver::~UiManeuver() = default;

void UiManeuver::setupUi()
{
    auto* layout = new QVBoxLayout(this);

    auto* typeLayout = new QHBoxLayout();
    typeLayout->addWidget(new QLabel(tr("发动机类型"), this));
    burnTypeCombo_ = new QComboBox(this);
    burnTypeCombo_->addItem(tr("脉冲 (Impulsive)"), 0);
    burnTypeCombo_->addItem(tr("有限推力 (Finite)"), 1);
    typeLayout->addWidget(burnTypeCombo_);
    layout->addLayout(typeLayout);

    burnInfoLabel_ = new QLabel(tr("选择发动机类型后可配置具体参数"), this);
    burnInfoLabel_->setWordWrap(true);
    layout->addWidget(burnInfoLabel_);

    layout->addStretch();

    connect(burnTypeCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &UiManeuver::refreshBurnInfo);
}

void UiManeuver::setManeuver(Maneuver* maneuver)
{
    if (!maneuver)
        return;
    setObject(maneuver);
    refreshBurnInfo();
}

Maneuver* UiManeuver::getManeuver() const
{
    return aobject_cast<Maneuver*>(getObject());
}

void UiManeuver::refreshBurnInfo()
{
    auto* m = getManeuver();
    if (!m)
        return;

    Burn* burn = m->burn();
    int idx = burnTypeCombo_->currentIndex();

    if (idx == 0)
        burnInfoLabel_->setText(tr("脉冲机动：瞬时速度增量，忽略推进时间。"));
    else
        burnInfoLabel_->setText(tr("有限推力机动：持续推力，考虑推进器参数。"));
}

AST_NAMESPACE_END
