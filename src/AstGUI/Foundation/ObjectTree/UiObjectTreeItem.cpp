///
/// @file      UiObjectTreeItem.cpp
/// @brief     对象树控件项实现
/// @author    axel
/// @date      2026-05-28
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

#include "UiObjectTreeItem.hpp"
#include "AstGUI/ObjectIcons.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/ObjectNode.hpp"

AST_NAMESPACE_BEGIN

UiObjectTreeItem::UiObjectTreeItem()
    : QTreeWidgetItem(QTreeWidgetItem::UserType)
{}

UiObjectTreeItem::UiObjectTreeItem(Object* obj)
    : UiObjectTreeItem()
{
    object_ = obj;
    configure(obj);
}

void UiObjectTreeItem::buildChildren()
{
    // 清除已有的子节点
    while (childCount() > 0)
        delete takeChild(0);
    for (auto* childItem : createChildItems())
    {
        addChild(childItem);
        // 递归构建子节点
        childItem->buildChildren();
    }
}

QList<UiObjectTreeItem*> UiObjectTreeItem::createChildItems() const
{
    auto* obj = object_.get();
    if (!obj)
        return {};

    auto* node = ObjectManager::CurrentInstance().getObjectNode(obj);
    if (!node)
        return {};
    
    QList<UiObjectTreeItem*> items;
    for (auto* childNode : node->getChildren())
    {
        if (auto* childObj = childNode->getObject())
            items.append(new UiObjectTreeItem(childObj));
    }
    return items;
}

UiObjectTreeItem* UiObjectTreeItem::clone() const
{
    return new UiObjectTreeItem(*this);
}

void UiObjectTreeItem::configure(Object* obj)
{
    object_ = obj;
    std::string name;
    std::string typeName;
    if (obj)
    {
        name = obj->getName();
        typeName = obj->typeName();
    }
    setText(0, name.empty() ? QObject::tr("未命名<%1>").arg(QString::fromStdString(typeName)) : QString::fromStdString(name));
    setToolTip(0, QString::fromStdString(typeName));
    setIcon(0, objectIcon(obj));
}

AST_NAMESPACE_END
