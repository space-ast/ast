///
/// @file      UiSweepVariableList.cpp
/// @brief     扫参变量表格实现
/// @details   ~
/// @author    axel
/// @date      2026-06-11
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiSweepVariableList.hpp"
#include "UiExpressionBrowser.hpp"
#include "UiWorkbenchExprPicker.hpp"
#include "AstAnalyzer/StudyWorkbench.hpp"
#include "AstScript/Expr.hpp"
#include "AstGUI/UiCommon.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造
// ============================================================================

UiSweepVariableList::UiSweepVariableList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

// ============================================================================
// 界面搭建
// ============================================================================

void UiSweepVariableList::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 工具栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    addBtn_ = new QPushButton(aUiIcon("Add"), tr("添加变量"), this);
    addBtn_->setToolTip(tr("添加一个新的扫参变量"));

    removeBtn_ = new QPushButton(aUiIcon("Delete"), tr("删除变量"), this);
    removeBtn_->setToolTip(tr("删除选中的变量"));
    removeBtn_->setEnabled(false);

    btnLayout->addWidget(addBtn_);
    btnLayout->addWidget(removeBtn_);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // 表格
    table_ = new QTableWidget(0, VAR_COUNT, this);
    table_->setHorizontalHeaderLabels({
        tr("名称"), tr("起始值"), tr("步长"), tr("结束值"), tr("表达式")
    });
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setVisible(false);
    table_->setColumnWidth(VAR_NAME, 100);
    table_->setColumnWidth(VAR_START, 80);
    table_->setColumnWidth(VAR_STEP, 80);
    table_->setColumnWidth(VAR_END, 80);
    table_->setColumnWidth(VAR_EXPR, 150);
    layout->addWidget(table_);

    // 连接
    connect(addBtn_, &QPushButton::clicked,
            this, &UiSweepVariableList::onAddVariable);
    connect(removeBtn_, &QPushButton::clicked,
            this, &UiSweepVariableList::onRemoveVariable);
    connect(table_, &QTableWidget::cellChanged,
            this, &UiSweepVariableList::onCellChanged);
    connect(table_, &QTableWidget::itemSelectionChanged,
            this, [this]() {
        removeBtn_->setEnabled(table_->currentRow() >= 0);
    });
}

// ============================================================================
// 公共接口
// ============================================================================

void UiSweepVariableList::setStudy(SweepStudy* study)
{
    study_ = study;
    refreshTable();
}

SweepStudy* UiSweepVariableList::study() const
{
    if (!study_.expired())
        return study_.get();
    return nullptr;
}

void UiSweepVariableList::refreshTable()
{
    table_->blockSignals(true);
    table_->setRowCount(0);

    auto* s = study();
    if (!s)
    {
        table_->blockSignals(false);
        return;
    }

    auto& vars = s->variables();
    int n = static_cast<int>(vars.size());
    table_->setRowCount(n);

    for (int i = 0; i < n; ++i)
    {
        auto* var = vars[i].get();
        if (!var)
            continue;

        // 名称
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(var->getName()));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        table_->setItem(i, VAR_NAME, nameItem);

        // 起始值
        auto* startItem = new QTableWidgetItem();
        startItem->setData(Qt::EditRole, var->startValue());
        startItem->setFlags(startItem->flags() | Qt::ItemIsEditable);
        table_->setItem(i, VAR_START, startItem);

        // 步长
        auto* stepItem = new QTableWidgetItem();
        stepItem->setData(Qt::EditRole, var->stepSize());
        stepItem->setFlags(stepItem->flags() | Qt::ItemIsEditable);
        table_->setItem(i, VAR_STEP, stepItem);

        // 结束值
        auto* endItem = new QTableWidgetItem();
        endItem->setData(Qt::EditRole, var->endValue());
        endItem->setFlags(endItem->flags() | Qt::ItemIsEditable);
        table_->setItem(i, VAR_END, endItem);

        // 表达式按钮
        auto* exprBtn = new QPushButton();
        QString exprText = var->expr()
            ? QString::fromStdString(var->expr()->getExpression())
            : tr("(点击选择表达式)");
        exprBtn->setText(exprText);
        exprBtn->setToolTip(tr("点击选择对象属性或计算量表达式"));
        int row = i;
        connect(exprBtn, &QPushButton::clicked, this, [this, row]() {
            onExprClicked(row);
        });
        table_->setCellWidget(i, VAR_EXPR, exprBtn);
    }

    // 底部空行：用于快速新建变量
    int newRow = n;
    table_->setRowCount(newRow + 1);
    for (int col = 0; col < VAR_COUNT; ++col)
    {
        auto* item = new QTableWidgetItem();
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        table_->setItem(newRow, col, item);
    }
    // 空行的表达式按钮
    auto* emptyExprBtn = new QPushButton(tr("(点击选择表达式)"));
    emptyExprBtn->setToolTip(tr("点击选择对象属性或计算量表达式"));
    connect(emptyExprBtn, &QPushButton::clicked, this, [this, newRow]() {
        onExprClicked(newRow);
    });
    table_->setCellWidget(newRow, VAR_EXPR, emptyExprBtn);

    table_->blockSignals(false);
}

