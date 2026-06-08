///
/// @file      UiExpressionBrowser.cpp
/// @brief     Expr 表达式选择对话框实现
/// @details   用于选择对象属性表达式和对象计算量表达式
/// @author    axel
/// @date      2026-05-25
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

#include "UiExpressionBrowser.hpp"
#include "AstGUI/UiAttributeTree.hpp"
#include "AstGUI/UiAttributeTreeItem.hpp"
#include "AstGUI/UiObjectTree.hpp"
#include "AstScript/ExprAttribute.hpp"
#include "AstScript/ExprCalculation.hpp"
#include "AstUtil/ObjectCalculation.hpp"
#include "AstUtil/ObjectManager.hpp"

#include <QAbstractItemView>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>

AST_NAMESPACE_BEGIN

namespace {

enum { 
    ObjectRole = Qt::UserRole + 1, 
    CalcRole
};

} // anonymous namespace

UiExpressionBrowser::UiExpressionBrowser(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    objectTree_->refresh();
    if (objectTree_->topLevelItemCount() > 0)
        objectTree_->setCurrentItem(objectTree_->topLevelItem(0));
    // @todo 选中当前对象
}

Expr* UiExpressionBrowser::GetExpression(QWidget* parent)
{
    if (ObjectManager::CurrentInstance().getAllObjects().empty())
    {
        QMessageBox::information(parent, QObject::tr("选择表达式"),
                                 QObject::tr("暂无对象"));
        return nullptr;
    }

    QDialog dlg(parent);
    dlg.setWindowTitle(QObject::tr("选择表达式"));
    dlg.resize(850, 720);

    auto* layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* browser = new UiExpressionBrowser(&dlg);
    layout->addWidget(browser);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, &dlg);
    layout->addWidget(buttons);

    QObject::connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    QObject::connect(browser, &UiExpressionBrowser::propertyExpressionSelected,
                     &dlg, &QDialog::accept);
    QObject::connect(browser, &UiExpressionBrowser::calculationExpressionSelected,
                     &dlg, &QDialog::accept);

    if (dlg.exec() != QDialog::Accepted)
        return nullptr;

    return browser->selectedExpr_.take();
}

void UiExpressionBrowser::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);

    auto* splitter = new QSplitter(Qt::Horizontal, this);

    auto* objectPanel = new QWidget(splitter);
    auto* objectLayout = new QVBoxLayout(objectPanel);
    objectLayout->setContentsMargins(0, 0, 0, 0);
    objectLayout->addWidget(new QLabel(tr("对象"), objectPanel));
    objectTree_ = new UiObjectTree(objectPanel);
    objectTree_->setMinimumWidth(230);
    objectLayout->addWidget(objectTree_);
    splitter->addWidget(objectPanel);

    auto* expressionPanel = new QWidget(splitter);
    auto* expressionLayout = new QVBoxLayout(expressionPanel);
    expressionLayout->setContentsMargins(0, 0, 0, 0);

    auto* expSplitter = new QSplitter(Qt::Vertical, expressionPanel);

    auto* propertyGroup = new QGroupBox(tr("对象属性"), expSplitter);
    auto* propertyLayout = new QHBoxLayout(propertyGroup);
    propertyTree_ = new UiAttributeTree(propertyGroup);
    propertySelectButton_ = new QPushButton(("→"), propertyGroup);
    propertySelectButton_->setEnabled(false);
    propertySelectButton_->setFixedWidth(34);
    propertySelectButton_->setToolTip(tr("选择对象属性"));
    propertyLayout->addWidget(propertyTree_);
    propertyLayout->addWidget(propertySelectButton_);

    auto* calculationGroup = new QGroupBox(tr("对象计算量"), expSplitter);
    auto* calculationLayout = new QHBoxLayout(calculationGroup);
    calculationTree_ = new QTreeWidget(calculationGroup);
    calculationTree_->setHeaderHidden(true);
    calculationTree_->setSelectionMode(QAbstractItemView::SingleSelection);
    calculationTree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    calculationTree_->header()->setStretchLastSection(true);
    calculationSelectButton_ = new QPushButton(("→"), calculationGroup);
    calculationSelectButton_->setEnabled(false);
    calculationSelectButton_->setFixedWidth(34);
    calculationSelectButton_->setToolTip(tr("选择对象计算量"));
    calculationLayout->addWidget(calculationTree_);
    calculationLayout->addWidget(calculationSelectButton_);

    expSplitter->addWidget(propertyGroup);
    expSplitter->addWidget(calculationGroup);
    expSplitter->setSizes({300, 300});

    expressionLayout->addWidget(expSplitter);

    splitter->addWidget(expressionPanel);
    splitter->setSizes({260, 560});
    rootLayout->addWidget(splitter);

    connect(objectTree_, &UiObjectTree::objectSelected,
            this, &UiExpressionBrowser::onObjectSelected);
    connect(propertyTree_, &UiAttributeTree::attributeSelected,
            this, &UiExpressionBrowser::onPropertySelected);
    connect(calculationTree_, &QTreeWidget::itemSelectionChanged,
            this, &UiExpressionBrowser::onCalculationSelectionChanged);
    connect(propertySelectButton_, &QPushButton::clicked,
            this, &UiExpressionBrowser::onPropertyAccepted);
    connect(calculationSelectButton_, &QPushButton::clicked,
            this, &UiExpressionBrowser::onCalculationAccepted);
    connect(propertyTree_, &QTreeWidget::itemDoubleClicked,
            this, &UiExpressionBrowser::onItemDoubleClicked);
    connect(calculationTree_, &QTreeWidget::itemDoubleClicked,
            this, &UiExpressionBrowser::onItemDoubleClicked);
}

