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
#include "AstGUI/UiCommon.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QDropEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>

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

    // ---- 重命名 ----
    QAction* renameAction = menu.addAction(tr("重命名"));
    renameAction->setEnabled(item != nullptr);

    menu.addSeparator();

    // ---- 在上方插入 / 在下方插入 ----
    QMenu* insertAboveMenu = createInsertMenu(tr("在上方插入"));
    QMenu* insertBelowMenu = createInsertMenu(tr("在下方插入"));
    menu.addMenu(insertAboveMenu);
    menu.addMenu(insertBelowMenu);

    // 根节点不允许通过"在上方/下方插入"的方式插入
    bool canInsertAround = item && item->parent();
    insertAboveMenu->setEnabled(canInsertAround);
    insertBelowMenu->setEnabled(canInsertAround);

    // ---- 添加子命令（仅对 Sequence 节点有效） ----
    auto* selectedCmd = item ? item->command() : nullptr;
    bool isSequence = aobject_cast<Sequence*>(selectedCmd) != nullptr;
    if (isSequence)
    {
        QMenu* addChildMenu = createInsertMenu(tr("添加子命令"));
        menu.addMenu(addChildMenu);
    }

    menu.addSeparator();

    // ---- 删除 ----
    QAction* delAction = menu.addAction(tr("删除"));
    delAction->setEnabled(item != nullptr);

    // 根节点不可删除
    if (item && !item->parent())
        delAction->setEnabled(false);

    menu.addSeparator();

    // ---- 概要 ----
    QAction* summaryAction = menu.addAction(tr("概要"));
    summaryAction->setEnabled(item != nullptr);

    // ---- 执行菜单 ----
    QAction* chosen = menu.exec(event->globalPos());
    if (!chosen)
        return;

    // 重命名
    if (chosen == renameAction)
    {
        auto* cmd = item->command();
        if (!cmd)
            return;

        bool ok = false;
        QString newName = QInputDialog::getText(
            this, tr("重命名"), tr("新名称:"),
            QLineEdit::Normal, QString::fromStdString(cmd->getName()), &ok);

        if (ok && !newName.isEmpty())
        {
            cmd->setName(newName.toStdString());
            item->setText(0, newName);
        }
    }
    // 删除
    else if (chosen == delAction && delAction->isEnabled())
    {
        item->removeCommandAndDeleteItem();
        item = nullptr;
        emit treeModified();
    }
    // 概要
    else if (chosen == summaryAction)
    {
        if (auto* cmd = item->command())
            emit commandSummaryRequested(cmd);
    }
    // 在上方插入 / 在下方插入 / 添加子命令
    else if (chosen->data().isValid())
    {
        QString typeName = chosen->data().toString();
        if (chosen->parent() == insertAboveMenu)
        {
            handleInsertAction(chosen, item, true);
        }
        else if (chosen->parent() == insertBelowMenu)
        {
            handleInsertAction(chosen, item, false);
        }
        else
        {
            // 添加子命令：插入到当前 Sequence 的末尾
            insertCommandRelativeToItem(item, typeName, false);
        }
    }
}

QMenu* UiCommandTree::createInsertMenu(const QString& title)
{
    auto* menu = new QMenu(title, this);

    auto* a1 = menu->addAction(aUiClassIcon("InitialState"), tr("初始状态"));
    a1->setData("InitialState");

    auto* a2 = menu->addAction(aUiClassIcon("Propagate"), tr("预报段"));
    a2->setData("Propagate");

    auto* a3 = menu->addAction(aUiClassIcon("Maneuver"), tr("机动段"));
    a3->setData("Maneuver");

    auto* a4 = menu->addAction(aUiClassIcon("Sequence"), tr("序列段"));
    a4->setData("Sequence");

    auto* a5 = menu->addAction(aUiClassIcon("TargeterSequence"), tr("瞄准序列段"));
    a5->setData("TargeterSequence");

    return menu;
}

void UiCommandTree::handleInsertAction(QAction* action, UiCommandTreeItem* item, bool above)
{
    Q_UNUSED(action);
    if (!item)
        return;
    QString typeName = action->data().toString();
    insertCommandRelativeToItem(item, typeName, above);
}

void UiCommandTree::insertCommandRelativeToItem(UiCommandTreeItem* item, const QString& typeName, bool above)
{
    if (!item)
        return;

    auto* cmd = item->command();
    if (!cmd)
        return;

    // 确定目标序列和插入位置
    // 如果右键选中的节点是 Sequence，则插入到该 Sequence 内部
    // 否则插入到该节点的父序列中
    Sequence* targetSeq = nullptr;
    int insertIndex = 0;

    auto* parentTreeItem = dynamic_cast<UiCommandTreeItem*>(item->parent());
    if (parentTreeItem)
    {
        targetSeq = aobject_cast<Sequence*>(parentTreeItem->command());
        if (targetSeq)
        {
            insertIndex = parentTreeItem->indexOfChild(item);
            if (!above)
                ++insertIndex;
        }
    }

    // 如果没有父序列（根节点），尝试将选中节点本身当作序列
    if (!targetSeq)
    {
        targetSeq = aobject_cast<Sequence*>(cmd);
        if (targetSeq)
        {
            // 添加到序列末尾（above/below 对子添加无意义，默认追加到末尾）
            insertIndex = static_cast<int>(targetSeq->getCommands().size());
        }
    }

    if (!targetSeq)
        return;

    // 通过 RTTI 创建新命令
    std::string typeNameStd = typeName.toStdString();
    auto* newObj = aNewObject(typeNameStd);
    auto* newCmd = aobject_cast<MissionCommand*>(newObj);
    if (!newCmd)
    {
        qWarning("Failed to create command of type: %s", typeNameStd.c_str());
        return;
    }

    // 设置默认名称（类型名 + 序号）
    std::string defaultName = typeNameStd + std::to_string(targetSeq->getCommands().size() + 1);
    newCmd->setName(defaultName);

    // 插入到序列
    targetSeq->insertCommand(insertIndex, newCmd);

    // 刷新树
    refresh();

    emit treeModified();
    emit commandInserted(newCmd);
}


AST_NAMESPACE_END
