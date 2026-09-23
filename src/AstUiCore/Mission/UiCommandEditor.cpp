///
/// @file      UiCommandEditor.cpp
/// @brief     段属性编辑器实现
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

#include "UiCommandEditor.hpp"
#include "UiInitialState.hpp"
#include "UiPropagate.hpp"
#include "UiManeuver.hpp"
#include "UiSequence.hpp"
#include "UiFeasibleRegionStudy.hpp"
#include "AstCore/MissionCommand.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstAnalyzer/FeasibleRegionStudy.hpp"
#include "AstUtil/Class.hpp"
#include <QLabel>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

UiCommandEditor::UiCommandEditor(QWidget* parent)
    : QStackedWidget(parent)
{
    registerEditors();
}

UiCommandEditor::~UiCommandEditor() = default;

void UiCommandEditor::registerEditors()
{
    setupUi();

    // 各页面依次添加，记录索引
    initStatePage_ = new UiInitialState(this);
    initStateIdx_ = addWidget(initStatePage_);

    propagatePage_ = new UiPropagate(this);
    propagateIdx_ = addWidget(propagatePage_);

    maneuverPage_ = new UiManeuver(this);
    maneuverIdx_ = addWidget(maneuverPage_);

    sequencePage_ = new UiSequence(this);
    sequenceIdx_ = addWidget(sequencePage_);

    targetSeqPage_ = new UiTargeterSequence(this);
    targetSeqIdx_ = addWidget(targetSeqPage_);

    feasibleRegionPage_ = new UiFeasibleRegionStudy(this);
    feasibleRegionIdx_ = addWidget(feasibleRegionPage_);
}

void UiCommandEditor::setupUi()
{
    // 初始占位页面
    auto* placeholder = new QWidget(this);
    auto* placeholderLayout = new QVBoxLayout(placeholder);
    auto* hint = new QLabel(tr("在任务树中选择一个段以编辑其属性"), placeholder);
    hint->setAlignment(Qt::AlignCenter);
    hint->setWordWrap(true);
    placeholderLayout->addWidget(hint);
    addWidget(placeholder);
    setCurrentIndex(0);
}

void UiCommandEditor::editCommand(MissionCommand* cmd)
{
    if (!cmd)
    {
        clear();
        return;
    }

    int targetIdx = -1;

    // 按 RTTI 类型匹配（注意先匹配子类再匹配父类）
    if (cmd->isOfType(TargeterSequence::StaticType()))
    {
        targetSeqPage_->setTargeterSequence(aobject_cast<TargeterSequence*>(cmd));
        targetIdx = targetSeqIdx_;
    }
    else if (cmd->isOfType(InitialState::StaticType()))
    {
        initStatePage_->setInitialState(aobject_cast<InitialState*>(cmd));
        targetIdx = initStateIdx_;
    }
    else if (cmd->isOfType(Propagate::StaticType()))
    {
        propagatePage_->setPropagate(aobject_cast<Propagate*>(cmd));
        targetIdx = propagateIdx_;
    }
    else if (cmd->isOfType(Maneuver::StaticType()))
    {
        maneuverPage_->setManeuver(aobject_cast<Maneuver*>(cmd));
        targetIdx = maneuverIdx_;
    }
    else if (cmd->isOfType(Sequence::StaticType()))
    {
        sequencePage_->setSequence(aobject_cast<Sequence*>(cmd));
        targetIdx = sequenceIdx_;
    }
    else if (cmd->isOfType(FeasibleRegionStudy::StaticType()))
    {
        feasibleRegionPage_->setStudy(aobject_cast<FeasibleRegionStudy*>(cmd));
        targetIdx = feasibleRegionIdx_;
    }

    if (targetIdx >= 0)
        setCurrentIndex(targetIdx);
}

void UiCommandEditor::clear()
{
    setCurrentIndex(0);
}

AST_NAMESPACE_END
