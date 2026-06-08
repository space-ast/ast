///
/// @file      UiCommandTree.cpp
/// @brief     任务段树控件实现
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

#include "UiCommandTree.hpp"
#include "UiCommandTreeItem.hpp"
#include "AstCore/MainSequence.hpp"
#include "AstCore/Sequence.hpp"
#include "AstGUI/MissionIcons.hpp"
#include "AstGUI/ObjectIcons.hpp"
#include <QDropEvent>
#include <QMenu>
#include <QAction>

AST_NAMESPACE_BEGIN

UiCommandTree::UiCommandTree(QWidget* parent)
    : UiObjectTree(parent)
{
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);

    connect(this, &UiObjectTree::objectSelected,
            this, [this](Object* obj)
    {
        emit commandSelected(aobject_cast<Command*>(obj));
    });
}

void UiCommandTree::setSequence(MainSequence* sequence)
{
    sequence_ = sequence;
    if (!sequence)
        return;

    auto* rootItem = new UiCommandTreeItem(sequence);
    this->setRootItem(rootItem);
    this->refresh();

    emit commandSelected(sequence);
}

Command* UiCommandTree::selectedCommand() const
{
    auto* obj = selectedObject();
    return obj ? aobject_cast<Command*>(obj) : nullptr;
}

void UiCommandTree::removeSelectedCommand()
{
    auto* item = dynamic_cast<UiCommandTreeItem*>(currentItem());
    if (!item)
        return;

    // 根节点不可删除
    if (!item->parent())
        return;

    item->removeCommandAndDeleteItem();
    item = nullptr;
    emit treeModified();
}

void UiCommandTree::dropEvent(QDropEvent* event)
{
    // 1. 拖拽前：保存被拖的 item
    auto* draggedItem = dynamic_cast<UiCommandTreeItem*>(currentItem());
    if (!draggedItem)
    {
        qWarning("draggedItem is nullptr");
        event->ignore();
        return;
    }
    auto* draggedCmd = draggedItem->command();
    if (!draggedCmd)
    {
        qWarning("draggedCmd is nullptr");
        event->ignore();
        return;
    }

    // 2. 落点信息（before move）
    auto* targetItem = dynamic_cast<UiCommandTreeItem*>(itemAt(event->pos()));
    DropIndicatorPosition dropPos = dropIndicatorPosition();  // AboveItem / BelowItem / OnItem / OnViewport
    A_UNUSED(targetItem);
    A_UNUSED(dropPos);

    // 3. 让 Qt 执行实际移动
    QTreeWidget::dropEvent(event);

    // 4. 移动后：draggedItem 已在新位置
    if (draggedItem->parent())
    {
        int newIndex = draggedItem->parent()->indexOfChild(draggedItem);
        auto* parentSeqItem = dynamic_cast<UiCommandTreeItem*>(draggedItem->parent());
        auto* parentSeq = parentSeqItem ? parentSeqItem->getObject<Sequence*>() : nullptr;
        if(!parentSeq)
        {
            qWarning("parentSeq is nullptr");
            event->ignore();
            return;
        }
        // newIndex = 插入到了父节点下第几个位置
        // parentSeq = 所属的 Sequence（需要重排 commands_ 的序列）
        parentSeq->insertCommand(newIndex, draggedCmd);
    }
    emit treeModified();
}

void UiCommandTree::contextMenuEvent(QContextMenuEvent* event)
{
    auto* item = dynamic_cast<UiCommandTreeItem*>(itemAt(event->pos()));
    if (item)
        setCurrentItem(item);

    QMenu menu(this);

    QAction* addInitState  = menu.addAction(aUiClassIcon("InitialState"),       tr("添加初始状态"));
    QAction* addPropagate  = menu.addAction(aUiClassIcon("Propagate"),          tr("添加预报段"));
    QAction* addManeuver   = menu.addAction(aUiClassIcon("Maneuver"),           tr("添加机动段"));
    QAction* addSequence   = menu.addAction(aUiClassIcon("Sequence"),           tr("添加序列段"));
    QAction* addTargetSeq  = menu.addAction(aUiClassIcon("TargeterSequence"),   tr("添加瞄准序列段"));
    A_UNUSED(addInitState);
    A_UNUSED(addPropagate);
    A_UNUSED(addManeuver);
    A_UNUSED(addSequence);
    A_UNUSED(addTargetSeq);

    menu.addSeparator();

    QAction* delAction = menu.addAction(tr("删除"));
    delAction->setEnabled(item != nullptr);

    // 根节点不可删除
    if (item && !item->parent())
        delAction->setEnabled(false);

    QAction* chosen = menu.exec(event->globalPos());
    if (!chosen)
        return;

    if (chosen == delAction && delAction->isEnabled())
    {
        item->removeCommandAndDeleteItem();
        item = nullptr;
        emit treeModified();
    }
}


AST_NAMESPACE_END
