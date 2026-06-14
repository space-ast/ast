///
/// @file      ChatSession.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-23
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

#include "ChatSession.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstAI/AssistantAgent.hpp"
#include <ctime>

AST_NAMESPACE_BEGIN

// #define _AST_DEBUG_CHAT_SESSION


ChatSession::ChatSession()
{
}

std::string ChatSession::chat(StringView message, int maxIterForToolCalls)
{
    this->messages_.addUserMessage(message);
    errc_t rc = this->agent().run(this->messages_, maxIterForToolCalls);
    if(rc != 0)
        return this->lastError();
    return this->messages_.back().content();
}

errc_t ChatSession::sendMessage(StringView message)
{
    this->messages_.addUserMessage(message);
    return this->makeChatCompletion();
}


errc_t ChatSession::makeChatCompletion()
{
    return this->agent().runOneStep(messages_);
}


void ChatSession::setSystemPrompt(StringView systemPrompt)
{
    this->messages_.setSystemPrompt(systemPrompt);
}

void ChatSession::handleToolCalls(const JsonValue& toolCalls)
{
    this->agent().handleToolCalls(toolCalls, messages_);
}

AssistantAgent& ChatSession::agent() const
{
    if(!agent_)
    {
        agent_.reset(new AssistantAgent());
    }
    return *agent_.get();
}

AST_NAMESPACE_END
