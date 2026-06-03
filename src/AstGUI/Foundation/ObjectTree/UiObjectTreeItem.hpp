///
/// @file      UiObjectTreeItem.hpp
/// @brief     对象树控件项，关联一个 Object 并可递归构建子节点
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include <QTreeWidget>
#include <vector>

AST_NAMESPACE_BEGIN

class AST_GUI_API UiObjectTreeItem : public QTreeWidgetItem
{
public:
    UiObjectTreeItem();

    explicit UiObjectTreeItem(Object* obj);

    /// 递归构建子节点
    void buildChildren();

    /// 获取关联的对象
    Object* object() const { return object_.get(); }

    UiObjectTreeItem* clone() const override;

    /// 创建子节点列表，子类可重写以实现自定义的层级关系和 item 类型
    virtual QList<UiObjectTreeItem*> createChildItems() const;
private:
    /// 配置 item 的基本属性（关联对象、文本、图标、tooltip）
    void configure(Object* obj);
private:
    WeakPtr<Object> object_ = nullptr;
};

AST_NAMESPACE_END
