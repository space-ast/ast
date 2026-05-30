///
/// @file      UiPropertyEditor.hpp
/// @brief     属性编辑器，根据选中对象切换编辑表单
/// @details   ~
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
#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QFormLayout>

class QLabel;

AST_NAMESPACE_BEGIN

/// @brief 变量属性编辑器
class AST_GUI_API UiVariableEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiVariableEditor(QWidget* parent = nullptr);

    /// @brief 加载变量属性到编辑器
    void loadVariable(AnalyzerVariable* variable);

    /// @brief 将编辑器的值写回变量
    void applyToVariable(AnalyzerVariable* variable);

private:
    void setupUi();

    QFormLayout*    layout_;
    QLineEdit*      nameEdit_;
    QDoubleSpinBox* startValueSpin_;
    QDoubleSpinBox* endValueSpin_;
    QDoubleSpinBox* stepSizeSpin_;
    QLineEdit*      exprEdit_;
};

/// @brief 响应属性编辑器
class AST_GUI_API UiResponseEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiResponseEditor(QWidget* parent = nullptr);

    /// @brief 加载响应属性到编辑器
    void loadResponse(AnalyzerConstraint* response);

    /// @brief 将编辑器的值写回响应
    void applyToResponse(AnalyzerConstraint* response);

private:
    void setupUi();

    QFormLayout* layout_;
    QLineEdit*   nameEdit_;
    QLineEdit*   exprEdit_;
};

/// @brief 属性编辑面板，根据选中的对象类型切换编辑页
class AST_GUI_API UiPropertyEditor : public QWidget
{
    Q_OBJECT
public:
    explicit UiPropertyEditor(QWidget* parent = nullptr);

    /// @brief 切换到变量编辑模式
    void editVariable(AnalyzerVariable* variable);

    /// @brief 切换到响应编辑模式
    void editResponse(AnalyzerConstraint* response);

    /// @brief 清除当前编辑
    void clear();

    /// @brief 应用当前编辑到后台对象
    void applyCurrent();

private:
    QStackedWidget*  stack_;
    UiVariableEditor* variableEditor_;
    UiResponseEditor* responseEditor_;

    AnalyzerVariable*   currentVariable_ = nullptr;
    AnalyzerConstraint* currentResponse_ = nullptr;
};

AST_NAMESPACE_END
