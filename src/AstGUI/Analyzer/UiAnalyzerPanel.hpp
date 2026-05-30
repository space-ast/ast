///
/// @file      UiAnalyzerPanel.hpp
/// @brief     AstAnalyzer 主分析面板 (QDockWidget)
/// @details   组装变量列表、响应列表、属性编辑器、结果表格和进度条
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

#pragma once

#include "AstGlobal.h"
#include "AstAnalyzer/AnalyzerVariable.hpp"
#include "AstAnalyzer/AnalyzerConstraint.hpp"
#include <QDockWidget>
#include <QToolBar>
#include <vector>

class QSplitter;

AST_NAMESPACE_BEGIN

class UiAnalyzerVariableList;
class UiResponseList;
class UiPropertyEditor;
class UiResultView;
class Command;
class TraverseSearchAnalyzer;

/// @brief Analyzer 主面板，作为 QDockWidget 嵌入主窗口
class AST_GUI_API UiAnalyzerPanel : public QDockWidget
{
    Q_OBJECT
public:
    explicit UiAnalyzerPanel(QWidget* parent = nullptr);
    ~UiAnalyzerPanel() override;

    /// @brief 设置要分析的仿真命令
    void setSimulationCommand(Command* command);

    /// @brief 获取分析器实例
    TraverseSearchAnalyzer* analyzer() const;

signals:
    void analysisStarted();
    void analysisFinished();
    void runCompleted(int runIndex, const std::vector<double>& varValues, const std::vector<double>& respValues);
    void progressChanged(int current, int total);

private slots:
    void onRunClicked();
    void onStopClicked();
    void onAddVariable();
    void onAddResponse();
    void onVariableSelected(AnalyzerVariable* variable);
    void onResponseSelected(AnalyzerConstraint* response);
    void onRunResult(int runIndex, const std::vector<double>& varValues, const std::vector<double>& respValues);
    void onProgress(int current, int total);

private:
    void setupUi();
    void setupToolBar();
    void setupConnections();

    QToolBar*          toolBar_;
    QAction*           runAction_;
    QAction*           stopAction_;

    QSplitter*         leftSplitter_;
    QSplitter*         mainSplitter_;

    UiAnalyzerVariableList*    variableList_;
    UiResponseList*    responseList_;
    UiPropertyEditor*  propertyEditor_;
    UiResultView*      resultView_;

    TraverseSearchAnalyzer* analyzer_ = nullptr;
    bool running_ = false;
};

AST_NAMESPACE_END
