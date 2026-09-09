///
/// @file      CommandManager.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-11
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

#include "CommandDispatcher.hpp"
#include "AstUtil/Logger.hpp"
#include "CommandInit.hpp"
#include "CommandAPI.hpp"

AST_NAMESPACE_BEGIN

CommandDispatcher::CommandDispatcher(bool whetherInit)
{
    if (whetherInit)
    {
        aCommandDispatcherInit(*this);
    }
}

void CommandDispatcher::addRuleHandler(StringView tmpl, std::shared_ptr<CommandHandler> handler)
{
    trie_.addRule(tmpl, handler);
}

CommandTrie::Node& CommandDispatcher::addRule(StringView tmpl)
{
    return trie_.addRule(tmpl);
}


errc_t CommandDispatcher::execute(StringView command, CommandResult& result) const 
{
    RoutingHandleResult handleResult;
    errc_t rc = trie_.find(command, handleResult);
    auto handler = handleResult.handler();
    if(rc || !handler)
    {
        aError(_("未找到匹配输入命令的处理函数: '%.*s'"), command.size(), command.data());
        return rc;
    }
    return handler->handle(handleResult.params(), result);
}


AST_NAMESPACE_END
