///
/// @file      UiAttributeTree.cpp
/// @brief     属性树控件实现
/// @author    axel
/// @date      2026-06-04
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

#include "UiAttributeTree.hpp"
#include "UiAttributeTreeItem.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Property.hpp"
#include <QHeaderView>
#include <QStyleFactory>
#include <algorithm>

AST_NAMESPACE_BEGIN

UiAttributeTree::UiAttributeTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setColumnCount(2);
    setHeaderLabels({tr("属性"), tr("值")});
    setSelectionMode(QAbstractItemView::SingleSelection);

    // 允许列宽自由调整
    header()->setStretchLastSection(true);
    header()->setSectionResizeMode(QHeaderView::Interactive);

    // 第一列给一个较宽的默认宽度，避免初始显示不全
    setColumnWidth(0, 160);

    connect(this, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        if (auto* item = static_cast<UiAttributeTreeItem*>(current))
            emit attributeSelected(item->attribute());
        else
            emit attributeSelected(Attribute());
    });

    setStyle(QStyleFactory::create("windows"));
}

void UiAttributeTree::setObject(Object* object)
{
    if (rootObject_.get() == object)
        return;

    rootObject_ = object;
    refresh();
}

Object* UiAttributeTree::object() const
{
    return rootObject_.get();
}

void UiAttributeTree::refresh()
{
    clear();

    auto* obj = rootObject_.get();
    if (!obj)
        return;

    QList<QTreeWidgetItem*> items = UiAttributeTreeItem::CreateTreeItems(obj);
    invisibleRootItem()->addChildren(items);
    expandAll();
    // 自动调整第一列宽度，使属性名称完整显示
    resizeColumnToContents(0);
}

Attribute UiAttributeTree::selectedAttribute() const
{
    auto* item = dynamic_cast<UiAttributeTreeItem*>(currentItem());
    return item ? item->attribute() : Attribute();
}

void UiAttributeTree::collectProperties(Class* type, std::vector<Property*>& out) const
{
    if (!type)
        return;
    type->getAllProperties(out, true);
}

AST_NAMESPACE_END
