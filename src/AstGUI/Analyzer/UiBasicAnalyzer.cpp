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
#include "AstGUI/UiExpressionBrowser.hpp"
#include "AstGUI/UiVariableList.hpp"

#include <QGroupBox>
#include <QLabel>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QVBoxLayout>

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

    // ============================================================
    // Tab 0: 变量
    // ============================================================
    auto* varTab = new QWidget(tabWidget_);
    auto* varLayout = new QVBoxLayout(varTab);
    varLayout->setContentsMargins(0, 0, 0, 0);

    // 水平分割器：左侧表达式浏览器，右侧输入/输出变量
    varSplitter_ = new QSplitter(Qt::Horizontal, varTab);
    varSplitter_->setChildrenCollapsible(true);

    expressionBrowser_ = new UiExpressionBrowser(varSplitter_);

    splitter_ = new QSplitter(Qt::Vertical, varSplitter_);
    splitter_->setChildrenCollapsible(true);

    // 输入变量区域
    auto* inputsGroup = new QGroupBox(tr("输入变量"), splitter_);
    auto* inputsLayout = new QVBoxLayout(inputsGroup);
    inputsLayout->setContentsMargins(4, 4, 4, 4);
    inputsEditor_ = new UiVariableList(inputsGroup);
    inputsLayout->addWidget(inputsEditor_);

    // 输出变量区域
    auto* outputsGroup = new QGroupBox(tr("输出变量"), splitter_);
    auto* outputsLayout = new QVBoxLayout(outputsGroup);
    outputsLayout->setContentsMargins(4, 4, 4, 4);
    outputsEditor_ = new UiVariableList(outputsGroup);
    outputsLayout->addWidget(outputsEditor_);

    varSplitter_->addWidget(expressionBrowser_);
    varSplitter_->addWidget(splitter_);
    varSplitter_->setSizes({400, 400});

    varLayout->addWidget(varSplitter_);
    tabWidget_->addTab(varTab, tr("变量"));

    // 连线：对象属性 → 输入变量，对象计算量 → 输出变量
    connect(expressionBrowser_, &UiExpressionBrowser::propertyExpressionSelected,
            inputsEditor_, &UiVariableList::addExpression);
    connect(expressionBrowser_, &UiExpressionBrowser::calculationExpressionSelected,
            outputsEditor_, &UiVariableList::addExpression);

    // ============================================================
    // Tab 1: 任务模型
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
