///
/// @file      UiPropertyEditor.cpp
/// @brief     属性编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiPropertyEditor.hpp"

AST_NAMESPACE_BEGIN

// ============================================================================
// UiVariableEditor
// ============================================================================

UiVariableEditor::UiVariableEditor(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiVariableEditor::setupUi()
{
    layout_ = new QFormLayout(this);

    nameEdit_ = new QLineEdit(this);
    layout_->addRow(tr("名称"), nameEdit_);

    startValueSpin_ = new QDoubleSpinBox(this);
    startValueSpin_->setRange(-1e12, 1e12);
    startValueSpin_->setDecimals(6);
    layout_->addRow(tr("初始值"), startValueSpin_);

    endValueSpin_ = new QDoubleSpinBox(this);
    endValueSpin_->setRange(-1e12, 1e12);
    endValueSpin_->setDecimals(6);
    layout_->addRow(tr("结束值"), endValueSpin_);

    stepSizeSpin_ = new QDoubleSpinBox(this);
    stepSizeSpin_->setRange(-1e12, 1e12);
    stepSizeSpin_->setDecimals(6);
    layout_->addRow(tr("步长"), stepSizeSpin_);

    exprEdit_ = new QLineEdit(this);
    exprEdit_->setPlaceholderText("e.g. state.sma");
    layout_->addRow(tr("表达式"), exprEdit_);
}

void UiVariableEditor::loadVariable(SweepVariable* variable)
{
    if (!variable) return;
    nameEdit_->setText(QString::fromStdString(variable->getName()));
    startValueSpin_->setValue(variable->startValue());
    endValueSpin_->setValue(variable->endValue());
    stepSizeSpin_->setValue(variable->stepSize());
    if (auto* expr = variable->expr())
        exprEdit_->setText(QString::fromStdString(expr->getName()));
    else
        exprEdit_->clear();
}

void UiVariableEditor::applyToVariable(SweepVariable* variable)
{
    if (!variable) return;
    variable->setName(nameEdit_->text().toStdString());
    variable->setStartValue(startValueSpin_->value());
    variable->setEndValue(endValueSpin_->value());
    variable->setStepSize(stepSizeSpin_->value());
    // expr is set via the scripting system, not here
}

// ============================================================================
// UiStudyConstraintEditor
// ============================================================================

UiStudyConstraintEditor::UiStudyConstraintEditor(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiStudyConstraintEditor::setupUi()
{
    layout_ = new QFormLayout(this);

    nameEdit_ = new QLineEdit(this);
    layout_->addRow(tr("名称"), nameEdit_);

    exprEdit_ = new QLineEdit(this);
    exprEdit_->setPlaceholderText("e.g. state.altitude");
    layout_->addRow(tr("表达式"), exprEdit_);
}

void UiStudyConstraintEditor::loadResponse(SweepOutput* response)
{
    if (!response) return;
    nameEdit_->setText(QString::fromStdString(response->getName()));
    exprEdit_->clear();
    // TODO: response->expr() not yet implemented on StudyConstraint
}

void UiStudyConstraintEditor::applyToResponse(SweepOutput* response)
{
    if (!response) return;
    response->setName(nameEdit_->text().toStdString());
    // expr is set via the scripting system, not here
}

// ============================================================================
// UiPropertyEditor
// ============================================================================

UiPropertyEditor::UiPropertyEditor(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    stack_ = new QStackedWidget(this);

    variableEditor_ = new UiVariableEditor(this);
    responseEditor_ = new UiStudyConstraintEditor(this);

    stack_->addWidget(variableEditor_);   // index 0
    stack_->addWidget(responseEditor_);   // index 1

    // 默认显示一个空白页
    stack_->addWidget(new QWidget(this)); // index 2 (empty)
    stack_->setCurrentIndex(2);

    layout->addWidget(stack_);
}

void UiPropertyEditor::editVariable(SweepVariable* variable)
{
    applyCurrent();
    currentVariable_ = variable;
    currentResponse_ = nullptr;
    variableEditor_->loadVariable(variable);
    stack_->setCurrentIndex(0);
}

void UiPropertyEditor::editResponse(SweepOutput* response)
{
    applyCurrent();
    currentVariable_ = nullptr;
    currentResponse_ = response;
    responseEditor_->loadResponse(response);
    stack_->setCurrentIndex(1);
}

void UiPropertyEditor::clear()
{
    applyCurrent();
    currentVariable_ = nullptr;
    currentResponse_ = nullptr;
    stack_->setCurrentIndex(2);
}

void UiPropertyEditor::applyCurrent()
{
    if (currentVariable_)
        variableEditor_->applyToVariable(currentVariable_);
    if (currentResponse_)
        responseEditor_->applyToResponse(currentResponse_);
}

AST_NAMESPACE_END
