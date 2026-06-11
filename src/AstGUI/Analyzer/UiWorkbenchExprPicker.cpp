///
/// @file      UiWorkbenchExprPicker.cpp
/// @brief     从 StudyWorkbench 的输入/输出变量中选择表达式
/// @details   复用 UiVariableList 展示变量表格，隐藏编辑按钮，双击选中
/// @author    axel
/// @date      2026-06-11
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiWorkbenchExprPicker.hpp"
#include "UiExpressionBrowser.hpp"
#include "AstGUI/UiVariableList.hpp"
#include "AstAnalyzer/StudyWorkbench.hpp"
#include "AstScript/Variable.hpp"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

UiWorkbenchExprPicker::UiWorkbenchExprPicker(StudyWorkbench* workbench, QWidget* parent)
    : QDialog(parent)
    , workbench_(workbench)
{
    setupUi();

    // 绑定数据
    if (workbench_)
    {
        inputList_->setVariableList(&workbench_->inputs(), workbench_);
        inputList_->setInterpreter(workbench_->interpreter(), workbench_);
        outputList_->setVariableList(&workbench_->outputs(), workbench_);
        outputList_->setInterpreter(workbench_->interpreter(), workbench_);
    }
}

void UiWorkbenchExprPicker::setupUi()
{
    setWindowTitle(tr("选择变量"));
    resize(650, 500);

    auto* mainLayout = new QVBoxLayout(this);

    auto* hintLabel = new QLabel(tr("从工作台的输入/输出变量中选择，或点击\"高级\"进入表达式浏览器"), this);
    hintLabel->setWordWrap(true);
    mainLayout->addWidget(hintLabel);

    // 输入变量组
    auto* inputGroup = new QGroupBox(tr("输入变量"), this);
    auto* inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputList_ = new UiVariableList(inputGroup);
    inputList_->setToolbarVisible(false);
    inputLayout->addWidget(inputList_);
    mainLayout->addWidget(inputGroup);

    // 输出变量组
    auto* outputGroup = new QGroupBox(tr("输出变量"), this);
    auto* outputLayout = new QVBoxLayout(outputGroup);
    outputLayout->setContentsMargins(0, 0, 0, 0);
    outputList_ = new UiVariableList(outputGroup);
    outputList_->setToolbarVisible(false);
    outputLayout->addWidget(outputList_);
    mainLayout->addWidget(outputGroup);

    // 底部按钮
    auto* bottomLayout = new QHBoxLayout;

    advancedBtn_ = new QPushButton(tr("高级..."), this);
    advancedBtn_->setToolTip(tr("打开表达式浏览器，选择任意对象属性或计算量"));
    bottomLayout->addWidget(advancedBtn_);

    bottomLayout->addStretch();

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    bottomLayout->addWidget(buttonBox);

    mainLayout->addLayout(bottomLayout);

    // 连接
    connect(inputList_, &UiVariableList::variableSelected,
            this, &UiWorkbenchExprPicker::onVariableSelected);
    connect(outputList_, &UiVariableList::variableSelected,
            this, &UiWorkbenchExprPicker::onVariableSelected);
    connect(advancedBtn_, &QPushButton::clicked,
            this, &UiWorkbenchExprPicker::onAdvanced);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void UiWorkbenchExprPicker::onAdvanced()
{
    Expr* expr = UiExpressionBrowser::GetExpression(this);
    if (expr)
    {
        selectedExpr_ = expr;
        QDialog::accept();
    }
}

void UiWorkbenchExprPicker::onVariableSelected(Variable* variable)
{
    if (variable)
    {
        selectedExpr_ = variable;
        QDialog::accept();
    }
}

void UiWorkbenchExprPicker::accept()
{
    // 如果还没选中，尝试从当前焦点控件获取
    if (!selectedExpr_)
    {
        auto* var = inputList_->selectedVariable();
        if (var)
            selectedExpr_ = var;
    }
    if (!selectedExpr_)
    {
        auto* var = outputList_->selectedVariable();
        if (var)
            selectedExpr_ = var;
    }

    QDialog::accept();
}

AST_NAMESPACE_END
