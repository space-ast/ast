///
/// @file      UiCommandTreeItem.cpp
/// @brief     Command 对象树项实现
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

#include "UiCommandTreeItem.hpp"
#include "AstCore/Sequence.hpp"

AST_NAMESPACE_BEGIN

QList<UiObjectTreeItem*> UiCommandTreeItem::createChildItems(const TreeBuildOptions& options) const
{
    auto* seq = aobject_cast<Sequence*>(object());
    if (!seq)
        return {};

    QList<UiObjectTreeItem*> items;

    for (auto& cmd : seq->getCommands())
    {
        if (!cmd)
            continue;
        items.push_back(new UiCommandTreeItem(cmd.get()));
    }
    return items;
}

UiCommandTreeItem* UiCommandTreeItem::clone() const
{
    return new UiCommandTreeItem(*this);
}


errc_t UiCommandTreeItem::removeCommand()
{
    Command* cmd = this->command();
    if (!cmd)
        return eErrorNullPtr;
    return cmd->removeFromParentSequence();
}

void UiCommandTreeItem::removeCommandAndDeleteItem()
{
    if (removeCommand() == eNoError)
    {
        delete this;
    }
}

AST_NAMESPACE_END
