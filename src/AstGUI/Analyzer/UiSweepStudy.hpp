///
/// @file      UiSweepStudy.hpp
/// @brief     SweepStudy 编辑界面
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

#pragma once

#include "AstGlobal.h"
#include "AstAnalyzer/SweepStudy.hpp"
#include <QWidget>

class QTabWidget;
class QSplitter;
class QStackedWidget;
class QLabel;

AST_NAMESPACE_BEGIN

class SweepStudy;
class UiStudyVariableList;
class UiStudyConstraintList;
class UiPropertyEditor;

/// @brief SweepStudy 编辑界面
class AST_GUI_API UiSweepStudy : public QWidget
{
    Q_OBJECT
public:
    explicit UiSweepStudy(QWidget* parent = nullptr);

    /// @brief 绑定要编辑的 SweepStudy
    void setAnalyzer(SweepStudy* analyzer);

    /// @brief 获取当前绑定的 SweepStudy
    SweepStudy* analyzer() const { return analyzer_.get(); }

    /// @brief 刷新界面
    void refreshUi();

private:
    void setupUi();
    void rebuildCommandEditor();

    WeakPtr<SweepStudy> analyzer_;

    QTabWidget*            tabWidget_;

    // 变量 Tab
    QSplitter*             varSplitter_;
    UiStudyVariableList* varList_;
    UiPropertyEditor*      varPropertyEditor_;

    // 约束 Tab
    QSplitter*             constraintSplitter_;
    UiStudyConstraintList*        responseList_;
    UiPropertyEditor*      constraintPropertyEditor_;

    // 任务模型 Tab
    QStackedWidget*        commandStack_;
    QLabel*                placeholderLabel_;
};

AST_NAMESPACE_END