// ============================================================================
// 表达式浏览
// ============================================================================

Expr* UiSweepVariableList::browseExpression(QWidget* parent)
{
    // 如果父级是 StudyWorkbench，优先从工作台的输入/输出中选择
    auto* svStudy = study();
    if (svStudy)
    {
        auto* svWb = aobject_cast<StudyWorkbench*>(svStudy->getParentScope());
        if (svWb)
        {
            UiWorkbenchExprPicker picker(svWb, parent);
            if (picker.exec() == QDialog::Accepted)
                return picker.selectedExpr();
            return nullptr;
        }
    }

    return UiExpressionBrowser::GetExpression(parent);
}

// ============================================================================
// 槽 — 添加 / 删除
// ============================================================================

void UiSweepVariableList::onAddVariable()
{
    auto* s = study();
    if (!s)
        return;

    auto* var = aNewObject<SweepVariable>();
    var->setName("var_" + std::to_string(s->variables().size() + 1));

    s->addVariable(var);
    refreshTable();

    // 选中新行并进入名称编辑
    int newRow = static_cast<int>(s->variables().size()) - 1;
    table_->selectRow(newRow);
    table_->editItem(table_->item(newRow, VAR_NAME));
}

void UiSweepVariableList::onRemoveVariable()
{
    int row = table_->currentRow();
    auto* s = study();
    if (row < 0 || !s)
        return;

    auto& vars = s->variables();
    if (row >= static_cast<int>(vars.size()))
        return;

    auto* var = vars[row].get();
    if (!var)
        return;

    int ret = QMessageBox::question(
        this, tr("删除变量"),
        tr("确认删除变量 \"%1\" 吗？").arg(QString::fromStdString(var->getName())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes)
        return;

    s->removeVariable(var);
    refreshTable();
}

// ============================================================================
// 槽 — 单元格编辑
// ============================================================================

void UiSweepVariableList::onCellChanged(int row, int col)
{
    auto* s = study();
    if (!s)
        return;

    auto& vars = s->variables();

    // 底部空行：编辑后自动新建变量
    if (row >= static_cast<int>(vars.size()))
    {
        auto* item = table_->item(row, col);
        if (!item)
            return;
        QString text = item->text().trimmed();
        if (text.isEmpty())
            return;

        auto* var = aNewObject<SweepVariable>();
        var->setName("var_" + std::to_string(vars.size() + 1));

        switch (col)
        {
        case VAR_NAME:
            var->setName(text.toStdString());
            break;
        case VAR_START:
        {
            bool ok = false;
            double val = item->data(Qt::EditRole).toDouble(&ok);
            if (ok)
                var->setStartValue(val);
            break;
        }
        case VAR_STEP:
        {
            bool ok = false;
            double val = item->data(Qt::EditRole).toDouble(&ok);
            if (ok)
                var->setStepSize(val);
            break;
        }
        case VAR_END:
        {
            bool ok = false;
            double val = item->data(Qt::EditRole).toDouble(&ok);
            if (ok)
                var->setEndValue(val);
            break;
        }
        }

        s->addVariable(var);
        refreshTable();
        return;
    }

    auto* var = vars[row].get();
    if (!var)
        return;

    auto* item = table_->item(row, col);
    if (!item)
        return;

    bool ok = false;
    switch (col)
    {
    case VAR_NAME:
    {
        QString text = item->text().trimmed();
        if (!text.isEmpty())
            var->setName(text.toStdString());
        break;
    }
    case VAR_START:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            var->setStartValue(val);
        break;
    }
    case VAR_STEP:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            var->setStepSize(val);
        break;
    }
    case VAR_END:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            var->setEndValue(val);
        break;
    }
    default:
        break;
    }
}

// ============================================================================
// 槽 — 表达式按钮点击
// ============================================================================

void UiSweepVariableList::onExprClicked(int row)
{
    auto* s = study();
    if (!s)
        return;

    auto& vars = s->variables();

    Expr* expr = browseExpression(this);
    if (!expr)
        return;

    // 底部空行：自动新建变量
    if (row >= static_cast<int>(vars.size()))
    {
        auto* var = aNewObject<SweepVariable>();
        var->setName("var_" + std::to_string(vars.size() + 1));
        var->setExpr(expr);
        s->addVariable(var);
        refreshTable();
        return;
    }

    auto* var = vars[row].get();
    if (!var)
        return;

    var->setExpr(expr);
    auto* btn = qobject_cast<QPushButton*>(table_->cellWidget(row, VAR_EXPR));
    if (btn)
        btn->setText(QString::fromStdString(expr->getExpression()));
}

AST_NAMESPACE_END
