///
/// @file      UiFeasibleRegionStudy.cpp
/// @brief     FeasibleRegionStudy 段编辑器实现
/// @details   ~
/// @author    axel
/// @date      2026-06-11
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

#include "UiFeasibleRegionStudy.hpp"
#include "UiExpressionBrowser.hpp"
#include "AstAnalyzer/FeasibleRegionStudy.hpp"
#include "AstScript/Expr.hpp"
#include "AstGUI/UiCommon.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>

AST_NAMESPACE_BEGIN

// ============================================================================
// 构造
// ============================================================================

UiFeasibleRegionStudy::UiFeasibleRegionStudy(Object* object, QWidget* parent)
    : UiFeasibleRegionStudy(parent)
{
    setStudy(aobject_cast<FeasibleRegionStudy*>(object));
}

UiFeasibleRegionStudy::UiFeasibleRegionStudy(QWidget* parent)
    : UiObject(parent)
{
    setupUi();
}

UiFeasibleRegionStudy::~UiFeasibleRegionStudy() = default;

// ============================================================================
// 界面搭建
// ============================================================================

void UiFeasibleRegionStudy::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    tabWidget_ = new QTabWidget(this);

    // 标签页1: 变量
    auto* varTab = new QWidget();
    setupVariablesTab(varTab);
    tabWidget_->addTab(varTab, tr("变量"));

    // 标签页2: 约束
    auto* ctrTab = new QWidget();
    setupConstraintsTab(ctrTab);
    tabWidget_->addTab(ctrTab, tr("约束"));

    // 标签页3: 设置
    auto* settingsTab = new QWidget();
    setupSettingsTab(settingsTab);
    tabWidget_->addTab(settingsTab, tr("设置"));

    rootLayout->addWidget(tabWidget_);
}

void UiFeasibleRegionStudy::setupVariablesTab(QWidget* tab)
{
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(4, 4, 4, 4);

    // 工具栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    addVarBtn_ = new QPushButton(aUiIcon("Add"), tr("添加变量"), tab);
    addVarBtn_->setToolTip(tr("添加一个新的可行域变量"));

    removeVarBtn_ = new QPushButton(aUiIcon("Delete"), tr("删除变量"), tab);
    removeVarBtn_->setToolTip(tr("删除选中的变量"));
    removeVarBtn_->setEnabled(false);

    btnLayout->addWidget(addVarBtn_);
    btnLayout->addWidget(removeVarBtn_);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // 变量表格
    varTable_ = new QTableWidget(0, VAR_COUNT, tab);
    varTable_->setHorizontalHeaderLabels({
        tr("名称"), tr("表达式"), tr("下界"), tr("上界"), tr("采样点数")
    });
    varTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    varTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    varTable_->horizontalHeader()->setStretchLastSection(true);
    varTable_->verticalHeader()->setVisible(false);
    varTable_->setColumnWidth(VAR_NAME, 100);
    varTable_->setColumnWidth(VAR_EXPR, 150);
    varTable_->setColumnWidth(VAR_LOWER, 80);
    varTable_->setColumnWidth(VAR_UPPER, 80);
    varTable_->setColumnWidth(VAR_STEPS, 80);
    layout->addWidget(varTable_);

    // 连接
    connect(addVarBtn_, &QPushButton::clicked,
            this, &UiFeasibleRegionStudy::onAddVariable);
    connect(removeVarBtn_, &QPushButton::clicked,
            this, &UiFeasibleRegionStudy::onRemoveVariable);
    connect(varTable_, &QTableWidget::cellChanged,
            this, &UiFeasibleRegionStudy::onVariableCellChanged);
    connect(varTable_, &QTableWidget::itemSelectionChanged,
            this, [this]() {
        removeVarBtn_->setEnabled(varTable_->currentRow() >= 0);
    });
}

