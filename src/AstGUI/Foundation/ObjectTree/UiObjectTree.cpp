///
/// @file      UiObjectTree.cpp
/// @brief     通用对象树控件实现
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

#include "UiObjectTree.hpp"
#include "UiObjectTreeItem.hpp"
#include "AstUtil/ObjectManager.hpp"
#include <QStyleFactory>
#include <unordered_map>

AST_NAMESPACE_BEGIN

UiObjectTree::UiObjectTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        auto* obj = current
            ? static_cast<UiObjectTreeItem*>(current)->object()
            : nullptr;
        emit objectSelected(obj);
    });
    // 显示节点间连线虚线和伸缩 +- 号
    this->setStyle(QStyleFactory::create("windows"));
}


UiObjectTree::~UiObjectTree()
{
    if (rootItem_)
    {
        delete rootItem_;
        rootItem_ = nullptr;
    }
}

void UiObjectTree::setRootItem(UiObjectTreeItem* item)
{
    if (rootItem_ && rootItem_ != item)
        delete rootItem_;
    rootItem_ = item;
}

UiObjectTreeItem* UiObjectTree::rootItem() const
{
    return rootItem_;
}

void UiObjectTree::setRootVisible(bool visible)
{
    rootVisible_ = visible;
}

bool UiObjectTree::isRootVisible() const
{
    return rootVisible_;
}

/// 有根节点时从该节点出发递归构建子树，否则展示 ObjectManager 中全部对象
void UiObjectTree::refresh()
{
    clear();

    if (rootItem_)
    {
        // 若 rootItem_ 当前在树中，先从树中移出避免被 clear() 销毁
        if (rootItem_->treeWidget())
        {
            auto* p = rootItem_->parent() ? rootItem_->parent() : invisibleRootItem();
            int idx = p->indexOfChild(rootItem_);
            if (idx >= 0)
                p->takeChild(idx);
        }
        if (rootVisible_)
        {
            auto* item = rootItem_->clone();
            invisibleRootItem()->addChild(item);
            item->buildChildren();
        }
        else
        {
            for (auto* child : rootItem_->createChildItems())
            {
                invisibleRootItem()->addChild(child);
                child->buildChildren();
            }
        }
    }
    else
    {
        auto& mgr = ObjectManager::CurrentInstance();
        auto allObjects = mgr.getRootObjects();

        for (auto* obj : allObjects)
        {
            if (!obj)
                continue;
            auto* item = new UiObjectTreeItem(obj);
            invisibleRootItem()->addChild(item);
            item->buildChildren();
        }
    }

    expandAll();
}

Object* UiObjectTree::selectedObject() const
{
    auto* item = static_cast<UiObjectTreeItem*>(currentItem());
    return item ? item->object() : nullptr;
}

AST_NAMESPACE_END
