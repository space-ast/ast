///
/// @file      UiWorkbenchExprPicker.hpp
/// @brief     从 StudyWorkbench 的输入/输出变量中选择表达式
/// @details   弹出对话框，复用 UiVariableList 表格展示父级 StudyWorkbench
///            的输入/输出变量供选择，并提供"高级"按钮进入 UiExpressionBrowser。
/// @author    axel
/// @date      2026-06-11
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
#include <QDialog>

class QPushButton;

AST_NAMESPACE_BEGIN

class StudyWorkbench;
class Expr;
class UiVariableList;
class Variable;

/// @brief 从 StudyWorkbench 的输入/输出变量中选择表达式
class AST_GUI_API UiWorkbenchExprPicker : public QDialog
{
    Q_OBJECT
public:
    /// @param workbench 父级研究工作台
    /// @param parent   父窗口
    explicit UiWorkbenchExprPicker(StudyWorkbench* workbench, QWidget* parent = nullptr);

    /// @brief 获取用户选中的表达式（Variable 即 Expr）
    Expr* selectedExpr() const { return selectedExpr_; }

private slots:
    void onAdvanced();
    void onVariableSelected(Variable* variable);
    void accept() override;

private:
    void setupUi();

    StudyWorkbench* workbench_;
    Expr*           selectedExpr_ = nullptr;

    UiVariableList* inputList_;
    UiVariableList* outputList_;
    QPushButton*    advancedBtn_;
};

AST_NAMESPACE_END