void UiFeasibleRegionStudy::setupConstraintsTab(QWidget* tab)
{
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(4, 4, 4, 4);

    // 工具栏
    auto* btnLayout = new QHBoxLayout();
    btnLayout->setContentsMargins(0, 0, 0, 0);

    addCtrBtn_ = new QPushButton(aUiIcon("Add"), tr("添加约束"), tab);
    addCtrBtn_->setToolTip(tr("添加一个新的可行域约束"));

    removeCtrBtn_ = new QPushButton(aUiIcon("Delete"), tr("删除约束"), tab);
    removeCtrBtn_->setToolTip(tr("删除选中的约束"));
    removeCtrBtn_->setEnabled(false);

    btnLayout->addWidget(addCtrBtn_);
    btnLayout->addWidget(removeCtrBtn_);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    // 约束表格
    ctrTable_ = new QTableWidget(0, CTR_COUNT, tab);
    ctrTable_->setHorizontalHeaderLabels({
        tr("名称"), tr("表达式"), tr("启用下界"), tr("下界"),
        tr("启用上界"), tr("上界"), tr("排除模式")
    });
    ctrTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    ctrTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    ctrTable_->horizontalHeader()->setStretchLastSection(true);
    ctrTable_->verticalHeader()->setVisible(false);
    ctrTable_->setColumnWidth(CTR_NAME, 100);
    ctrTable_->setColumnWidth(CTR_EXPR, 150);
    ctrTable_->setColumnWidth(CTR_USE_LOWER, 70);
    ctrTable_->setColumnWidth(CTR_LOWER, 80);
    ctrTable_->setColumnWidth(CTR_USE_UPPER, 70);
    ctrTable_->setColumnWidth(CTR_UPPER, 80);
    ctrTable_->setColumnWidth(CTR_EXCLUDE, 70);
    layout->addWidget(ctrTable_);

    // 连接
    connect(addCtrBtn_, &QPushButton::clicked,
            this, &UiFeasibleRegionStudy::onAddConstraint);
    connect(removeCtrBtn_, &QPushButton::clicked,
            this, &UiFeasibleRegionStudy::onRemoveConstraint);
    connect(ctrTable_, &QTableWidget::cellChanged,
            this, &UiFeasibleRegionStudy::onConstraintCellChanged);
    connect(ctrTable_, &QTableWidget::itemSelectionChanged,
            this, [this]() {
        removeCtrBtn_->setEnabled(ctrTable_->currentRow() >= 0);
    });
}

void UiFeasibleRegionStudy::setupSettingsTab(QWidget* tab)
{
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* infoGroup = new QGroupBox(tr("运行信息"), tab);
    auto* infoLayout = new QVBoxLayout(infoGroup);

    totalRunsLabel_ = new QLabel(tab);
    totalRunsLabel_->setWordWrap(true);
    infoLayout->addWidget(totalRunsLabel_);

    auto* hintLabel = new QLabel(tab);
    hintLabel->setWordWrap(true);
    hintLabel->setText(tr(
        "可行域研究将对每个变量在 [下界, 上界] 范围内等距采样，"
        "遍历所有变量组合，每次迭代设置变量值、执行关联命令、验证约束条件，"
        "最终确定满足所有约束的可行区域。\n\n"
        "关联命令通过右键菜单中的\"关联到可行域研究\"设置。"
    ));
    infoLayout->addWidget(hintLabel);

    layout->addWidget(infoGroup);
    layout->addStretch();
}

// ============================================================================
// 公共接口
// ============================================================================

void UiFeasibleRegionStudy::setStudy(FeasibleRegionStudy* study)
{
    if (!study)
        return;
    setObject(study);
    refreshFromStudy();
}

FeasibleRegionStudy* UiFeasibleRegionStudy::getStudy() const
{
    return aobject_cast<FeasibleRegionStudy*>(getObject());
}

void UiFeasibleRegionStudy::refreshFromStudy()
{
    refreshVariablesTable();
    refreshConstraintsTable();
    updateTotalRunsLabel();
}

// ============================================================================
// 变量表刷新
// ============================================================================

