///
/// @file      UiObjectTree.hpp
/// @brief     通用对象树控件，显示 ObjectManager 中所有对象的层级关系
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include "UiObjectTreeItem.hpp"
#include <QTreeWidget>

AST_NAMESPACE_BEGIN

/// @brief 通用对象树控件，支持显示全部对象或以指定节点为根的子树
class AST_GUI_API UiObjectTree : public QTreeWidget
{
    Q_OBJECT

public:
    /// @brief 构造全部对象树（显示 ObjectManager 中所有对象）
    explicit UiObjectTree(QWidget* parent = nullptr);

    ~UiObjectTree() override;

    UiObjectTree(const UiObjectTree&) = delete;
    UiObjectTree& operator=(const UiObjectTree&) = delete;

    /// @brief 重建对象树，根据当前根节点设置确定显示范围
    void refresh();

    /// @brief 获取树中当前选中的对象，无选中时返回 nullptr
    Object* selectedObject() const;

    /// @brief 设置根节点，设为 nullptr 则恢复显示全部对象（tree 接管所有权）
    void setRootItem(UiObjectTreeItem* item);

    /// @brief 设置根节点对象，设为 nullptr 则恢复显示全部对象
    void setRootObject(Object* object);

    /// @brief 获取当前根节点，未设置时返回 nullptr
    UiObjectTreeItem* rootItem() const;

    /// @brief 获取当前根节点对象，未设置时返回 nullptr
    Object* rootObject() const;

    /// @brief 设置根节点自身是否在树中可见，默认为 true
    void setRootVisible(bool visible);

    /// @brief 查询根节点是否可见
    bool isRootVisible() const;

    /// @brief 设置是否显示组件节点，默认为 true
    void setShowComponents(bool show);

    /// @brief 查询是否显示组件节点
    bool isShowComponents() const{return buildOptions_.showComponents;}

signals:
    /// @brief 当用户在树中选中对象时发出，参数仅在槽函数执行期间有效
    void objectSelected(Object* object);

    /// @brief 当用户双击树中对象时发出，参数仅在槽函数执行期间有效
    void objectDoubleClicked(Object* object);

protected:
    TreeBuildOptions buildOptions_{}; ///< 对象树构建选项

private:
    UiObjectTreeItem* rootItem_ = nullptr;
    bool rootVisible_ = true;       ///< 根节点是否可见，默认 true
};

AST_NAMESPACE_END
