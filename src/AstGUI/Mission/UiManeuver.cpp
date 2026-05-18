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
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

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
    layout->setContentsMargins(0, 0, 0, 0);

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
