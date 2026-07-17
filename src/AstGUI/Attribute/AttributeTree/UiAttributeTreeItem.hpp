///
/// @file      UiAttributeTreeItem.hpp
/// @brief     属性树控件项，关联一个 Attribute 并显示属性名与当前值
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Attribute.hpp"
#include <QTreeWidgetItem>
#include <string>

AST_NAMESPACE_BEGIN

class Property;

/// @brief 属性树控件项，封装一个 Attribute（对象-属性绑定），显示属性名和当前值
/// @details 对于对象类型（eObject）的属性，可通过 buildChildren() 展开子属性节点
class AST_GUI_API UiAttributeTreeItem : public QTreeWidgetItem
{
public:
    UiAttributeTreeItem();

    /// @brief 用对象和属性构造树项，内部构造 Attribute 并缓存属性名和描述
    UiAttributeTreeItem(Object* obj, Property* prop);

    /// @brief 创建属性树项，递归处理对象类型的属性，返回树项列表
    /// @param object 要处理的对象，可能包含子对象
    /// @return 包含所有属性树项的列表
    static QList<QTreeWidgetItem*> CreateTreeItems(Object* object, int maxDepth = 20);

    /// @brief 获取内部的 Attribute 引用（可读写）
    Attribute& attribute() { return attr_; }

    /// @brief 获取内部的 Attribute 引用（只读）
    const Attribute& attribute() const { return attr_; }

    /// @brief 获取缓存的属性名
    const std::string& propertyName() const { return name_; }

    /// @brief 获取缓存的属性描述
    const std::string& propertyDesc() const { return desc_; }

    /// @brief 刷新显示文本，从 Attribute 读取当前值
    void refreshDisplay();

    /// @brief 递归构建子属性节点（仅对 eObject 类型属性有效）
    /// @param maxDepth 最大递归深度，防止循环引用导致无限递归
    void buildChildren(int maxDepth = 20);

    UiAttributeTreeItem* clone() const override;

private:
    Attribute   attr_{};   ///< 对象-属性绑定器
    std::string name_{};   ///< 属性名缓存（Property::name()）
    std::string desc_{};   ///< 属性描述缓存（Property::desc()）
};

AST_NAMESPACE_END
