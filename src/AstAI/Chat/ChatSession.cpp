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
    const ChatMessage* res = this->sendMessage(message);
    if(res == nullptr)
        return lastError_;
    errc_t rc = this->loopToolCalls(*res, maxIterForToolCalls);
    if(rc != 0)
        return lastError_;
    return this->messages_.back().content();
}

const ChatMessage* ChatSession::sendMessage(StringView message)
{
    this->messages_.addUserMessage(message);
    return this->makeChatCompletion();
}


const ChatMessage* ChatSession::makeChatCompletion()
{
    /*
    @todo 
    模型名称和温度参数被硬编码在 makeChatCompletion 方法中。
    这限制了会话的灵活性。需要将这些参数移动到 LLMConfig 中，
    或者作为 ChatSession 的成员变量，以便在运行时进行配置。

    @fixme
    thinking 字段是 DeepSeek 特有的扩展参数。
    如果将来切换到标准的 OpenAI 或其他服务商，该请求可能会因为包含未知字段而被拒绝。
    需要根据当前使用的 Provider 动态添加此字段，或者将其放入配置项中。
    */

    auto& client = this->client();
    JsonValue json;
    json["messages"] = this->messages_.toJson();
    json["model"] = "deepseek-v4-flash";
    json["temperature"] = 0.2;
    json["thinking"]["type"] = "disabled";  // "enabled" or "disabled"
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
            return nullptr;
        }
        // ast_printf("res: %s\n", res.toJsonString().c_str());
        auto& choices = res["choices"];
        if(!choices.isArray() || choices.size() == 0)
        {
            aError("choices is empty or not array");
            lastError_ = "choices is empty or not array";
            return nullptr;
        }
        JsonValue& message = choices[0]["message"];
        std::string response = message["content"].toString();
        ast_printf("ai: %s\n", response.c_str());
        JsonValue& toolCalls = message["tool_calls"];
        auto msg = ChatMessage::Assistant(response, toolCalls);
        if(!message["reasoning_content"].isNull())
            msg.setReasoningContent(message["reasoning_content"].toString());
        this->messages_.addMessage(msg);

        return &this->messages_.back();
    }
}

errc_t ChatSession::loopToolCalls(const ChatMessage& message, int maxIterForToolCalls)
{
    const ChatMessage* msg = &message;
    for(int i = 0; i < maxIterForToolCalls; i++)
    {
        if(msg->hasToolCalls())
        {
            this->handleToolCalls(*msg);
            msg = this->makeChatCompletion();
            if(msg == nullptr)
                return -1;
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
