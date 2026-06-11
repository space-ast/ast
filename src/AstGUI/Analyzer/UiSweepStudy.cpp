///
/// @file      UiSweepStudy.cpp
/// @brief     SweepStudy 编辑界面实现
/// @details   提供两个 Tab 页：变量与输出（左右分栏内嵌表格编辑）和任务模型编辑
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
#include "AstGUI/UiSweepVariableList.hpp"
#include "AstGUI/UiSweepOutputList.hpp"

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
    // Tab 0: 变量与输出（左右分栏）
    // ============================================================
    auto* studyTab = new QWidget(tabWidget_);
    auto* studyTabLayout = new QVBoxLayout(studyTab);
    studyTabLayout->setContentsMargins(0, 0, 0, 0);

    studySplitter_ = new QSplitter(Qt::Horizontal, studyTab);
    studySplitter_->setChildrenCollapsible(true);

    // 左侧：变量表格
    auto* varGroup = new QGroupBox(tr("扫描变量"), studySplitter_);
    auto* varGroupLayout = new QVBoxLayout(varGroup);
    varGroupLayout->setContentsMargins(0, 0, 0, 0);
    varList_ = new UiSweepVariableList(varGroup);
    varGroupLayout->addWidget(varList_);

    // 右侧：输出表格
    auto* outGroup = new QGroupBox(tr("输出"), studySplitter_);
    auto* outGroupLayout = new QVBoxLayout(outGroup);
    outGroupLayout->setContentsMargins(0, 0, 0, 0);
    outputList_ = new UiSweepOutputList(outGroup);
    outGroupLayout->addWidget(outputList_);

    studySplitter_->addWidget(varGroup);
    studySplitter_->addWidget(outGroup);
    studySplitter_->setSizes({400, 300});

    studyTabLayout->addWidget(studySplitter_);
    tabWidget_->addTab(studyTab, tr("变量与输出"));

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

void UiSweepStudy::setAnalyzer(SweepStudy* a)
{
    if (analyzer() == a)
        return;

    analyzer_ = a;

    if (a)
    {
        varList_->setStudy(a);
        outputList_->setStudy(a);
        rebuildCommandEditor();
    }
    else
    {
        varList_->setStudy(nullptr);
        outputList_->setStudy(nullptr);
        commandStack_->setCurrentIndex(0);
    }
}

void UiSweepStudy::refreshUi()
{
    auto* a = analyzer();
    if (a)
    {
        varList_->refreshTable();
        outputList_->refreshTable();
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