void UiExpressionBrowser::onObjectSelected(Object* object)
{
    currentObject_ = object;
    propertyTree_->setObject(object);
    refreshCalculationTree();
}

void UiExpressionBrowser::onPropertySelected(const Attribute& attr)
{
    propertySelectButton_->setEnabled(attr.isValid());
}

void UiExpressionBrowser::onPropertyAccepted()
{
    auto* item = static_cast<UiAttributeTreeItem*>(propertyTree_->currentItem());
    if (!item || !item->attribute().isValid())
        return;

    selectedExpr_ = ExprAttribute::New(item->attribute());
    emit propertyExpressionSelected(selectedExpr_.get());
}

void UiExpressionBrowser::onCalculationSelectionChanged()
{
    calculationSelectButton_->setEnabled(calculationTree_->currentItem() != nullptr);
}

void UiExpressionBrowser::onCalculationAccepted()
{
    auto* item = calculationTree_->currentItem();
    if (!item)
        return;

    auto* obj = reinterpret_cast<Object*>(
        item->data(0, ObjectRole).value<quintptr>());
    auto* calc = reinterpret_cast<ObjectCalculation*>(
        item->data(0, CalcRole).value<quintptr>());

    if (!obj || !calc)
        return;

    selectedExpr_ = ExprCalculation::New(obj, calc);
    emit calculationExpressionSelected(selectedExpr_.get());
}

void UiExpressionBrowser::onItemDoubleClicked(QTreeWidgetItem* item, int)
{
    if (!item)
        return;

    if (item->treeWidget() == propertyTree_)
        onPropertyAccepted();
    else if (item->treeWidget() == calculationTree_)
        onCalculationAccepted();
}

void UiExpressionBrowser::refreshCalculationTree()
{
    calculationTree_->clear();
    calculationSelectButton_->setEnabled(false);

    if (!currentObject_)
        return;

    auto allObjects = ObjectManager::CurrentInstance().getAllObjects();
    for (auto* object : allObjects)
    {
        auto* calc = dynamic_cast<ObjectCalculation*>(object);
        if (!calc || !calc->isExpectedType(currentObject_.get()))
            continue;

        auto name = QString::fromStdString(calc->getName());
        if (name.isEmpty())
            continue;

        auto* item = new QTreeWidgetItem;
        item->setText(0, name);
        item->setData(0, ObjectRole, QVariant::fromValue(reinterpret_cast<quintptr>(object)));
        item->setData(0, CalcRole, QVariant::fromValue(reinterpret_cast<quintptr>(calc)));
        calculationTree_->addTopLevelItem(item);
    }

    if (calculationTree_->topLevelItemCount() > 0)
        calculationTree_->setCurrentItem(calculationTree_->topLevelItem(0));
}

AST_NAMESPACE_END
