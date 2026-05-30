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
#include "UiExpressionBrowser.hpp"
#include "AstScript/Variable.hpp"

#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

AST_NAMESPACE_BEGIN

enum { COL_NAME = 0, COL_EXPR = 1, COL_DESC = 2 };

namespace {

/// @brief 在 form 中添加带浏览按钮的表达式输入行，返回 QLineEdit*
QLineEdit* addExpressionRow(QFormLayout* form, QDialog* parent, const QString& initialText)
{
    auto* row = new QHBoxLayout;
    auto* edit = new QLineEdit(initialText, parent);
    auto* browseBtn = new QPushButton("...", parent);
    browseBtn->setToolTip(QObject::tr("浏览对象属性和计算量"));
    browseBtn->setFixedWidth(30);
    row->addWidget(edit);
    row->addWidget(browseBtn);
    form->addRow(QObject::tr("表达式"), row);

    QObject::connect(browseBtn, &QPushButton::clicked, [edit, parent]() {
        QString expr = UiExpressionBrowser::getExpression(parent);
        if (!expr.isEmpty())
            edit->setText(expr);
    });

    return edit;
}

} // anonymous namespace

UiVariableList::UiVariableList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiVariableList::setupUi()
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);

    tableWidget_ = new QTableWidget(0, 3, this);
    tableWidget_->setHorizontalHeaderLabels({tr("名称"), tr("表达式"), tr("描述")});
    tableWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget_->horizontalHeader()->setStretchLastSection(true);
    tableWidget_->verticalHeader()->setVisible(false);
    tableWidget_->setColumnWidth(COL_NAME, 120);
    tableWidget_->setColumnWidth(COL_EXPR, 150);
    mainLayout_->addWidget(tableWidget_);

    buttonLayout_ = new QHBoxLayout;
    buttonLayout_->setContentsMargins(0, 0, 0, 0);

    addButton_ = new QPushButton(("+"), this);
    addButton_->setToolTip(tr("添加新变量"));
    removeButton_ = new QPushButton(("-"), this);
    removeButton_->setToolTip(tr("删除选中变量"));
    removeButton_->setEnabled(false);

    buttonLayout_->addStretch();
    buttonLayout_->addWidget(addButton_);
    buttonLayout_->addWidget(removeButton_);
    mainLayout_->addLayout(buttonLayout_);

    connect(tableWidget_, &QTableWidget::itemSelectionChanged,
            this, &UiVariableList::onSelectionChanged);
    connect(tableWidget_, &QTableWidget::cellDoubleClicked,
            this, &UiVariableList::onCellDoubleClicked);
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
    tableWidget_->setRowCount(0);
    if (!variableList_) return;

    size_t n = variableList_->size();
    tableWidget_->setRowCount(static_cast<int>(n));
    for (size_t i = 0; i < n; ++i)
    {
        Variable* var = variableList_->at(i);
        if (!var) continue;
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(var->name()));
        nameItem->setData(Qt::UserRole, static_cast<qlonglong>(i));
        tableWidget_->setItem(static_cast<int>(i), COL_NAME, nameItem);
        tableWidget_->setItem(static_cast<int>(i), COL_EXPR,
            new QTableWidgetItem(QString::fromStdString(var->getInnerExpression())));

        tableWidget_->setItem(static_cast<int>(i), COL_DESC,
            new QTableWidgetItem(QString::fromStdString(var->desc())));
    }
}

Variable* UiVariableList::selectedVariable() const
{
    int row = tableWidget_->currentRow();
    if (row < 0 || !variableList_) return nullptr;

    size_t index = static_cast<size_t>(row);
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
    QDialog dlg(this);
    dlg.setWindowTitle(tr("新建变量"));

    auto* form = new QFormLayout(&dlg);
    auto* nameEdit = new QLineEdit(("var"), &dlg);
    form->addRow(tr("名称"), nameEdit);

    auto* exprEdit = addExpressionRow(form, &dlg, ("0"));

    auto* descEdit = new QLineEdit(&dlg);
    form->addRow(tr("描述"), descEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    form->addRow(buttons);

    if (dlg.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) return;

    auto* var = Variable::New();
    var->setName(name.toStdString());
    var->setExpr(exprEdit->text().trimmed().toStdString());
    var->setDesc(descEdit->text().trimmed().toStdString());

    if (!variableList_)
    {
        delete var;
        return;
    }

    variableList_->append(var);
    refreshUi();
    tableWidget_->selectRow(static_cast<int>(variableList_->size()) - 1);
    emit variableListChanged();
}

void UiVariableList::onRemoveVariable()
{
    int row = tableWidget_->currentRow();
    if (row < 0 || !variableList_) return;

    size_t index = static_cast<size_t>(row);
    if (index >= variableList_->size()) return;

    Variable* var = variableList_->at(index);
    int ret = QMessageBox::question(
        this, tr("删除变量"),
        tr("确认删除变量 \"%1\" 吗？").arg(QString::fromStdString(var->name())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    variableList_->erase(index);
    refreshUi();
    emit variableListChanged();
}

void UiVariableList::onCellDoubleClicked(int row, int /*column*/)
{
    if (!variableList_ || row < 0 || static_cast<size_t>(row) >= variableList_->size())
        return;

    Variable* var = variableList_->at(static_cast<size_t>(row));

    QDialog dlg(this);
    dlg.setWindowTitle(tr("编辑变量"));

    auto* form = new QFormLayout(&dlg);
    auto* nameEdit = new QLineEdit(QString::fromStdString(var->name()), &dlg);
    form->addRow(tr("名称"), nameEdit);

    auto* exprEdit = addExpressionRow(form, &dlg,
        QString::fromStdString(var->getInnerExpression()));

    auto* descEdit = new QLineEdit(QString::fromStdString(var->desc()), &dlg);
    form->addRow(tr("描述"), descEdit);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    form->addRow(buttons);

    if (dlg.exec() != QDialog::Accepted) return;

    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) return;

    var->setName(name.toStdString());
    var->setExpr(exprEdit->text().trimmed().toStdString());
    var->setDesc(descEdit->text().trimmed().toStdString());

    refreshUi();
    emit variableListChanged();
}

AST_NAMESPACE_END
