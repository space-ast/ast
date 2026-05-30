///
/// @file      UiAnalyzerPanel.cpp
/// @brief     AstAnalyzer 主分析面板实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiAnalyzerPanel.hpp"
#include "UiAnalyzerVariableList.hpp"
#include "UiResponseList.hpp"
#include "UiPropertyEditor.hpp"
#include "UiResultView.hpp"
#include "AstAnalyzer/TraverseSearchAnalyzer.hpp"
#include "AstAnalyzer/AnalyzerVariable.hpp"
#include "AstAnalyzer/AnalyzerConstraint.hpp"
#include "AstCore/Command.hpp"
#include "AstUtil/RTTIAPI.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QElapsedTimer>

AST_NAMESPACE_BEGIN

UiAnalyzerPanel::UiAnalyzerPanel(QWidget* parent)
    : QDockWidget(tr("分析器"), parent)
{
    setObjectName("UiAnalyzerPanel");
    setMinimumWidth(300);
    setupUi();
    setupConnections();
}

UiAnalyzerPanel::~UiAnalyzerPanel() = default;

void UiAnalyzerPanel::setupUi()
{
    auto* centralWidget = new QWidget(this);
    auto* rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(4, 4, 4, 4);

    // ---- 工具栏 ----
    setupToolBar();

    // ---- 主分割器 (水平: 左侧列表+右侧属性) ----
    mainSplitter_ = new QSplitter(Qt::Horizontal, this);

    // 左侧垂直分割器: 变量列表 + 响应列表
    leftSplitter_ = new QSplitter(Qt::Vertical, this);

    variableList_ = new UiAnalyzerVariableList(this);
    leftSplitter_->addWidget(variableList_);

    responseList_ = new UiResponseList(this);
    leftSplitter_->addWidget(responseList_);

    leftSplitter_->setStretchFactor(0, 1);
    leftSplitter_->setStretchFactor(1, 1);

    mainSplitter_->addWidget(leftSplitter_);

    // 右侧属性编辑器
    propertyEditor_ = new UiPropertyEditor(this);
    mainSplitter_->addWidget(propertyEditor_);

    mainSplitter_->setStretchFactor(0, 2);
    mainSplitter_->setStretchFactor(1, 3);

    rootLayout->addWidget(mainSplitter_);

    // 结果表格
    resultView_ = new UiResultView(this);
    rootLayout->addWidget(resultView_);

    setWidget(centralWidget);
}

void UiAnalyzerPanel::setupToolBar()
{
    toolBar_ = new QToolBar(tr("分析器工具栏"), this);

    runAction_ = toolBar_->addAction(tr("▶ 运行"));
    runAction_->setToolTip(tr("开始参数扫描分析"));

    stopAction_ = toolBar_->addAction(tr("■ 停止"));
    stopAction_->setToolTip(tr("停止运行分析"));
    stopAction_->setEnabled(false);

    setTitleBarWidget(toolBar_);
}

void UiAnalyzerPanel::setupConnections()
{
    connect(runAction_, &QAction::triggered,
            this, &UiAnalyzerPanel::onRunClicked);
    connect(stopAction_, &QAction::triggered,
            this, &UiAnalyzerPanel::onStopClicked);

    connect(variableList_, &UiAnalyzerVariableList::variableSelected,
            this, &UiAnalyzerPanel::onVariableSelected);
    connect(variableList_, &UiAnalyzerVariableList::addVariableRequested,
            this, &UiAnalyzerPanel::onAddVariable);

    connect(responseList_, &UiResponseList::responseSelected,
            this, &UiAnalyzerPanel::onResponseSelected);
    connect(responseList_, &UiResponseList::addResponseRequested,
            this, &UiAnalyzerPanel::onAddResponse);

    connect(this, &UiAnalyzerPanel::runCompleted,
            this, &UiAnalyzerPanel::onRunResult);
    connect(this, &UiAnalyzerPanel::progressChanged,
            this, &UiAnalyzerPanel::onProgress);
}

// ============================================================================
// 公共接口
// ============================================================================

void UiAnalyzerPanel::setSimulationCommand(Command* command)
{
    if (!analyzer_)
    {
        analyzer_ = aNewObject<TraverseSearchAnalyzer>();
    }
    // TODO: analyzer_->setSimulationCommand(command);
}

TraverseSearchAnalyzer* UiAnalyzerPanel::analyzer() const
{
    return analyzer_;
}

// ============================================================================
// 槽函数
// ============================================================================

void UiAnalyzerPanel::onRunClicked()
{
    if (!analyzer_)
    {
        QMessageBox::warning(this, tr("分析器"),
                             tr("未配置分析器，请先添加变量和响应。"));
        return;
    }

    // 应用当前属性编辑
    propertyEditor_->applyCurrent();

    if (running_)
        return;

    running_ = true;
    runAction_->setEnabled(false);
    stopAction_->setEnabled(true);
    emit analysisStarted();

    resultView_->clearResults();

    // 收集列名
    QStringList varNames, respNames;
    // TODO: get from analyzer
    resultView_->initColumns(varNames, respNames);

    QElapsedTimer timer;
    timer.start();

    // TODO: 分析计算必须放在后台线程（QThread/QtConcurrent）中执行，
    // 否则会阻塞 GUI 事件循环，导致界面冻结且"停止"按钮无法响应。
    // 循环中应定期检查 running_ 标志以支持取消。
    // int total = analyzer_->totalRuns();
    // for (int i = 0; i < total; ++i) {
    //     if (!running_) break;
    //     analyzer_->executeStep(i);
    //     emit progressChanged(i + 1, total);
    //     emit runCompleted(i + 1, varValues, respValues);
    // }
    // resultView_->setElapsedTime(timer.elapsed() / 1000.0);

    running_ = false;
    runAction_->setEnabled(true);
    stopAction_->setEnabled(false);
    emit analysisFinished();
}

void UiAnalyzerPanel::onStopClicked()
{
    running_ = false;
}

void UiAnalyzerPanel::onAddVariable()
{
    if (!analyzer_)
        analyzer_ = aNewObject<TraverseSearchAnalyzer>();

    auto* variable = aNewObject<AnalyzerVariable>();
    variable->setName("Variable");
    // TODO: analyzer_->addVariable(variable);

    // TODO: 刷新变量列表
    emit variableList_->variableSelected(variable);
    propertyEditor_->editVariable(variable);
}

void UiAnalyzerPanel::onAddResponse()
{
    if (!analyzer_)
        analyzer_ = aNewObject<TraverseSearchAnalyzer>();

    auto* response = aNewObject<AnalyzerConstraint>();
    response->setName("Response");
    // TODO: analyzer_->addConstraint(response);

    // TODO: 刷新响应列表
    emit responseList_->responseSelected(response);
    propertyEditor_->editResponse(response);
}

void UiAnalyzerPanel::onVariableSelected(AnalyzerVariable* variable)
{
    if (variable)
        propertyEditor_->editVariable(variable);
    else
        propertyEditor_->clear();
}

void UiAnalyzerPanel::onResponseSelected(AnalyzerConstraint* response)
{
    if (response)
        propertyEditor_->editResponse(response);
    else
        propertyEditor_->clear();
}

void UiAnalyzerPanel::onRunResult(int runIndex,
                                   const std::vector<double>& varValues,
                                   const std::vector<double>& respValues)
{
    resultView_->appendRow(runIndex, varValues, respValues);
}

void UiAnalyzerPanel::onProgress(int current, int total)
{
    resultView_->setProgress(current, total);
}

AST_NAMESPACE_END
