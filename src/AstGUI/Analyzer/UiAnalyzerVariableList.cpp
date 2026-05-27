///
/// @file      UiAnalyzerVariableList.cpp
/// @brief     变量列表面板实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiAnalyzerVariableList.hpp"
#include "AstAnalyzer/AnalyzerVariable.hpp"

AST_NAMESPACE_BEGIN

UiAnalyzerVariableList::UiAnalyzerVariableList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiAnalyzerVariableList::setupUi()
{
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);

    listWidget_ = new QListWidget(this);
    listWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout_->addWidget(listWidget_);

    addButton_ = new QPushButton(tr("+ 添加变量"), this);
    layout_->addWidget(addButton_);

    connect(listWidget_, &QListWidget::currentItemChanged,
            this, &UiAnalyzerVariableList::onSelectionChanged);
    connect(addButton_, &QPushButton::clicked,
            this, &UiAnalyzerVariableList::addVariableRequested);
}

void UiAnalyzerVariableList::setVariables(const std::vector<SharedPtr<AnalyzerVariable>>& variables)
{
    variables_ = variables;
    listWidget_->clear();
    for (auto& var : variables)
    {
        QString text = QString("%1  [%2 : %3 : %4]")
            .arg(QString::fromStdString(var->getName()))
            .arg(var->startValue())
            .arg(var->stepSize())
            .arg(var->endValue());
        QListWidgetItem* item = new QListWidgetItem(text, listWidget_);
        item->setData(Qt::UserRole, QString::fromStdString(var->getName()));
    }
}

AnalyzerVariable* UiAnalyzerVariableList::selectedVariable() const
{
    auto item = listWidget_->currentItem();
    if (!item) return nullptr;
    QString name = item->data(Qt::UserRole).toString();
    for (auto& var : variables_)
    {
        if (QString::fromStdString(var->getName()) == name)
            return var.get();
    }
    return nullptr;
}

void UiAnalyzerVariableList::onSelectionChanged()
{
    emit variableSelected(selectedVariable());
}

AST_NAMESPACE_END