void UiFeasibleRegionStudy::refreshVariablesTable()
{
    auto* study = getStudy();
    if (!study)
        return;

    varTable_->blockSignals(true);
    varTable_->setRowCount(0);

    auto& vars = study->variables();
    int n = static_cast<int>(vars.size());
    varTable_->setRowCount(n);

    for (int i = 0; i < n; ++i)
    {
        auto* var = vars[i].get();
        if (!var)
            continue;

        // 名称
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(var->getName()));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        varTable_->setItem(i, VAR_NAME, nameItem);

        // 表达式按钮
        auto* exprBtn = new QPushButton();
        QString exprText = var->expr()
            ? QString::fromStdString(var->expr()->getExpression())
            : tr("(点击选择表达式)");
        exprBtn->setText(exprText);
        exprBtn->setToolTip(tr("点击选择对象属性或计算量表达式"));
        // 用 row 捕获以便在 lambda 中知道是哪一行
        int row = i;
        connect(exprBtn, &QPushButton::clicked, this, [this, row]() {
            onVariableExprClicked(row);
        });
        varTable_->setCellWidget(i, VAR_EXPR, exprBtn);

        // 下界
        auto* lowerItem = new QTableWidgetItem();
        lowerItem->setData(Qt::EditRole, var->lower());
        lowerItem->setFlags(lowerItem->flags() | Qt::ItemIsEditable);
        varTable_->setItem(i, VAR_LOWER, lowerItem);

        // 上界
        auto* upperItem = new QTableWidgetItem();
        upperItem->setData(Qt::EditRole, var->upper());
        upperItem->setFlags(upperItem->flags() | Qt::ItemIsEditable);
        varTable_->setItem(i, VAR_UPPER, upperItem);

        // 采样点数
        auto* stepsItem = new QTableWidgetItem();
        stepsItem->setData(Qt::EditRole, var->steps());
        stepsItem->setFlags(stepsItem->flags() | Qt::ItemIsEditable);
        varTable_->setItem(i, VAR_STEPS, stepsItem);
    }

    varTable_->blockSignals(false);
    updateTotalRunsLabel();
}

// ============================================================================
// 约束表刷新
// ============================================================================

void UiFeasibleRegionStudy::refreshConstraintsTable()
{
    auto* study = getStudy();
    if (!study)
        return;

    ctrTable_->blockSignals(true);
    ctrTable_->setRowCount(0);

    auto& ctrs = study->constraints();
    int n = static_cast<int>(ctrs.size());
    ctrTable_->setRowCount(n);

    for (int i = 0; i < n; ++i)
    {
        auto* ctr = ctrs[i].get();
        if (!ctr)
            continue;

        // 名称
        auto* nameItem = new QTableWidgetItem(QString::fromStdString(ctr->getName()));
        nameItem->setFlags(nameItem->flags() | Qt::ItemIsEditable);
        ctrTable_->setItem(i, CTR_NAME, nameItem);

        // 表达式按钮
        auto* exprBtn = new QPushButton();
        QString exprText = ctr->expr()
            ? QString::fromStdString(ctr->expr()->getExpression())
            : tr("(点击选择表达式)");
        exprBtn->setText(exprText);
        exprBtn->setToolTip(tr("点击选择对象属性或计算量表达式"));
        int row = i;
        connect(exprBtn, &QPushButton::clicked, this, [this, row]() {
            onConstraintExprClicked(row);
        });
        ctrTable_->setCellWidget(i, CTR_EXPR, exprBtn);

        // 启用下界 (checkbox)
        auto* useLowerCb = new QCheckBox();
        useLowerCb->setChecked(ctr->useLower());
        connect(useLowerCb, &QCheckBox::toggled, this, [this, row](bool checked) {
            if (auto* study = getStudy())
            {
                auto& ctrs = study->constraints();
                if (row < static_cast<int>(ctrs.size()) && ctrs[row])
                    ctrs[row]->setUseLower(checked);
            }
        });
        ctrTable_->setCellWidget(i, CTR_USE_LOWER, useLowerCb);

        // 下界值
        auto* lowerItem = new QTableWidgetItem();
        lowerItem->setData(Qt::EditRole, ctr->lower());
        lowerItem->setFlags(lowerItem->flags() | Qt::ItemIsEditable);
        ctrTable_->setItem(i, CTR_LOWER, lowerItem);

        // 启用上界 (checkbox)
        auto* useUpperCb = new QCheckBox();
        useUpperCb->setChecked(ctr->useUpper());
        connect(useUpperCb, &QCheckBox::toggled, this, [this, row](bool checked) {
            if (auto* study = getStudy())
            {
                auto& ctrs = study->constraints();
                if (row < static_cast<int>(ctrs.size()) && ctrs[row])
                    ctrs[row]->setUseUpper(checked);
            }
        });
        ctrTable_->setCellWidget(i, CTR_USE_UPPER, useUpperCb);

        // 上界值
        auto* upperItem = new QTableWidgetItem();
        upperItem->setData(Qt::EditRole, ctr->upper());
        upperItem->setFlags(upperItem->flags() | Qt::ItemIsEditable);
        ctrTable_->setItem(i, CTR_UPPER, upperItem);

        // 排除模式 (checkbox)
        auto* excludeCb = new QCheckBox();
        excludeCb->setChecked(ctr->exclude());
        excludeCb->setToolTip(tr("排除模式：可行域为区间补集"));
        connect(excludeCb, &QCheckBox::toggled, this, [this, row](bool checked) {
            if (auto* study = getStudy())
            {
                auto& ctrs = study->constraints();
                if (row < static_cast<int>(ctrs.size()) && ctrs[row])
                    ctrs[row]->setExclude(checked);
            }
        });
        ctrTable_->setCellWidget(i, CTR_EXCLUDE, excludeCb);
    }

    ctrTable_->blockSignals(false);
}

