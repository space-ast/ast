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
#include "AstScript/Interpreter.hpp"
#include "AstScript/ScriptContext.hpp"
#include "AstScript/Value.hpp"

#include <QStyle>
#include <QToolButton>
#include <QHeaderView>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

enum { COL_NAME = 0, COL_EXPR = 1, COL_VALUE = 2, COL_DESC = 3 };

UiVariableList::UiVariableList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiVariableList::setupUi()
{
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);

    tableWidget_ = new QTableWidget(0, 4, this);
    tableWidget_->setHorizontalHeaderLabels({tr("名称"), tr("表达式"), tr("值"), tr("描述")});
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
    variableListOwner_ = owner;
    refreshUi();
}

void UiVariableList::setInterpreter(Interpreter* interpreter, Object* owner)
{
    interpreter_ = interpreter;
    interpreterOwner_ = owner;
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

        // 表达式列
        auto* exprItem = new QTableWidgetItem(QString::fromStdString(var->getInnerExpression()));
        exprItem->setFlags(exprItem->flags() | Qt::ItemIsEditable);
        tableWidget_->setItem(static_cast<int>(i), COL_EXPR, exprItem);

        // 值列：bind 变量可编辑并写回，否则只读
        auto* valItem = new QTableWidgetItem();
        if (var->isBind())
            valItem->setFlags(valItem->flags() | Qt::ItemIsEditable);
        else
            valItem->setFlags(valItem->flags() & ~Qt::ItemIsEditable);
        SharedPtr<Value> val = var->eval();
        if (val)
            valItem->setText(QString::fromStdString(val->toString()));
        else
            valItem->setText(QStringLiteral("-"));
        tableWidget_->setItem(static_cast<int>(i), COL_VALUE, valItem);

        // 描述列：可内联编辑
        auto* descItem = new QTableWidgetItem(QString::fromStdString(var->desc()));
        descItem->setFlags(descItem->flags() | Qt::ItemIsEditable);
        tableWidget_->setItem(static_cast<int>(i), COL_DESC, descItem);
    }

    // 底部空行：用于快速新建变量
    int newRow = static_cast<int>(n);
    tableWidget_->setRowCount(newRow + 1);
    for (int col = 0; col < 4; ++col)
    {
        auto* item = new QTableWidgetItem();
        item->setData(Qt::UserRole, static_cast<qlonglong>(-1));
        if (col == COL_VALUE)
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        else
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        tableWidget_->setItem(newRow, col, item);
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
    // 通过 variableListOwner_ 的 WeakPtr 判断对象是否有效，避免悬空指针
    if (!variableListOwner_.expired())
        return variableList_;
    return nullptr;
}

Interpreter* UiVariableList::interpreter() const
{
    return interpreter_;
}


void UiVariableList::addExpression(Expr* expr, bool bind)
{
    auto variableList = this->variableList();
    if (!variableList || !expr)
        return;

    // 根据表达式生成默认变量名
    std::string exprStr = expr->getExpression();
    std::string varName = "var_" + std::to_string(variableList->size() + 1);

    auto* var = Variable::New();
    var->setName(varName);
    if(bind)
        var->setBindExpr(expr);
    else
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
    auto variableList = this->variableList();
    if (!variableList)
        return;

    auto* var = Variable::New();
    var->setName("var_" + std::to_string(variableList->size() + 1));

    variableList->append(var);
    refreshUi();
    tableWidget_->selectRow(static_cast<int>(variableList->size()) - 1);
    // 自动进入名称编辑
    tableWidget_->editItem(tableWidget_->item(static_cast<int>(variableList->size()) - 1, COL_NAME));
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

void UiVariableList::onCellChanged(int row, int col)
{
    auto variableList = this->variableList();
    if (!variableList || row < 0)
        return;

    // 底部空行：编辑后自动新建变量
    if (static_cast<size_t>(row) >= variableList->size())
    {
        auto* item = tableWidget_->item(row, col);
        if (!item) return;
        QString text = item->text().trimmed();
        if (text.isEmpty()) return;

        auto* var = Variable::New();
        switch (col)
        {
        case COL_NAME: var->setName(text.toStdString()); break;
        case COL_EXPR: {
            auto* interp = interpreter();
            InterpreterContext ctx(interp);
            Expr* expr = aExec(text.toStdString());
            if (expr)
                var->setExpr(expr);
            else {
                delete var;
                QMessageBox::warning(this, tr("表达式错误"), tr("表达式解析失败"));
                return;
            }
            break;
        }
        case COL_DESC: var->setDesc(text.toStdString()); break;
        }
        variableList->append(var);
        refreshUi();
        emit variableListChanged();
        return;
    }

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
    case COL_EXPR:
    {
        auto* interp = interpreter();
        InterpreterContext ctx(interp);
        Expr* expr = aExec(text.toStdString());
        if(expr)
        {
            var->setExpr(expr);
        }
        else
        {
            QMessageBox::warning(this, tr("表达式错误"), tr("变量 \"%1\" 的表达式错误").arg(QString::fromStdString(var->name())));
            return;
        }
        break;
    }
    case COL_VALUE:
    {
        auto* interp = interpreter();
        InterpreterContext ctx(interp);
        SharedPtr<Value> val = aEval(text.toStdString());
        if(val)
        {
            errc_t rc = var->setValue(val->toString());
            if(rc != eNoError)
            {
                qWarning("Variable::setValue failed: %d", rc);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("值错误"), tr("变量 \"%1\" 的值错误").arg(QString::fromStdString(var->name())));
            return;
        }
        break;
    }
    case COL_DESC:
        var->setDesc(text.toStdString());
        break;
    default:
        return;
    }

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
