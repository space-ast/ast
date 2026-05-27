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
#include "ObjectIcons.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/ObjectNode.hpp"
#include <unordered_map>

AST_NAMESPACE_BEGIN

class ObjectTreeItem : public QTreeWidgetItem
{
public:
    Object* object = nullptr;
    explicit ObjectTreeItem() : QTreeWidgetItem(QTreeWidgetItem::UserType) {}
};

UiObjectTree::UiObjectTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        auto* obj = current
            ? static_cast<ObjectTreeItem*>(current)->object
            : nullptr;
        emit objectSelected(obj);
    });
}

void UiObjectTree::refresh()
{
    clear();

    auto& mgr = ObjectManager::CurrentInstance();
    auto allObjects = mgr.getAllObjects();

    std::unordered_map<Object*, ObjectTreeItem*> itemMap;

    // 第一遍：为每个对象创建树节点
    for (auto* obj : allObjects)
    {
        if (!obj)
            continue;
        auto* item = new ObjectTreeItem();
        item->object = obj;
        auto name = obj->getName();
        item->setText(0, QString::fromStdString(name.empty() ? u8"<无名称>" : name));
        item->setToolTip(0, QString::fromStdString(obj->typeName()));
        item->setIcon(0, objectIcon(obj));
        itemMap[obj] = item;
    }

    // 第二遍：建立父子挂载关系
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
        // 无父节点或父对象已失效，挂到根下
        invisibleRootItem()->addChild(itemMap[obj]);
    }

    expandAll();
}

Object* UiObjectTree::selectedObject() const
{
    auto* item = static_cast<ObjectTreeItem*>(currentItem());
    return item ? item->object : nullptr;
}

AST_NAMESPACE_END
