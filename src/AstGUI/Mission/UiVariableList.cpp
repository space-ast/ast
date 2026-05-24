///
/// @file      UiVariableList.cpp
/// @brief     脚本变量列表编辑控件实现
/// @details   ~
/// @author    axel
/// @date      2026-05-24
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiVariableList.hpp"
#include "AstScript/Variable.hpp"

#include <QInputDialog>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

UiVariableList::UiVariableList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiVariableList::setupUi()
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);

    listWidget_ = new QListWidget(this);
    listWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout_->addWidget(listWidget_);

    buttonLayout_ = new QHBoxLayout;
    buttonLayout_->setContentsMargins(0, 0, 0, 0);

    addButton_ = new QPushButton(tr("+"), this);
    addButton_->setToolTip(tr("Add a new variable"));
    removeButton_ = new QPushButton(tr("-"), this);
    removeButton_->setToolTip(tr("Remove selected variable"));
    removeButton_->setEnabled(false);

    buttonLayout_->addStretch();
    buttonLayout_->addWidget(addButton_);
    buttonLayout_->addWidget(removeButton_);
    mainLayout_->addLayout(buttonLayout_);

    connect(listWidget_, &QListWidget::currentItemChanged,
            this, &UiVariableList::onSelectionChanged);
    connect(listWidget_, &QListWidget::itemDoubleClicked,
            this, &UiVariableList::onItemDoubleClicked);
    connect(addButton_, &QPushButton::clicked,
            this, &UiVariableList::onAddVariable);
    connect(removeButton_, &QPushButton::clicked,
            this, &UiVariableList::onRemoveVariable);
}

void UiVariableList::setVariableList(VariableList* variableList)
{
    variableList_ = variableList;
    refreshUi();
}

void UiVariableList::refreshUi()
{
    listWidget_->clear();
    if (!variableList_) return;

    for (size_t i = 0; i < variableList_->size(); ++i)
    {
        Variable* var = variableList_->at(i);
        QString text = QString::fromStdString(var->name())
            + QStringLiteral(" = ")
            + QString::fromStdString(var->getExpression());
        QListWidgetItem* item = new QListWidgetItem(text, listWidget_);
        item->setData(Qt::UserRole, static_cast<qlonglong>(i));
    }
}

Variable* UiVariableList::selectedVariable() const
{
    auto* item = listWidget_->currentItem();
    if (!item || !variableList_) return nullptr;

    size_t index = static_cast<size_t>(item->data(Qt::UserRole).toLongLong());
    if (index < variableList_->size())
        return variableList_->at(index);
    return nullptr;
}

void UiVariableList::onSelectionChanged()
{
    Variable* var = selectedVariable();
    removeButton_->setEnabled(var != nullptr);
    emit variableSelected(var);
}

void UiVariableList::onAddVariable()
{
    bool ok = false;
    QString name = QInputDialog::getText(
        this, tr("New Variable"), tr("Variable name:"),
        QLineEdit::Normal, tr("var"), &ok);

    if (!ok || name.trimmed().isEmpty()) return;

    QString expr = QInputDialog::getText(
        this, tr("New Variable"), tr("Expression:"),
        QLineEdit::Normal, tr("0"), &ok);

    if (!ok) return;

    auto* var = Variable::New();
    var->setName(name.trimmed().toStdString());
    var->setExpr(expr.trimmed().toStdString());

    if (!variableList_)
    {
        // 如果没有设置外部列表，则无法添加
        delete var;
        return;
    }

    variableList_->append(var);
    refreshUi();

    // 选中新添加的变量
    int newIndex = static_cast<int>(variableList_->size()) - 1;
    listWidget_->setCurrentRow(newIndex);

    emit variableListChanged();
}

void UiVariableList::onRemoveVariable()
{
    auto* item = listWidget_->currentItem();
    if (!item || !variableList_) return;

    size_t index = static_cast<size_t>(item->data(Qt::UserRole).toLongLong());
    if (index >= variableList_->size()) return;

    Variable* var = variableList_->at(index);
    int ret = QMessageBox::question(
        this, tr("Remove Variable"),
        tr("Remove variable \"%1\"?").arg(QString::fromStdString(var->name())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    variableList_->erase(index);
    refreshUi();
    emit variableListChanged();
}

void UiVariableList::onItemDoubleClicked(QListWidgetItem* item)
{
    if (!item || !variableList_) return;

    size_t index = static_cast<size_t>(item->data(Qt::UserRole).toLongLong());
    if (index >= variableList_->size()) return;

    Variable* var = variableList_->at(index);

    bool ok = false;
    QString name = QInputDialog::getText(
        this, tr("Edit Variable"), tr("Variable name:"),
        QLineEdit::Normal, QString::fromStdString(var->name()), &ok);

    if (!ok || name.trimmed().isEmpty()) return;

    QString expr = QInputDialog::getText(
        this, tr("Edit Variable"), tr("Expression:"),
        QLineEdit::Normal, QString::fromStdString(var->getExpression()), &ok);

    if (!ok) return;

    var->setName(name.trimmed().toStdString());
    var->setExpr(expr.trimmed().toStdString());

    refreshUi();
    emit variableListChanged();
}

AST_NAMESPACE_END
