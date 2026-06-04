///
/// @file      UiAttributeTreeItem.cpp
/// @brief     属性树控件项实现
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

#include "UiAttributeTreeItem.hpp"
#include "AstUtil/Class.hpp"
#include "AstUtil/Property.hpp"
#include "AstGUI/UiCommon.hpp"
#include <algorithm>
#include <vector>

AST_NAMESPACE_BEGIN

namespace {

const char* valueTypeName(EValueType type)
{
    switch (type)
    {
    case EValueType::eBool:      return "bool";
    case EValueType::eInt:       return "int";
    case EValueType::eDouble:    return "double";
    case EValueType::eQuantity:  return "quantity";
    case EValueType::eString:    return "string";
    case EValueType::eObject:    return "object";
    case EValueType::eTimePoint: return "time";
    default:                     return "?";
    }
}

/// @brief 收集指定 Class 的所有属性（遍历继承链，子类同名属性覆盖父类）
void collectClassProperties(Class* type, std::vector<Property*>& out)
{
    if (!type)
        return;

    type->getAllProperties(out, true);
}

} // anonymous namespace

UiAttributeTreeItem::UiAttributeTreeItem()
    : QTreeWidgetItem(QTreeWidgetItem::UserType)
{}

UiAttributeTreeItem::UiAttributeTreeItem(Object* obj, Property* prop)
    : QTreeWidgetItem(QTreeWidgetItem::UserType)
    , attr_(obj, prop)
    , name_(prop ? prop->name() : "")
    , desc_(prop ? prop->desc() : "")
{
    refreshDisplay();
}

QList<QTreeWidgetItem*> UiAttributeTreeItem::CreateTreeItems(Object* object, int maxDepth)
{
    if (!object)
        return {};
    auto type = object->getType();
    if (!type)
        return {};

    QList<QTreeWidgetItem*> items;
    std::vector<Property*> allProps;
    collectClassProperties(type, allProps);
    for (auto prop : allProps)
    {
        if(!prop)
            continue;
        auto item = new UiAttributeTreeItem(object, prop);
        item->buildChildren(maxDepth - 1);
        items.append(item);
    }

    return items;
}

void UiAttributeTreeItem::refreshDisplay()
{
    if (!attr_.isValid())
    {
        setText(0, QObject::tr("(无效属性)"));
        setText(1, QString());
        setToolTip(0, QString());
        return;
    }

    // 第 0 列：属性名
    setText(0, QString::fromStdString(name_));

    // 第 1 列：属性值
    QString valueStr;
    EValueType vt = attr_.getValueType();

    if (vt == EValueType::eObject)
    {
        Object* obj = nullptr;
        attr_.getValueObject(obj);
        valueStr = aUiObjectDisplayName(obj);
    }
    else
    {
        valueStr = QString::fromStdString(attr_.getValueString());
    }
    setText(1, valueStr);

    // Tooltip: 描述 + 类型
    QString tip;
    if (!desc_.empty())
        tip = QString::fromStdString(desc_) + QStringLiteral("\n");
    tip += QObject::tr("类型: %1").arg(QLatin1String(valueTypeName(vt)));
    setToolTip(0, tip);
    setToolTip(1, tip);
}

UiAttributeTreeItem* UiAttributeTreeItem::clone() const
{
    return new UiAttributeTreeItem(*this);
}

void UiAttributeTreeItem::buildChildren(int maxDepth)
{
    if (maxDepth <= 0)
        return;

    // 仅对象类型属性可以展开
    if (attr_.getValueType() != EValueType::eObject)
        return;

    // 获取子对象
    Object* childObj = nullptr;
    attr_.getValueObject(childObj);
    if (!childObj)
        return;

    // 检查循环引用：沿父级链查找是否已出现相同的子对象
    auto* ancestor = parent();
    while (ancestor)
    {
        auto* ancestorItem = dynamic_cast<UiAttributeTreeItem*>(ancestor);
        if (ancestorItem)
        {
            Object* ancestorObj = nullptr;
            ancestorItem->attribute().getValueObject(ancestorObj);
            if (ancestorObj == childObj)
                return;  // 检测到循环引用，停止展开
        }
        ancestor = ancestor->parent();
    }

    QList<QTreeWidgetItem*> childItems = CreateTreeItems(childObj, maxDepth - 1);
    this->addChildren(childItems);
}

AST_NAMESPACE_END