// ============================================================================
// 总运行次数
// ============================================================================

void UiFeasibleRegionStudy::updateTotalRunsLabel()
{
    auto* study = getStudy();
    if (!study)
    {
        totalRunsLabel_->setText(tr("无可行域研究"));
        return;
    }

    int total = study->totalRuns();
    auto& vars = study->variables();

    QString detail;
    if (vars.empty())
    {
        detail = tr("总运行次数: 0（未定义变量）");
    }
    else
    {
        detail = tr("总运行次数: %1\n").arg(total);
        for (size_t i = 0; i < vars.size(); ++i)
        {
            auto* var = vars[i].get();
            if (!var)
                continue;
            detail += QString("  %1: [%2, %3] × %4 点\n")
                .arg(QString::fromStdString(var->getName()))
                .arg(var->lower())
                .arg(var->upper())
                .arg(var->steps());
        }
    }

    totalRunsLabel_->setText(detail);
}

// ============================================================================
// 表达式浏览
// ============================================================================

Expr* UiFeasibleRegionStudy::browseExpression(QWidget* parent)
{
    return UiExpressionBrowser::GetExpression(parent);
}

// ============================================================================
// 槽 — 变量
// ============================================================================

void UiFeasibleRegionStudy::onAddVariable()
{
    auto* study = getStudy();
    if (!study)
        return;

    auto* var = new FeasibleRegionVariable();
    var->setName("var_" + std::to_string(study->variables().size() + 1));
    var->setLower(0.0);
    var->setUpper(1.0);
    var->setSteps(10);

    study->addVariable(var);
    refreshFromStudy();

    // 选中新行并进入名称编辑
    int newRow = static_cast<int>(study->variables().size()) - 1;
    varTable_->selectRow(newRow);
    varTable_->editItem(varTable_->item(newRow, VAR_NAME));
}

void UiFeasibleRegionStudy::onRemoveVariable()
{
    int row = varTable_->currentRow();
    auto* study = getStudy();
    if (row < 0 || !study)
        return;

    auto& vars = study->variables();
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

    study->removeVariable(var);
    refreshFromStudy();
}

void UiFeasibleRegionStudy::onVariableCellChanged(int row, int col)
{
    auto* study = getStudy();
    if (!study)
        return;

    auto& vars = study->variables();
    if (row < 0 || row >= static_cast<int>(vars.size()))
        return;

    auto* var = vars[row].get();
    if (!var)
        return;

    auto* item = varTable_->item(row, col);
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
    case VAR_LOWER:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            var->setLower(val);
        break;
    }
    case VAR_UPPER:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            var->setUpper(val);
        break;
    }
    case VAR_STEPS:
    {
        int val = item->data(Qt::EditRole).toInt(&ok);
        if (ok && val > 0)
            var->setSteps(val);
        break;
    }
    default:
        break;
    }

    if (col == VAR_STEPS)
        updateTotalRunsLabel();
}

