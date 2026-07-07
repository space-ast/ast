///
/// @file      UiSweepOutputList.cpp
/// @brief     扫参输出表格实现
/// @details   ~
/// @author    axel
/// @date      2026-06-11
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiSweepOutputList.hpp"
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

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造
// ============================================================================

UiSweepOutputList::UiSweepOutputList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

// ============================================================================
// 界面搭建
// ============================================================================

void UiSweepOutputList::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 工具栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    addBtn_ = new QPushButton(aUiIcon("Add"), tr("添加约束"), this);
    addBtn_->setToolTip(tr("添加一个新的输出约束"));

    removeBtn_ = new QPushButton(aUiIcon("Delete"), tr("删除约束"), this);
    removeBtn_->setToolTip(tr("删除选中的约束"));
    removeBtn_->setEnabled(false);

    btnLayout->addWidget(addBtn_);
    btnLayout->addWidget(removeBtn_);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // 表格
    table_ = new QTableWidget(0, OUT_COUNT, this);
    table_->setHorizontalHeaderLabels({
        tr("名称"), tr("表达式")
    });
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setVisible(false);
    table_->setColumnWidth(OUT_NAME, 150);
    table_->setColumnWidth(OUT_EXPR, 200);
    layout->addWidget(table_);

    // 连接
    connect(addBtn_, &QPushButton::clicked,
            this, &UiSweepOutputList::onAddOutput);
    connect(removeBtn_, &QPushButton::clicked,
            this, &UiSweepOutputList::onRemoveOutput);
    connect(table_, &QTableWidget::cellChanged,
            this, &UiSweepOutputList::onCellChanged);
    connect(table_, &QTableWidget::itemSelectionChanged,
            this, [this]() {
        removeBtn_->setEnabled(table_->currentRow() >= 0);
    });
}

// ============================================================================
// 公共接口
// ============================================================================

void UiSweepOutputList::setStudy(SweepStudy* study)
{
    study_ = study;
    refreshTable();
}

SweepStudy* UiSweepOutputList::study() const
{
    if (!study_.expired())
        return study_.get();
    return nullptr;
}

void UiSweepOutputList::refreshTable()
{
    table_->blockSignals(true);
    table_->setRowCount(0);

    auto* s = study();
    if (!s)
    {
        table_->blockSignals(false);
        return;
    }

    auto& outputs = s->outputs();
    int n = static_cast<int>(outputs.size());
    table_->setRowCount(n);

    for (int i = 0; i < n; ++i)
    {
        auto* out = outputs[i].get();
        if (!out)
            continue;

        // 名称
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(out->getName()));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        table_->setItem(i, OUT_NAME, nameItem);

        // 表达式按钮
        auto* exprBtn = new QPushButton();
        QString exprText = out->expr()
            ? QString::fromStdString(out->expr()->getExpression())
            : tr("(点击选择表达式)");
        exprBtn->setText(exprText);
        exprBtn->setToolTip(tr("点击选择对象属性或计算量表达式"));
        int row = i;
        connect(exprBtn, &QPushButton::clicked, this, [this, row]() {
            onExprClicked(row);
        });
        table_->setCellWidget(i, OUT_EXPR, exprBtn);
    }

    // 底部空行：用于快速新建输出
    int newRow = n;
    table_->setRowCount(newRow + 1);
    for (int col = 0; col < OUT_COUNT; ++col)
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
    table_->setCellWidget(newRow, OUT_EXPR, emptyExprBtn);

    table_->blockSignals(false);
}

// ============================================================================
// 表达式浏览
// ============================================================================

Expr* UiSweepOutputList::browseExpression(QWidget* parent)
{
    // 如果父级是 StudyWorkbench，优先从工作台的输入/输出中选择
    auto* soStudy = study();
    if (soStudy)
    {
        auto* soWb = aobject_cast<StudyWorkbench*>(soStudy->getParentScope());
        if (soWb)
        {
            UiWorkbenchExprPicker picker(soWb, parent);
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

void UiSweepOutputList::onAddOutput()
{
    auto* s = study();
    if (!s)
        return;

    auto* out = aNewObject<SweepOutput>();
    out->setName("out_" + std::to_string(s->outputs().size() + 1));

    s->addOutput(out);
    refreshTable();

    // 选中新行并进入名称编辑
    int newRow = static_cast<int>(s->outputs().size()) - 1;
    table_->selectRow(newRow);
    table_->editItem(table_->item(newRow, OUT_NAME));
}

void UiSweepOutputList::onRemoveOutput()
{
    int row = table_->currentRow();
    auto* s = study();
    if (row < 0 || !s)
        return;

    auto& outputs = s->outputs();
    if (row >= static_cast<int>(outputs.size()))
        return;

    auto* out = outputs[row].get();
    if (!out)
        return;

    int ret = QMessageBox::question(
        this, tr("删除约束"),
        tr("确认删除约束 \"%1\" 吗？").arg(QString::fromStdString(out->getName())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes)
        return;

    s->removeOutput(out);
    refreshTable();
}

// ============================================================================
// 槽 — 单元格编辑
// ============================================================================

void UiSweepOutputList::onCellChanged(int row, int col)
{
    auto* s = study();
    if (!s)
        return;

    auto& outputs = s->outputs();

    // 底部空行：编辑后自动新建输出
    if (row >= static_cast<int>(outputs.size()))
    {
        auto* item = table_->item(row, col);
        if (!item)
            return;
        QString text = item->text().trimmed();
        if (text.isEmpty())
            return;

        auto* out = aNewObject<SweepOutput>();
        out->setName(text.toStdString());

        s->addOutput(out);
        refreshTable();
        return;
    }

    auto* out = outputs[row].get();
    if (!out)
        return;

    if (col == OUT_NAME)
    {
        auto* item = table_->item(row, col);
        if (!item)
            return;
        QString text = item->text().trimmed();
        if (!text.isEmpty())
            out->setName(text.toStdString());
    }
}

// ============================================================================
// 槽 — 表达式按钮点击
// ============================================================================

void UiSweepOutputList::onExprClicked(int row)
{
    auto* s = study();
    if (!s)
        return;

    auto& outputs = s->outputs();

    Expr* expr = browseExpression(this);
    if (!expr)
        return;

    // 底部空行：自动新建输出
    if (row >= static_cast<int>(outputs.size()))
    {
        auto* out = aNewObject<SweepOutput>();
        out->setName("out_" + std::to_string(outputs.size() + 1));
        out->setExpr(expr);
        s->addOutput(out);
        refreshTable();
        return;
    }

    auto* out = outputs[row].get();
    if (!out)
        return;

    out->setExpr(expr);
    auto* btn = qobject_cast<QPushButton*>(table_->cellWidget(row, OUT_EXPR));
    if (btn)
        btn->setText(QString::fromStdString(expr->getExpression()));
}

AST_NAMESPACE_END
