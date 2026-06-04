///
/// @file      UiVariableList.cpp
/// @brief     脚本变量列表编辑控件实现
/// @details   支持内联编辑名称和描述、拖拽排序、表达式浏览联动
/// @author    axel
/// @date      2026-05-24
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

#include "UiVariableList.hpp"
#include "UiExpressionBrowser.hpp"
#include "MissionIcons.hpp"
#include "AstGUI/UiCommon.hpp"
#include "AstScript/Variable.hpp"

#include <QDialog>
#include <QStyle>
#include <QToolButton>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSplitter>
#include <QDrag>
#include <QMimeData>

AST_NAMESPACE_BEGIN

enum { COL_NAME = 0, COL_EXPR = 1, COL_DESC = 2 };

namespace {

/// @brief 在 form 中添加带浏览按钮的表达式输入行
/// @param form 表单布局
/// @param parent 父对话框
/// @param initialText 初始文本
/// @param outExpr [out] 用户浏览选择的表达式（若未浏览则为空）
/// @return QLineEdit* 文本编辑框
QLineEdit* addExpressionRow(QFormLayout* form, QDialog* parent,
                             const QString& initialText, SharedPtr<Expr>& outExpr)
{
    auto* row = new QHBoxLayout;
    auto* edit = new QLineEdit(initialText, parent);
    auto* browseBtn = new QPushButton("...", parent);
    browseBtn->setToolTip(QObject::tr("浏览对象属性和计算量"));
    browseBtn->setFixedWidth(30);
    row->addWidget(edit);
    row->addWidget(browseBtn);
    form->addRow(QObject::tr("表达式"), row);

    QObject::connect(browseBtn, &QPushButton::clicked, [edit, parent, &outExpr]() {
        SharedPtr<Expr> expr = UiExpressionBrowser::GetExpression(parent);
        if (expr)
        {
            outExpr = expr;
            edit->setText(QString::fromStdString(expr->getExpression()));
        }
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

    // 启用内联编辑：双击或 F2 进入编辑
    tableWidget_->setEditTriggers(QAbstractItemView::DoubleClicked |
                                   QAbstractItemView::EditKeyPressed);
    tableWidget_->horizontalHeader()->setStretchLastSection(true);
    tableWidget_->verticalHeader()->setVisible(false);
    tableWidget_->setColumnWidth(COL_NAME, 120);
    tableWidget_->setColumnWidth(COL_EXPR, 150);

    // 启用拖拽排序（仅内部移动）
    tableWidget_->setDragEnabled(true);
    tableWidget_->setAcceptDrops(true);
    tableWidget_->setDropIndicatorShown(true);
    tableWidget_->setDragDropMode(QAbstractItemView::InternalMove);
    tableWidget_->setDragDropOverwriteMode(false);
    tableWidget_->installEventFilter(this);

    // 工具栏按钮（表格上方）
    buttonLayout_ = new QHBoxLayout;
    buttonLayout_->setContentsMargins(0, 0, 0, 0);

    addButton_ = new QToolButton(this);
    addButton_->setIcon(aUiIcon("Add"));
    addButton_->setToolTip(tr("添加新变量"));
    addButton_->setAutoRaise(true);

    removeButton_ = new QToolButton(this);
    removeButton_->setIcon(aUiIcon("Delete"));
    removeButton_->setToolTip(tr("删除选中变量"));
    removeButton_->setAutoRaise(true);
    removeButton_->setEnabled(false);

    refreshButton_ = new QToolButton(this);
    refreshButton_->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    refreshButton_->setToolTip(tr("刷新变量列表"));
    refreshButton_->setAutoRaise(true);

    buttonLayout_->addWidget(addButton_);
    buttonLayout_->addWidget(removeButton_);
    buttonLayout_->addWidget(refreshButton_);
    buttonLayout_->addStretch();
    mainLayout_->addLayout(buttonLayout_);
    mainLayout_->addWidget(tableWidget_);

    connect(tableWidget_, &QTableWidget::itemSelectionChanged,
            this, &UiVariableList::onSelectionChanged);
    connect(tableWidget_, &QTableWidget::cellDoubleClicked,
            this, &UiVariableList::onCellDoubleClicked);
    connect(tableWidget_, &QTableWidget::cellChanged,
            this, &UiVariableList::onCellChanged);
    connect(addButton_, &QToolButton::clicked,
            this, &UiVariableList::onAddVariable);
    connect(removeButton_, &QToolButton::clicked,
            this, &UiVariableList::onRemoveVariable);
    connect(refreshButton_, &QToolButton::clicked,
            this, &UiVariableList::onRefresh);
}

void UiVariableList::setVariableList(VariableList* variableList, Object* owner)
{
    variableList_ = variableList;
    owner_ = owner;
    refreshUi();
}

void UiVariableList::refreshUi()
{
    // 刷新期间阻止信号，避免 cellChanged 误触发
    tableWidget_->blockSignals(true);
    tableWidget_->setRowCount(0);

    auto variableList = this->variableList();
    if (!variableList)
    {
        tableWidget_->blockSignals(false);
        return;
    }

    size_t n = variableList->size();
    tableWidget_->setRowCount(static_cast<int>(n));
    for (size_t i = 0; i < n; ++i)
    {
        Variable* var = variableList->at(i);
        if (!var) continue;

        // 名称列：可内联编辑
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(var->name()));
        nameItem->setData(Qt::UserRole, static_cast<qlonglong>(i));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        tableWidget_->setItem(static_cast<int>(i), COL_NAME, nameItem);

        // 表达式列：显示表达式字符串，不可内联编辑（双击弹窗）
        std::string exprStr = var->getInnerExpression();
        auto* exprItem = new QTableWidgetItem(QString::fromStdString(exprStr));
        exprItem->setFlags(exprItem->flags() & ~Qt::ItemIsEditable);
        tableWidget_->setItem(static_cast<int>(i), COL_EXPR, exprItem);

        // 描述列：可内联编辑
        auto* descItem = new QTableWidgetItem(QString::fromStdString(var->desc()));
        descItem->setFlags(descItem->flags() | Qt::ItemIsEditable);
        tableWidget_->setItem(static_cast<int>(i), COL_DESC, descItem);
    }

    tableWidget_->blockSignals(false);
}

Variable* UiVariableList::selectedVariable() const
{
    int row = tableWidget_->currentRow();
    auto variableList = this->variableList();
    if (row < 0 || !variableList) return nullptr;

    size_t index = static_cast<size_t>(row);
    if (index < variableList->size())
        return variableList->at(index);
    return nullptr;
}

VariableList* UiVariableList::variableList() const
{
    // 通过 owner_ 的 WeakPtr 判断对象是否有效，避免悬空指针
    if (!owner_.expired())
        return variableList_;
    return nullptr;
}

void UiVariableList::addExpression(Expr* expr)
{
    auto variableList = this->variableList();
    if (!variableList || !expr)
        return;

    // 根据表达式生成默认变量名
    std::string exprStr = expr->getExpression();
    std::string varName = "var_" + std::to_string(variableList->size() + 1);

    auto* var = Variable::New();
    var->setName(varName);
    var->setExpr(expr);
    var->setDesc(exprStr);

    variableList->append(var);
    refreshUi();
    tableWidget_->selectRow(static_cast<int>(variableList->size()) - 1);
    emit variableListChanged();
}

void UiVariableList::onSelectionChanged()
{
    Variable* var = selectedVariable();
    removeButton_->setEnabled(var != nullptr);
    emit variableSelected(var);
    if (var)
        emit variableFocused();
}

void UiVariableList::onRefresh()
{
    refreshUi();
}

void UiVariableList::onAddVariable()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("新建变量"));

