///
/// @file      UiStudyWorkbench.hpp
/// @brief     StudyWorkbench 编辑界面
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

#pragma once

#include "AstGlobal.h"
#include "AstAnalyzer/StudyWorkbench.hpp"
#include <QWidget>

class QTabWidget;
class QSplitter;
class QStackedWidget;
class QLabel;

AST_NAMESPACE_BEGIN

class StudyWorkbench;
class UiExpressionBrowser;
class UiVariableList;

/// @brief StudyWorkbench 编辑界面
class AST_GUI_API UiStudyWorkbench : public QWidget
{
    Q_OBJECT
public:
    explicit UiStudyWorkbench(QWidget* parent = nullptr);

    UiStudyWorkbench(const UiStudyWorkbench&) = delete;
    UiStudyWorkbench& operator=(const UiStudyWorkbench&) = delete;

    /// @brief 绑定要编辑的 StudyWorkbench
    void setStudyWorkbench(StudyWorkbench* analyzer);

    /// @brief 获取当前绑定的 StudyWorkbench
    StudyWorkbench* studyWorkbench() const { return analyzer_.get(); }

    /// @brief 刷新界面
    void refreshUi();

private:
    void setupUi();
    void rebuildCommandEditor();

    WeakPtr<StudyWorkbench> analyzer_{};

    QSplitter*           varSplitter_{};
    UiExpressionBrowser* expressionBrowser_{};
    QTabWidget*          tabWidget_{};
    QSplitter*           splitter_{};
    UiVariableList*      inputsEditor_{};
    UiVariableList*      outputsEditor_{};
    QStackedWidget*      commandStack_{};
    QLabel*              placeholderLabel_{};
};

AST_NAMESPACE_END
