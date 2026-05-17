///
/// @file      UiMissionTree.cpp
/// @brief     任务树控件实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiMissionTree.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Segment.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstCore/MissionModerator.hpp"
#include <QDropEvent>
#include <QMenu>
#include <QAction>

AST_NAMESPACE_BEGIN

// ============================================================================
// MissionTreeItem — 直接持有 HMissionCommand (SharedPtr)
// ============================================================================

class MissionTreeItem : public QTreeWidgetItem
{
public:
    /// @brief 子 Segment 的 SharedPtr (来自父 Sequence::commands_)
    HMissionCommand command;

    /// @brief 根 Sequence 的裸指针 (MissionModerator 持有，不能 SharedPtr)
    MissionCommand* rootPtr = nullptr;

    explicit MissionTreeItem() : QTreeWidgetItem(QTreeWidgetItem::UserType) {}

    MissionCommand* ptr() const { return command ? command.get() : rootPtr; }

    bool isSequence() const
    {
        auto* p = ptr();
        return p && p->isOfType(Sequence::StaticType());
    }

    Sequence* toSequence() const { return aobject_cast<Sequence*>(ptr()); }
};

// ============================================================================
// UiMissionTree
// ============================================================================

UiMissionTree::UiMissionTree(QWidget* parent)
    : QTreeWidget(parent)
{
    setHeaderHidden(true);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, &QTreeWidget::currentItemChanged,
            this, [this](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        auto* cmd = current
            ? static_cast<MissionTreeItem*>(current)->ptr()
            : nullptr;
        emit segmentSelected(cmd);
    });
}

// ============================================================================
// 公共接口
// ============================================================================

void UiMissionTree::setModerator(MissionModerator* moderator)
{
    moderator_ = moderator;
    clear();

    if (!moderator)
        return;

    auto& seq = moderator->getSequence();

    // 根 Sequence 由 MissionModerator 持有，树节点存裸指针
    auto* rootItem = new MissionTreeItem();
    rootItem->rootPtr = &seq;
    updateItemDisplay(rootItem, &seq);
    invisibleRootItem()->addChild(rootItem);

    // 递归构建子节点 (子节点的 SharedPtr 由 Sequence::commands_ 持有，树节点复制一份)
    for (auto& childCmd : seq.getCommands())
    {
        buildTreeItem(childCmd, rootItem);
    }

    expandItem(rootItem);

    emit segmentSelected(&seq);
}

MissionCommand* UiMissionTree::selectedCommand() const
{
    auto* item = static_cast<MissionTreeItem*>(currentItem());
    return item ? item->ptr() : nullptr;
}

void UiMissionTree::removeSelectedCommand()
{
    auto* item = static_cast<MissionTreeItem*>(currentItem());
    if (!item)
        return;

    // 根 Sequence 不可删除
    if (!item->parent() && invisibleRootItem()->indexOfChild(item) == 0)
        return;

    delete item;
    syncAllSequences(invisibleRootItem());
    emit treeModified();
}

// ============================================================================
// 拖拽排序
// ============================================================================

void UiMissionTree::dropEvent(QDropEvent* event)
{
    QTreeWidget::dropEvent(event);
    syncAllSequences(invisibleRootItem());
    emit treeModified();
}

// ============================================================================
// 右键菜单
// ============================================================================

void UiMissionTree::contextMenuEvent(QContextMenuEvent* event)
{
    auto* item = static_cast<MissionTreeItem*>(itemAt(event->pos()));
    if (item)
        setCurrentItem(item);

    QMenu menu(this);

    QAction* addInitState  = menu.addAction(tr("Add Initial State"));
    QAction* addPropagate  = menu.addAction(tr("Add Propagate"));
    QAction* addManeuver   = menu.addAction(tr("Add Maneuver"));
    QAction* addSequence   = menu.addAction(tr("Add Sequence"));
    QAction* addTargetSeq  = menu.addAction(tr("Add Targeter Sequence"));
    menu.addSeparator();
    QAction* delAction = menu.addAction(tr("Delete"));
    delAction->setEnabled(item && item->ptr() != nullptr);

    // 根 Sequence 不可删除
    if (item && !item->parent() && invisibleRootItem()->indexOfChild(item) == 0)
        delAction->setEnabled(false);

    QAction* chosen = menu.exec(event->globalPos());
    if (!chosen)
        return;

    // TODO: 通过发射信号让 UiMissionPanel 处理段创建
    // 当前仅处理删除
    if (chosen == delAction && delAction->isEnabled())
    {
        delete item;
        syncSequenceOrder(invisibleRootItem());
        emit treeModified();
    }
}

// ============================================================================
// 内部辅助
// ============================================================================

QTreeWidgetItem* UiMissionTree::buildTreeItem(HMissionCommand cmd, QTreeWidgetItem* parentItem)
{
    if (!cmd)
        return nullptr;

    auto* item = new MissionTreeItem();
    item->command = cmd;
    updateItemDisplay(item, cmd.get());

    if (parentItem)
        parentItem->addChild(item);

    // 递归构建子序列
    if (auto* seq = aobject_cast<Sequence*>(cmd.get()))
    {
        for (auto& childCmd : seq->getCommands())
        {
            buildTreeItem(childCmd, item);
        }
        item->setExpanded(true);
    }

    return item;
}

void UiMissionTree::syncSequenceOrder(QTreeWidgetItem* parentItem)
{
    // 找到 parentItem 对应的 Sequence
    Sequence* parentSeq = nullptr;

    if (!parentItem || parentItem == invisibleRootItem())
    {
        // 从根 Sequence 同步
        if (invisibleRootItem()->childCount() > 0)
        {
            parentSeq = static_cast<MissionTreeItem*>(invisibleRootItem()->child(0))->toSequence();
        }
    }
    else
    {
        parentSeq = static_cast<MissionTreeItem*>(parentItem)->toSequence();
    }

    if (!parentSeq)
        return;

    // 按树中顺序重建 commands_ 向量
    std::vector<HMissionCommand> newOrder;
    for (int i = 0; i < parentItem->childCount(); ++i)
    {
        auto* childItem = static_cast<MissionTreeItem*>(parentItem->child(i));
        if (childItem->command)
        {
            newOrder.push_back(childItem->command);
        }
    }
    parentSeq->setCommands(std::move(newOrder));
}

void UiMissionTree::syncAllSequences(QTreeWidgetItem* root)
{
    syncSequenceOrder(root);
    for (int i = 0; i < root->childCount(); ++i)
    {
        auto* child = root->child(i);
        if (static_cast<MissionTreeItem*>(child)->isSequence())
            syncAllSequences(child);
    }
}

void UiMissionTree::updateItemDisplay(QTreeWidgetItem* item, MissionCommand* cmd)
{
    if (!item || !cmd)
        return;

    QString name = QString::fromStdString(cmd->getName());

    QString prefix;
    if (cmd->isOfType(TargeterSequence::StaticType()))
        prefix = QStringLiteral("[T] ");
    else if (cmd->isOfType(Sequence::StaticType()))
        prefix = QStringLiteral("[S] ");
    else if (cmd->isOfType(InitialState::StaticType()))
        prefix = QStringLiteral("[I] ");
    else if (cmd->isOfType(Maneuver::StaticType()))
        prefix = QStringLiteral("[M] ");
    else if (cmd->isOfType(Propagate::StaticType()))
        prefix = QStringLiteral("[P] ");
    else if (cmd->isOfType(Segment::StaticType()))
        prefix = QStringLiteral("[Seg] ");
    else
        prefix = QStringLiteral("[?] ");

    item->setText(0, prefix + name);
}

AST_NAMESPACE_END
