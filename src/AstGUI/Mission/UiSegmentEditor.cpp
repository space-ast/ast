///
/// @file      UiSegmentEditor.cpp
/// @brief     段属性编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiSegmentEditor.hpp"
#include "UiInitialState.hpp"
#include "UiPropagate.hpp"
#include "UiManeuver.hpp"
#include "UiSequence.hpp"
#include "AstCore/MissionCommand.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstUtil/Class.hpp"
#include <QLabel>

AST_NAMESPACE_BEGIN

UiSegmentEditor::UiSegmentEditor(QWidget* parent)
    : QStackedWidget(parent)
{
    registerEditors();
}

UiSegmentEditor::~UiSegmentEditor() = default;

void UiSegmentEditor::registerEditors()
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
}

void UiSegmentEditor::setupUi()
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

void UiSegmentEditor::editCommand(MissionCommand* cmd)
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

    if (targetIdx >= 0)
        setCurrentIndex(targetIdx);
}

void UiSegmentEditor::clear()
{
    setCurrentIndex(0);
}

AST_NAMESPACE_END
