///
/// @file      UiBasicAnalyzer.cpp
/// @brief     BasicAnalyzer 编辑界面实现
/// @details   提供两个 Tab 页：变量编辑（inputs/outputs）和执行命令编辑
/// @author    axel
/// @date      2026-06-03
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

#include "UiBasicAnalyzer.hpp"
#include "AstAnalyzer/BasicAnalyzer.hpp"
#include "AstGUI/ObjectEditRegistry.hpp"
#include "AstGUI/UiVariableList.hpp"

#include <QVBoxLayout>
#include <QTabWidget>
#include <QSplitter>
#include <QStackedWidget>
#include <QLabel>

AST_NAMESPACE_BEGIN

UiBasicAnalyzer::UiBasicAnalyzer(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiBasicAnalyzer::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);

    // --- Tab 0: 变量 ---
    auto* varTab = new QWidget(this);
    auto* varLayout = new QVBoxLayout(varTab);
    varLayout->setContentsMargins(0, 0, 0, 0);

    splitter_ = new QSplitter(Qt::Vertical, varTab);
    splitter_->setChildrenCollapsible(true);

    // 输入变量区域
    auto* inputsWidget = new QWidget(splitter_);
    auto* inputsLayout = new QVBoxLayout(inputsWidget);
    inputsLayout->setContentsMargins(0, 0, 0, 0);
    auto* inputsLabel = new QLabel(tr("输入变量"), inputsWidget);
    inputsEditor_ = new UiVariableList(inputsWidget);
    inputsLayout->addWidget(inputsLabel);
    inputsLayout->addWidget(inputsEditor_);

    // 输出变量区域
    auto* outputsWidget = new QWidget(splitter_);
    auto* outputsLayout = new QVBoxLayout(outputsWidget);
    outputsLayout->setContentsMargins(0, 0, 0, 0);
    auto* outputsLabel = new QLabel(tr("输出变量"), outputsWidget);
    outputsEditor_ = new UiVariableList(outputsWidget);
    outputsLayout->addWidget(outputsLabel);
    outputsLayout->addWidget(outputsEditor_);

    varLayout->addWidget(splitter_);
    tabWidget_->addTab(varTab, tr("变量"));

    // --- Tab 1: 执行命令 ---
    auto* cmdTab = new QWidget(this);
    auto* cmdLayout = new QVBoxLayout(cmdTab);
    cmdLayout->setContentsMargins(0, 0, 0, 0);

    commandStack_ = new QStackedWidget(cmdTab);
    placeholderLabel_ = new QLabel(tr("未关联执行命令"), commandStack_);
    placeholderLabel_->setAlignment(Qt::AlignCenter);
    commandStack_->addWidget(placeholderLabel_); // index 0
    cmdLayout->addWidget(commandStack_);
    tabWidget_->addTab(cmdTab, tr("执行命令"));

    mainLayout->addWidget(tabWidget_);
}

void UiBasicAnalyzer::setBasicAnalyzer(BasicAnalyzer* analyzer)
{
    if (analyzer_ == analyzer)
        return;

    analyzer_ = analyzer;

    if (analyzer_)
    {
        inputsEditor_->setVariableList(&analyzer_->inputs(), analyzer_);
        outputsEditor_->setVariableList(&analyzer_->outputs(), analyzer_);
        rebuildCommandEditor();
    }
    else
    {
        inputsEditor_->setVariableList(nullptr, nullptr);
        outputsEditor_->setVariableList(nullptr, nullptr);
        commandStack_->setCurrentIndex(0);
    }
}

void UiBasicAnalyzer::refreshUi()
{
    if (analyzer_)
    {
        inputsEditor_->refreshUi();
        outputsEditor_->refreshUi();
    }
}

void UiBasicAnalyzer::rebuildCommandEditor()
{
    // 移除旧编辑器（index 1，如果存在）
    while (commandStack_->count() > 1)
    {
        QWidget* w = commandStack_->widget(1);
        commandStack_->removeWidget(w);
        delete w;
    }

    if (!analyzer_)
    {
        commandStack_->setCurrentIndex(0);
        return;
    }

    auto* cmd = analyzer_->relatedCommand();
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
