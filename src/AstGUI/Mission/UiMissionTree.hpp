///
/// @file      UiMissionTree.hpp
/// @brief     任务树控件，显示 Mission Control Sequence 层级
/// @details   基于 QTreeWidget，支持拖拽排序、右键菜单、节点类型图标
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include "AstCore/MissionCommand.hpp"
#include <QTreeWidget>

AST_NAMESPACE_BEGIN

class Sequence;
class MissionModerator;

/// @brief 任务树控件，用于显示和编辑任务序列层级
class AST_GUI_API UiMissionTree : public QTreeWidget
{
    Q_OBJECT

public:
    /// @brief 自定义 data role，存储 HMissionCommand (SharedPtr<MissionCommand>) 的裸指针
    ///        配合 MissionTreeItem 使用，数据所有权由树节点持有
    static constexpr int kCommandRole = Qt::UserRole + 1;

    explicit UiMissionTree(QWidget* parent = nullptr);
    ~UiMissionTree() override = default;

    void setModerator(MissionModerator* moderator);

    MissionCommand* selectedCommand() const;

    /// @brief 删除当前选中的命令（树节点 + 同步内核）
    void removeSelectedCommand();

signals:
    void segmentSelected(MissionCommand* command);
    void treeModified();

protected:
    void dropEvent(QDropEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    QTreeWidgetItem* buildTreeItem(HMissionCommand cmd, QTreeWidgetItem* parentItem);
    void syncSequenceOrder(QTreeWidgetItem* parentItem);
    void syncAllSequences(QTreeWidgetItem* root);
    void updateItemDisplay(QTreeWidgetItem* item, MissionCommand* cmd);

    MissionModerator* moderator_ = nullptr;
};

AST_NAMESPACE_END