void UiFeasibleRegionStudy::onVariableExprClicked(int row)
{
    auto* study = getStudy();
    if (!study)
        return;

    auto& vars = study->variables();
    if (row < 0 || row >= static_cast<int>(vars.size()))
        return;

    auto* var = vars[row].get();
    if (!var)
        return;

    Expr* expr = browseExpression(this);
    if (expr)
    {
        var->setExpr(expr);
        // 更新按钮文本
        auto* btn = qobject_cast<QPushButton*>(varTable_->cellWidget(row, VAR_EXPR));
        if (btn)
            btn->setText(QString::fromStdString(expr->getExpression()));
    }
}

// ============================================================================
// 槽 — 约束
// ============================================================================

void UiFeasibleRegionStudy::onAddConstraint()
{
    auto* study = getStudy();
    if (!study)
        return;

    auto* ctr = new FeasibleRegionConstraint();
    ctr->setName("ctr_" + std::to_string(study->constraints().size() + 1));
    ctr->setUseLower(false);
    ctr->setUseUpper(false);
    ctr->setExclude(false);
    ctr->setLower(0.0);
    ctr->setUpper(1.0);

    study->addConstraint(ctr);
    refreshFromStudy();

    // 选中新行并进入名称编辑
    int newRow = static_cast<int>(study->constraints().size()) - 1;
    ctrTable_->selectRow(newRow);
    ctrTable_->editItem(ctrTable_->item(newRow, CTR_NAME));
}

void UiFeasibleRegionStudy::onRemoveConstraint()
{
    int row = ctrTable_->currentRow();
    auto* study = getStudy();
    if (row < 0 || !study)
        return;

    auto& ctrs = study->constraints();
    if (row >= static_cast<int>(ctrs.size()))
        return;

    auto* ctr = ctrs[row].get();
    if (!ctr)
        return;

    int ret = QMessageBox::question(
        this, tr("删除约束"),
        tr("确认删除约束 \"%1\" 吗？").arg(QString::fromStdString(ctr->getName())),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret != QMessageBox::Yes)
        return;

    study->removeConstraint(ctr);
    refreshFromStudy();
}

void UiFeasibleRegionStudy::onConstraintCellChanged(int row, int col)
{
    auto* study = getStudy();
    if (!study)
        return;

    auto& ctrs = study->constraints();
    if (row < 0 || row >= static_cast<int>(ctrs.size()))
        return;

    auto* ctr = ctrs[row].get();
    if (!ctr)
        return;

    auto* item = ctrTable_->item(row, col);
    if (!item)
        return;

    bool ok = false;
    switch (col)
    {
    case CTR_NAME:
    {
        QString text = item->text().trimmed();
        if (!text.isEmpty())
            ctr->setName(text.toStdString());
        break;
    }
    case CTR_LOWER:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            ctr->setLower(val);
        break;
    }
    case CTR_UPPER:
    {
        double val = item->data(Qt::EditRole).toDouble(&ok);
        if (ok)
            ctr->setUpper(val);
        break;
    }
    default:
        break;
    }
}

void UiFeasibleRegionStudy::onConstraintExprClicked(int row)
{
    auto* study = getStudy();
    if (!study)
        return;

    auto& ctrs = study->constraints();
    if (row < 0 || row >= static_cast<int>(ctrs.size()))
        return;

    auto* ctr = ctrs[row].get();
    if (!ctr)
        return;

    Expr* expr = browseExpression(this);
    if (expr)
    {
        ctr->setExpr(expr);
        auto* btn = qobject_cast<QPushButton*>(ctrTable_->cellWidget(row, CTR_EXPR));
        if (btn)
            btn->setText(QString::fromStdString(expr->getExpression()));
    }
}

AST_NAMESPACE_END
