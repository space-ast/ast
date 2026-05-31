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
#include "AstCore/BurnImpulsive.hpp"
#include "AstGUI/UiBurnEditor.hpp"
#include <QVBoxLayout>

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

    burnEditor_ = new UiBurnEditor(this);
    layout->addWidget(burnEditor_);
}

void UiManeuver::setManeuver(Maneuver* maneuver)
{
    if (!maneuver)
        return;
    setObject(maneuver);

    Burn* burn = maneuver->burn();
    if (!burn)
    {
        burn = new BurnImpulsive();
        burn->setName("ImpulsiveBurn");
        maneuver->setBurn(burn);
    }
    burnEditor_->setBurn(burn);
}

Maneuver* UiManeuver::getManeuver() const
{
    return aobject_cast<Maneuver*>(getObject());
}

AST_NAMESPACE_END
