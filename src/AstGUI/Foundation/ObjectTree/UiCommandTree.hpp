///
/// @file      UiCommandTree.hpp
/// @brief     任务命令树控件，显示和编辑任务命令序列层级
/// @author    axel
/// @date      2026-06-03
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

#include "UiObjectTree.hpp"
#include "AstCore/MissionCommand.hpp"
#include "AstCore/MainSequence.hpp"

AST_NAMESPACE_BEGIN

class MainSequence;
class Segment;

/// @brief 任务段树控件，继承 UiObjectTree，支持拖拽排序、右键菜单、序列同步
class AST_GUI_API UiCommandTree : public UiObjectTree
{
    Q_OBJECT

public:
    explicit UiCommandTree(QWidget* parent = nullptr);

    /// @brief 设置主序列并刷新树，设为 nullptr 则清空
    void setSequence(MainSequence* sequence);

    /// @brief 获取树中当前选中的任务命令，无选中时返回 nullptr
    Command* selectedCommand() const;

    /// @brief 删除当前选中的命令（树节点 + 同步内核）
    void removeSelectedCommand();

signals:
    /// @brief 当用户在树中选中任务命令时发出
    void commandSelected(Command* command);

    /// @brief 当树结构被拖拽或删除修改后发出
    void treeModified();

protected:
    void dropEvent(QDropEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    WeakPtr<MainSequence> sequence_ = nullptr;
};

AST_NAMESPACE_END
