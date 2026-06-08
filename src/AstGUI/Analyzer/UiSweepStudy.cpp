///
/// @file      UiSweepStudy.cpp
/// @brief     SweepStudy 编辑界面实现
/// @details   提供三个 Tab 页：变量编辑、约束编辑和执行命令编辑
/// @author    axel
/// @date      2026-06-05
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

#include "UiSweepStudy.hpp"
#include "AstAnalyzer/SweepStudy.hpp"
#include "AstGUI/ObjectEditRegistry.hpp"
#include "AstGUI/UiStudyVariableList.hpp"
#include "AstGUI/UiStudyConstraintList.hpp"
#include "AstGUI/UiPropertyEditor.hpp"
#include "AstUtil/RTTIAPI.hpp"

#include <QGroupBox>
#include <QLabel>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

UiSweepStudy::UiSweepStudy(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiSweepStudy::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);

    // ============================================================
    // Tab 0: 变量
    // ============================================================
    auto* varTab = new QWidget(tabWidget_);
    auto* varTabLayout = new QVBoxLayout(varTab);
    varTabLayout->setContentsMargins(0, 0, 0, 0);

    varSplitter_ = new QSplitter(Qt::Horizontal, varTab);
    varSplitter_->setChildrenCollapsible(true);

    // 左侧：变量列表
    auto* varListGroup = new QGroupBox(tr("扫描变量"), varSplitter_);
    auto* varListLayout = new QVBoxLayout(varListGroup);
    varListLayout->setContentsMargins(4, 4, 4, 4);
    varList_ = new UiStudyVariableList(varListGroup);
    varListLayout->addWidget(varList_);

    // 右侧：变量属性编辑器
    varPropertyEditor_ = new UiPropertyEditor(varSplitter_);

    varSplitter_->addWidget(varListGroup);
    varSplitter_->addWidget(varPropertyEditor_);
    varSplitter_->setSizes({300, 400});

    varTabLayout->addWidget(varSplitter_);
    tabWidget_->addTab(varTab, tr("变量"));

    // 连线：选中变量 → 属性编辑器
    connect(varList_, &UiStudyVariableList::variableSelected,
            varPropertyEditor_, &UiPropertyEditor::editVariable);
    connect(varList_, &UiStudyVariableList::addVariableRequested,
            this, [this]()
    {
        auto* a = analyzer();
        if (!a)
            return;
        auto* variable = aNewObject<SweepVariable>();
        variable->setName(u8"变量");
        a->addVariable(variable);
        refreshUi();
        varPropertyEditor_->editVariable(variable);
    });

    // ============================================================
    // Tab 1: 约束
    // ============================================================
    auto* constraintTab = new QWidget(tabWidget_);
    auto* constraintTabLayout = new QVBoxLayout(constraintTab);
    constraintTabLayout->setContentsMargins(0, 0, 0, 0);

    constraintSplitter_ = new QSplitter(Qt::Horizontal, constraintTab);
    constraintSplitter_->setChildrenCollapsible(true);

    // 左侧：约束列表
    auto* constraintListGroup = new QGroupBox(tr("约束条件"), constraintSplitter_);
    auto* constraintListLayout = new QVBoxLayout(constraintListGroup);
    constraintListLayout->setContentsMargins(4, 4, 4, 4);
    responseList_ = new UiStudyConstraintList(constraintListGroup);
    constraintListLayout->addWidget(responseList_);

    // 右侧：约束属性编辑器
    constraintPropertyEditor_ = new UiPropertyEditor(constraintSplitter_);

    constraintSplitter_->addWidget(constraintListGroup);
    constraintSplitter_->addWidget(constraintPropertyEditor_);
    constraintSplitter_->setSizes({300, 400});

    constraintTabLayout->addWidget(constraintSplitter_);
    tabWidget_->addTab(constraintTab, tr("约束"));

    // 连线：选中约束 → 属性编辑器
    connect(responseList_, &UiStudyConstraintList::responseSelected,
            constraintPropertyEditor_, &UiPropertyEditor::editResponse);
    connect(responseList_, &UiStudyConstraintList::addResponseRequested,
            this, [this]()
    {
        auto* a = analyzer();
        if (!a)
            return;
        auto* constraint = aNewObject<SweepOutput>();
        constraint->setName(u8"约束");
        a->addOutput(constraint);
        refreshUi();
        constraintPropertyEditor_->editResponse(constraint);
    });

    // ============================================================
    // Tab 2: 任务模型
    // ============================================================
    auto* cmdTab = new QWidget(tabWidget_);
    auto* cmdLayout = new QVBoxLayout(cmdTab);
    cmdLayout->setContentsMargins(0, 0, 0, 0);

    commandStack_ = new QStackedWidget(cmdTab);
    placeholderLabel_ = new QLabel(tr("未关联任务模型"), commandStack_);
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    commandStack_->addWidget(placeholderLabel_); // index 0
    cmdLayout->addWidget(commandStack_);
    tabWidget_->addTab(cmdTab, tr("任务模型"));

    mainLayout->addWidget(tabWidget_);
}

void UiSweepStudy::setAnalyzer(SweepStudy* a)
{
    if (analyzer() == a)
        return;

    analyzer_ = a;

    if (a)
    {
        varList_->setVariables(a->variables());
        responseList_->setResponses(a->outputs());
        rebuildCommandEditor();
    }
    else
    {
        varList_->setVariables({});
        responseList_->setResponses({});
        commandStack_->setCurrentIndex(0);
    }
}

void UiSweepStudy::refreshUi()
{
    auto* a = analyzer();
    if (a)
    {
        // 保存当前选中
        auto* selVar = varList_->selectedVariable();
        auto* selResp = responseList_->selectedResponse();

        varList_->setVariables(a->variables());
        responseList_->setResponses(a->outputs());

        // 恢复选中
        if (selVar)
            varPropertyEditor_->editVariable(selVar);
        if (selResp)
            constraintPropertyEditor_->editResponse(selResp);
    }
}

void UiSweepStudy::rebuildCommandEditor()
{
    // 移除旧编辑器（index 1，如果存在）
    while (commandStack_->count() > 1)
    {
        QWidget* w = commandStack_->widget(1);
        commandStack_->removeWidget(w);
        delete w;
    }

    auto* a = analyzer();
    if (!a)
    {
        commandStack_->setCurrentIndex(0);
        return;
    }

    auto* cmd = a->relatedCommand();
    if (!cmd)
    {
        commandStack_->setCurrentIndex(0);
        return;
    }

    auto* editor = ObjectEditRegistry::Instance().newEditWidget(cmd);
    if (!editor)
    {
        commandStack_->setCurrentIndex(0);
        return;
    }

    editor->setParent(commandStack_);
    commandStack_->addWidget(editor); // index 1
    commandStack_->setCurrentIndex(1);
}

AST_NAMESPACE_END
