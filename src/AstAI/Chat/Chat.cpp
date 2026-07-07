///
/// @file      Chat.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-25
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

#include "Chat.hpp"
#include "ChatSession.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN


ChatSession& aChatSession()
{
    static ChatSession session;
    return session;
}

std::string aChat(StringView message)
{
    return aChatSession().chat(message);
}


errc_t aChat(StringView message, std::string& response)
{
    response = aChatSession().chat(message);
    return 0;
}

void aChatClearMessages()
{
    auto& msgs = aChatSession().messages();
    // 保留系统提示词（如果第一条是系统消息），清除其余
    if(!msgs.empty() && msgs.front().role() == EChatRole::eSystem)
    {
        ChatMessage sysMsg = msgs.front();
        msgs.clear();
        msgs.addMessage(sysMsg);
    }
    else
    {
        msgs.clear();
    }
}

void aChatClearAllMessages()
{
    aChatSession().messages().clear();
}

AST_NAMESPACE_END
