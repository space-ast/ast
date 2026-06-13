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
#include <ctime>

AST_NAMESPACE_BEGIN

// #define _AST_DEBUG_CHAT_SESSION


ChatSession::ChatSession()
{
}

std::string ChatSession::chat(StringView message, int maxIterForToolCalls)
{
    errc_t rc = this->sendMessage(message);
    if(rc != 0)
        return lastError_;
    // sendMessage 成功后，assistant 回复位于 messages_ 末尾
    rc = this->loopToolCalls(this->messages_.back(), maxIterForToolCalls);
    if(rc != 0)
        return lastError_;
    return this->messages_.back().content();
}

errc_t ChatSession::sendMessage(StringView message)
{
    this->messages_.addUserMessage(message);
    return this->makeChatCompletion();
}


errc_t ChatSession::makeChatCompletion()
{
    auto& client = this->client();
    JsonValue json;
    json["messages"] = this->messages_.toJson();
    json["model"] = config_.model();
    json["temperature"] = config_.temperature();
    // 合并提供者特有的额外请求体字段（若无则跳过，保持对标准OpenAI的兼容性）
    const auto& extra = config_.extraBody();
    if(!extra.isNull() && extra.isObject())
    {
        if(!extra["thinking"].isNull())
            json["thinking"] = extra["thinking"];
        // 可在此处追加其他提供者特有字段
    }
    json["stream"] = false;
    json["tools"] = tools_.toJson();

    {
        #ifdef _AST_DEBUG_CHAT_SESSION
        clock_t start = clock();
        #endif

        JsonValue res = client.chat(json);

        #ifdef _AST_DEBUG_CHAT_SESSION
        clock_t end = clock();
        ast_printf("network request cost: %ld ms\n", (end - start) * CLOCKS_PER_SEC / 1000);
        #endif

        if(!res["error"].isNull())
        {
            std::string errorMsg = res["error"]["message"].toString();
            aError("response error: %s", errorMsg.c_str());
            lastError_ = errorMsg;
            return -1;
        }
        // ast_printf("res: %s\n", res.toJsonString().c_str());
        auto& choices = res["choices"];
        if(!choices.isArray() || choices.size() == 0)
        {
            aError("choices is empty or not array");
            lastError_ = "choices is empty or not array";
            return -1;
        }
        JsonValue& message = choices[0]["message"];
        std::string response = message["content"].toString();
        ast_printf("ai: %s\n", response.c_str());
        JsonValue& toolCalls = message["tool_calls"];
        auto msg = ChatMessage::Assistant(response, toolCalls);
        if(!message["reasoning_content"].isNull())
            msg.setReasoningContent(message["reasoning_content"].toString());
        this->messages_.addMessage(msg);

        return 0;
    }
}

errc_t ChatSession::loopToolCalls(const ChatMessage& message, int maxIterForToolCalls)
{
    // 在向量修改前复制 toolCalls，避免引用失效
    JsonValue toolCalls = message.toolCalls();
    for(int i = 0; i < maxIterForToolCalls; i++)
    {
        if(toolCalls.isArray() && toolCalls.size() > 0)
        {
            this->handleToolCalls(toolCalls);
            errc_t rc = this->makeChatCompletion();
            if(rc != 0)
                return rc;
            // 从最新的 assistant 回复中获取新的 toolCalls
            toolCalls = this->messages_.back().toolCalls();
        }
        else
        {
            break;
        }
    }
    return 0;
}

void ChatSession::handleToolCalls(const ChatMessage& message)
{
    // 这里必须拷贝，因为在添加消息后，msg指针会失效
    JsonValue toolCalls = message.toolCalls();
    this->handleToolCalls(toolCalls);
}


void ChatSession::setSystemPrompt(StringView systemPrompt)
{
    this->messages_.setSystemPrompt(systemPrompt);
}

void ChatSession::handleToolCalls(const JsonValue &toolCalls)
{
    for(auto& item: toolCalls.getArray())
    {
        #ifdef _AST_DEBUG_CHAT_SESSION
        ast_printf("toolCall: %s\n", item.toJsonString().c_str());
        clock_t start = clock();
        #endif
        
        std::string response = this->handleToolCall(item);

        #ifdef _AST_DEBUG_CHAT_SESSION
        clock_t end = clock();
        ast_printf("handleToolCall cost: %ld ms\n", (end - start) * CLOCKS_PER_SEC / 1000);
        ast_printf("response: %s\n", response.c_str());
        #endif
        
        std::string id = item["id"];
        this->messages_.addToolMessage(response, id);
    }
}



std::string ChatSession::handleToolCall(const JsonValue &toolCall)
{
    return this->tools_.handleToolCall(toolCall);
}

OpenAI& ChatSession::client() {
    if (client_ == nullptr) {
        client_ = &internalClient_;
    }
    return *client_;
}

AST_NAMESPACE_END
