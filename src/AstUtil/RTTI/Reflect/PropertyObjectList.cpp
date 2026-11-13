///
/// @file      PropertyObjectList.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-11-05
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

#include "PropertyObjectList.hpp"
#include "AstUtil/PropertyVisitor.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

namespace
{
/// @brief 检查待设置的列表元素
/// @param elementClass 元素类型，为空时不检查
/// @param item 待设置的元素
/// @return errc_t 错误码
errc_t checkItem(Class* elementClass, Object* item)
{
    if(!item)
    {
        aError(_("输入的对象为空"));
        return eErrorNullInput;
    }
    if(elementClass && !item->isOfType(elementClass))
    {
        aError(_("输入的对象类型无效，期望 '%s'，给定 '%s'"), elementClass->name().c_str(), item->getType()->name().c_str());
        return eErrorInvalidType;
    }
    return eNoError;
}

/// @brief 检查待设置的列表元素
/// @param elementClass 元素类型，为空时不检查
/// @param items 待设置的元素列表
/// @return errc_t 错误码
errc_t checkItems(Class* elementClass, Span<Object*> items)
{
    for(Object* item: items)
    {
        errc_t rc = checkItem(elementClass, item);
        if(rc != eNoError)
            return rc;
    }
    return eNoError;
}


} // namespace


size_t aFakeGetSize(const void* /*container*/)
{
    aError(_("对象列表属性没有设置大小获取函数"));
    return 0;
}


errc_t aFakeGetItemByIndex(const void* /*container*/, size_t /*index*/, Object*& value)
{
    aError(_("对象列表属性没有设置元素获取函数"));
    value = nullptr;
    return eErrorNotImplemented;
}

errc_t aFakeReplaceItems(void* /*container*/, size_t /*pos*/, size_t /*count*/, Object** /*items*/, size_t /*size*/)
{
    aError(_("对象列表属性没有设置元素替换函数"));
    return eErrorNotImplemented;
}

PropertyObjectList::PropertyObjectList(FPropertyGetSize getSize, FPropertyGetItemByIndex getItemByIndex, FPropertyReplaceItems replaceItems, Class* elementClass)
    : Property()
    , getSize_(getSize?getSize:&aFakeGetSize)
    , getItemByIndex_(getItemByIndex?getItemByIndex:&aFakeGetItemByIndex)
    , replaceItems_(replaceItems?replaceItems:&aFakeReplaceItems)
    , elementClass_(elementClass?elementClass:Object::StaticType())
{}

errc_t PropertyObjectList::accept(PropertyVisitor &visitor, const void *container)
{
    return visitor.visit(*this, container);
}

errc_t PropertyObjectList::getValueDouble(const void* /*container*/, double& /*value*/)
{
    aError(_("无法从对象列表类型的属性获取 double 值"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::setValueDouble(void* /*container*/, double /*value*/)
{
    aError(_("无法将 double 值设置到对象列表类型的属性中"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::getValueInt(const void* /*container*/, int& /*value*/)
{
    aError(_("无法从对象列表类型的属性获取 int 值"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::setValueInt(void* /*container*/, int /*value*/)
{
    aError(_("无法将 int 值设置到对象列表类型的属性中"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::getValueBool(const void* /*container*/, bool& /*value*/)
{
    aError(_("无法从对象列表类型的属性获取 bool 值"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::setValueBool(void* /*container*/, bool /*value*/)
{
    aError(_("无法将 bool 值设置到对象列表类型的属性中"));
    return eErrorInvalidType;
}

errc_t PropertyObjectList::getValueString(const void* container, std::string& value)
{
    std::vector<Object*> items;
    errc_t rc = getItems(container, items);
    if(rc != eNoError)
        return rc;
    value.clear();
    for(size_t i = 0; i < items.size(); ++i)
    {
        if(i != 0)
            value += ", ";
        if(items[i])
            value += items[i]->getRepresentation();
        else
            value += "null";
    }
    return eNoError;
}

errc_t PropertyObjectList::setValueString(void* /*container*/, StringView /*value*/)
{
    aError(_("无法将 string 值设置到对象列表类型的属性中"));
    return eErrorInvalidType;
}

EValueType PropertyObjectList::getValueType() const
{
    return EValueType::eObjectList;
}

size_t PropertyObjectList::getSize(const void* container)
{
    return getSize_(container);
}

errc_t PropertyObjectList::getItem(const void* container, size_t index, Object*& value)
{
    return getItemByIndex_(container, index, value);
}

errc_t PropertyObjectList::getItems(const void* container, std::vector<Object*>& items)
{
    items.clear();
    size_t size = getSize_(container);
    items.reserve(size);
    for(size_t i = 0; i < size; ++i)
    {
        Object* item = nullptr;
        errc_t rc = getItemByIndex_(container, i, item);
        if(rc != eNoError)
        {
            items.clear();
            return rc;
        }
        items.push_back(item);
    }
    return eNoError;
}

errc_t PropertyObjectList::setItem(void* container, size_t index, Object* item)
{
    errc_t rc = checkItem(elementClass_, item);
    if(rc != eNoError)
        return rc;
    size_t size = getSize_(container);
    if(index >= size)
    {
        aError(_("索引 %zu 超出对象列表范围，大小为 %zu"), index, size);
        return eErrorOutOfRange;
    }
    return replaceItems_(container, index, 1, &item, 1);
}

errc_t PropertyObjectList::setItems(void* container, Span<Object*> items)
{
    errc_t rc = checkItems(elementClass_, items);
    if(rc != eNoError)
        return rc;
    return replaceItems_(container, 0, getSize_(container), items.data(), items.size());
}

errc_t PropertyObjectList::insertItem(void* container, size_t index, Object* item)
{
    errc_t rc = checkItem(elementClass_, item);
    if(rc != eNoError)
        return rc;
    size_t size = getSize_(container);
    if(index > size)
    {
        aError(_("索引 %zu 超出对象列表范围，大小为 %zu"), index, size);
        return eErrorOutOfRange;
    }
    return replaceItems_(container, index, 0, &item, 1);
}

errc_t PropertyObjectList::appendItem(void* container, Object* item)
{
    return insertItem(container, getSize_(container), item);
}

errc_t PropertyObjectList::removeItem(void* container, size_t index)
{
    size_t size = getSize_(container);
    if(index >= size)
    {
        aError(_("索引 %zu 超出对象列表范围，大小为 %zu"), index, size);
        return eErrorOutOfRange;
    }
    return replaceItems_(container, index, 1, nullptr, 0);
}

errc_t PropertyObjectList::clearItems(void* container)
{
    return replaceItems_(container, 0, getSize_(container), nullptr, 0);
}


AST_NAMESPACE_END
