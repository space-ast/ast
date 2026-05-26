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
#include "AstGUI/UiObjectTree.hpp"
#include "AstUtil/ObjectCalculation.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/Property.hpp"
#include "AstUtil/Struct.hpp"

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

enum { ExpressionRole = Qt::UserRole + 1 };

QString objectExpressionPrefix(Object* object)
{
    if (!object)
        return {};
    return QString::fromStdString(object->getName());
}

} // anonymous namespace

UiExpressionBrowser::UiExpressionBrowser(QWidget* parent)
    : QDialog(parent)
{
    setupUi();
    objectTree_->refresh();
    if (objectTree_->topLevelItemCount() > 0)
        objectTree_->setCurrentItem(objectTree_->topLevelItem(0));
}

QString UiExpressionBrowser::getExpression(QWidget* parent)
{
    if (ObjectManager::CurrentInstance().getAllObjects().empty())
    {
        QMessageBox::information(parent, QObject::tr("Browse Expression"),
                                 QObject::tr("No objects available."));
        return {};
    }

    UiExpressionBrowser dlg(parent);
    if (dlg.exec() != QDialog::Accepted)
        return {};
    return dlg.selectedExpression();
}

void UiExpressionBrowser::setupUi()
{
    setWindowTitle(tr("Select Expression"));
    resize(850, 720);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);

    auto* splitter = new QSplitter(Qt::Horizontal, this);

    auto* objectPanel = new QWidget(splitter);
    auto* objectLayout = new QVBoxLayout(objectPanel);
    objectLayout->setContentsMargins(0, 0, 0, 0);
    objectLayout->addWidget(new QLabel(tr("Objects"), objectPanel));
    objectTree_ = new UiObjectTree(objectPanel);
    objectTree_->setMinimumWidth(230);
    objectLayout->addWidget(objectTree_);
    splitter->addWidget(objectPanel);

    auto* expressionPanel = new QWidget(splitter);
    auto* expressionLayout = new QVBoxLayout(expressionPanel);
    expressionLayout->setContentsMargins(0, 0, 0, 0);

    auto* propertyGroup = new QGroupBox(tr("Property Variables"), expressionPanel);
    auto* propertyLayout = new QHBoxLayout(propertyGroup);
    propertyTree_ = new QTreeWidget(propertyGroup);
    propertyTree_->setHeaderHidden(true);
    propertyTree_->setSelectionMode(QAbstractItemView::SingleSelection);
    propertyTree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    propertyTree_->header()->setStretchLastSection(true);
    propertySelectButton_ = new QPushButton(tr("→"), propertyGroup);
    propertySelectButton_->setEnabled(false);
    propertySelectButton_->setFixedWidth(34);
    propertySelectButton_->setToolTip(tr("Select property expression"));
    propertyLayout->addWidget(propertyTree_);
    propertyLayout->addWidget(propertySelectButton_);
    expressionLayout->addWidget(propertyGroup, 1);

    auto* calculationGroup = new QGroupBox(tr("Object Calculation(对象计算量)"), expressionPanel);
    auto* calculationLayout = new QHBoxLayout(calculationGroup);
    calculationTree_ = new QTreeWidget(calculationGroup);
    calculationTree_->setHeaderHidden(true);
    calculationTree_->setSelectionMode(QAbstractItemView::SingleSelection);
    calculationTree_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    calculationTree_->header()->setStretchLastSection(true);
    calculationSelectButton_ = new QPushButton(tr("→"), calculationGroup);
    calculationSelectButton_->setEnabled(false);
    calculationSelectButton_->setFixedWidth(34);
    calculationSelectButton_->setToolTip(tr("Select object calculation expression"));
    calculationLayout->addWidget(calculationTree_);
    calculationLayout->addWidget(calculationSelectButton_);
    expressionLayout->addWidget(calculationGroup, 1);

    splitter->addWidget(expressionPanel);
    splitter->setSizes({260, 560});
    rootLayout->addWidget(splitter);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    rootLayout->addWidget(buttons);

    connect(objectTree_, &UiObjectTree::objectSelected,
            this, &UiExpressionBrowser::onObjectSelected);
    connect(propertyTree_, &QTreeWidget::itemSelectionChanged,
            this, &UiExpressionBrowser::onPropertySelectionChanged);
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
    refreshPropertyTree();
    refreshCalculationTree();
}

void UiExpressionBrowser::onPropertySelectionChanged()
{
    auto* item = propertyTree_->currentItem();
    propertySelectButton_->setEnabled(item && !item->data(0, ExpressionRole).toString().isEmpty());
}

void UiExpressionBrowser::onCalculationSelectionChanged()
{
    auto* item = calculationTree_->currentItem();
    calculationSelectButton_->setEnabled(item && !item->data(0, ExpressionRole).toString().isEmpty());
}

void UiExpressionBrowser::onPropertyAccepted()
{
    acceptExpression(propertyTree_->currentItem());
}

void UiExpressionBrowser::onCalculationAccepted()
{
    acceptExpression(calculationTree_->currentItem());
}

void UiExpressionBrowser::onItemDoubleClicked(QTreeWidgetItem* item, int)
{
    acceptExpression(item);
}

void UiExpressionBrowser::refreshPropertyTree()
{
    propertyTree_->clear();
    propertySelectButton_->setEnabled(false);

    auto prefix = objectExpressionPrefix(currentObject_);
    if (prefix.isEmpty())
        return;

    Class* cls = currentObject_->getType();
    if (!cls)
        return;

    const Struct::PropertyList& props = cls->getProperties();
    for (size_t i = 0; i < props.size(); ++i)
    {
        Property* prop = props[i];
        if (!prop)
            continue;
        auto name = QString::fromStdString(prop->name());
        addExpressionItem(propertyTree_, name, prefix + "." + name);
    }

    if (propertyTree_->topLevelItemCount() > 0)
        propertyTree_->setCurrentItem(propertyTree_->topLevelItem(0));
}

void UiExpressionBrowser::refreshCalculationTree()
{
    calculationTree_->clear();
    calculationSelectButton_->setEnabled(false);

    auto prefix = objectExpressionPrefix(currentObject_);
    if (prefix.isEmpty())
        return;

    auto allObjects = ObjectManager::CurrentInstance().getAllObjects();
    for (auto* object : allObjects)
    {
        auto* calc = dynamic_cast<ObjectCalculation*>(object);
        if (!calc || !calc->isExpectedType(currentObject_))
            continue;

        auto name = QString::fromStdString(calc->getName());
        if (name.isEmpty())
            continue;
        addExpressionItem(calculationTree_, name, prefix + "." + name + "()");
    }

    if (calculationTree_->topLevelItemCount() > 0)
        calculationTree_->setCurrentItem(calculationTree_->topLevelItem(0));
}

void UiExpressionBrowser::acceptExpression(QTreeWidgetItem* item)
{
    if (!item)
        return;

    auto expression = item->data(0, ExpressionRole).toString();
    if (expression.isEmpty())
        return;

    selectedExpression_ = expression;
    accept();
}

QTreeWidgetItem* UiExpressionBrowser::addExpressionItem(QTreeWidget* tree, const QString& name, const QString& expression)
{
    auto* item = new QTreeWidgetItem;
    item->setText(0, name);
    item->setData(0, ExpressionRole, expression);
    tree->addTopLevelItem(item);
    return item;
}

AST_NAMESPACE_END