    auto* form = new QFormLayout(&dlg);
    auto* nameEdit = new QLineEdit(("var"), &dlg);
    form->addRow(tr("名称"), nameEdit);

    SharedPtr<Expr> browsedExpr;
    auto* exprEdit = addExpressionRow(form, &dlg, ("0"), browsedExpr);

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
    if (browsedExpr)
        var->setExpr(browsedExpr.get());
    else
        var->setExpr(exprEdit->text().trimmed().toStdString());
    var->setDesc(descEdit->text().trimmed().toStdString());

    auto variableList = this->variableList();
    if (!variableList)
    {
        delete var;
        return;
    }

    variableList->append(var);
    refreshUi();
    tableWidget_->selectRow(static_cast<int>(variableList->size()) - 1);
    emit variableListChanged();
}

void UiVariableList::onRemoveVariable()
{
    int row = tableWidget_->currentRow();
    auto variableList = this->variableList();
    if (row < 0 || !variableList) return;

    size_t index = static_cast<size_t>(row);
    if (index >= variableList->size()) return;

    Variable* var = variableList->at(index);
    int ret = QMessageBox::question(
        this, tr("删除变量"),
        tr("确认删除变量 \"%1\" 吗？").arg(QString::fromStdString(var->name())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes) return;

    variableList->erase(index);
    refreshUi();
    emit variableListChanged();
}

void UiVariableList::onCellDoubleClicked(int row, int column)
{
    // 仅表达式列需要弹窗编辑；名称和描述由内联编辑处理
    if (column == COL_EXPR)
        openExpressionEditor(row);
}

void UiVariableList::onCellChanged(int row, int col)
{
    auto variableList = this->variableList();
    if (!variableList || row < 0 || static_cast<size_t>(row) >= variableList->size())
        return;

    Variable* var = variableList->at(static_cast<size_t>(row));
    auto* item = tableWidget_->item(row, col);
    if (!item || !var)
        return;

    QString text = item->text().trimmed();
    if (text.isEmpty())
        return;

    switch (col)
    {
    case COL_NAME:
        var->setName(text.toStdString());
        break;
    case COL_DESC:
        var->setDesc(text.toStdString());
        break;
    default:
        return;
    }

    emit variableListChanged();
}

void UiVariableList::openExpressionEditor(int row)
{
    auto variableList = this->variableList();
    if (!variableList || row < 0 || static_cast<size_t>(row) >= variableList->size())
        return;

    Variable* var = variableList->at(static_cast<size_t>(row));

    QDialog dlg(this);
    dlg.setWindowTitle(tr("编辑表达式 — %1").arg(QString::fromStdString(var->name())));

    auto* layout = new QVBoxLayout(&dlg);

    // 当前表达式显示
    auto* currentLabel = new QLabel(
        tr("当前: %1").arg(QString::fromStdString(var->getInnerExpression())), &dlg);
    currentLabel->setWordWrap(true);
    layout->addWidget(currentLabel);

    auto* form = new QFormLayout;
    SharedPtr<Expr> browsedExpr;
    auto* exprEdit = addExpressionRow(form, &dlg,
        QString::fromStdString(var->getInnerExpression()), browsedExpr);
    layout->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    layout->addWidget(buttons);

    if (dlg.exec() != QDialog::Accepted) return;

    if (browsedExpr)
        var->setExpr(browsedExpr.get());
    else
        var->setExpr(exprEdit->text().trimmed().toStdString());

    refreshUi();
    emit variableListChanged();
}

bool UiVariableList::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == tableWidget_ && event->type() == QEvent::Drop)
    {
        // 保存拖拽前的原始 VariableList 索引
        int fromRow = tableWidget_->currentRow();
        auto* nameItem = tableWidget_->item(fromRow, COL_NAME);
        qlonglong fromIdx = nameItem ? nameItem->data(Qt::UserRole).toLongLong() : -1;

        // 让 Qt 先处理拖拽事件（返回 false 将事件传递给 tableWidget_）
        // 延迟同步内核顺序：等 Qt 完成行移动后再更新
        if (fromIdx >= 0)
        {
            QMetaObject::invokeMethod(this, [this, fromIdx]() {
                int toRow = -1;
                for (int i = 0; i < tableWidget_->rowCount(); ++i)
                {
                    auto* item = tableWidget_->item(i, COL_NAME);
                    if (item && item->data(Qt::UserRole).toLongLong() == fromIdx)
                    {
                        toRow = i;
                        break;
                    }
                }
                if (toRow >= 0 && static_cast<size_t>(toRow) != static_cast<size_t>(fromIdx))
                    syncOrderFromTable();
            }, Qt::QueuedConnection);
        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

void UiVariableList::syncOrderFromTable()
{
    auto varList = this->variableList();
    if (!varList)
        return;

    // 按表格当前行顺序收集变量
    std::vector<Variable*> newOrder;
    newOrder.reserve(tableWidget_->rowCount());
    for (int i = 0; i < tableWidget_->rowCount(); ++i)
    {
        auto* item = tableWidget_->item(i, COL_NAME);
        if (!item) continue;
        size_t origIdx = static_cast<size_t>(item->data(Qt::UserRole).toLongLong());
        if (origIdx < varList->size())
            newOrder.push_back(varList->at(origIdx));
    }

    if (newOrder.size() != varList->size())
        return;

    // 重建 VariableList 顺序：逐个 move 到目标位置
    for (size_t i = 0; i < newOrder.size(); ++i)
    {
        // 找到 newOrder[i] 在当前 VariableList 中的位置
        for (size_t j = i; j < varList->size(); ++j)
        {
            if (varList->at(j) == newOrder[i])
            {
                if (j != i)
                    varList->move(j, i);
                break;
            }
        }
    }

    // 更新所有 UserRole 以匹配新顺序
    for (int i = 0; i < tableWidget_->rowCount(); ++i)
    {
        auto* item = tableWidget_->item(i, COL_NAME);
        if (item)
            item->setData(Qt::UserRole, static_cast<qlonglong>(i));
    }

    emit variableListChanged();
}

AST_NAMESPACE_END
