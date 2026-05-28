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

UiObjectTree::UiObjectTree(Object* root, QWidget* parent)
    : UiObjectTree(parent)
{
    rootObject_ = root;
}

void UiObjectTree::setRootObject(Object* root)
{
    rootObject_ = root;
}

Object* UiObjectTree::rootObject() const
{
    return rootObject_.get();
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

    auto& mgr = ObjectManager::CurrentInstance();

    if (rootObject_)
    {
        auto* rootNode = mgr.getObjectNode(rootObject_.get());
        if (!rootNode)
            return;

        if (rootVisible_)
        {
            auto* rootItem = static_cast<UiObjectTreeItem*>(buildItem(rootObject_.get()));
            invisibleRootItem()->addChild(rootItem);
            rootItem->buildChildren();
        }
        else
        {
            for (auto* childNode : rootNode->getChildren())
            {
                if (auto* childObj = childNode->getObject())
                {
                    auto* item = static_cast<UiObjectTreeItem*>(buildItem(childObj));
                    invisibleRootItem()->addChild(item);
                    item->buildChildren();
                }
            }
        }
    }
    else
    {
        auto allObjects = mgr.getAllObjects();

        std::unordered_map<Object*, UiObjectTreeItem*> itemMap;

        for (auto* obj : allObjects)
        {
            if (!obj)
                continue;
            auto* item = static_cast<UiObjectTreeItem*>(buildItem(obj));
            itemMap[obj] = item;
        }

        for (auto* obj : allObjects)
        {
            if (!obj)
                continue;
            auto* node = mgr.getObjectNode(obj);
            if (!node)
                continue;

            auto* parentNode = node->getParentNode();
            if (parentNode)
            {
                auto* parentObj = parentNode->getObject();
                if (parentObj)
                {
                    auto it = itemMap.find(parentObj);
                    if (it != itemMap.end())
                    {
                        it->second->addChild(itemMap[obj]);
                        continue;
                    }
                }
            }
            invisibleRootItem()->addChild(itemMap[obj]);
        }
    }

    expandAll();
}

Object* UiObjectTree::selectedObject() const
{
    auto* item = static_cast<UiObjectTreeItem*>(currentItem());
    return item ? item->object() : nullptr;
}

QTreeWidgetItem* UiObjectTree::buildItem(Object* obj)
{
    auto* item = new UiObjectTreeItem();
    item->configure(obj, tr("<无名称>"));
    return item;
}

AST_NAMESPACE_END
