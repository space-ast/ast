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


AST_NAMESPACE_BEGIN



ChatSession::ChatSession()
{
}

std::string ChatSession::sendMessage(StringView message)
{
    this->messages_.addUserMessage(message);
    return this->makeChatCompletion();
}


std::string ChatSession::makeChatCompletion(int maxIterations)
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
        JsonValue res = client.chat(json);
        if(!res["error"].isNull())
        {
            std::string errorMsg = res["error"]["message"].toString();
            return aText("response error: " + errorMsg);
        }
        // ast_printf("res: %s\n", res.toJsonString().c_str());
        auto& choices = res["choices"];
        if(!choices.isArray() || choices.size() == 0)
        {
            aError("choices is empty or not array");
            return aText("response error: choices is empty or not array");
        }
        JsonValue& message = choices[0]["message"];
        std::string response = message["content"].toString();
        ast_printf("ai: %s\n", response.c_str());
        JsonValue& toolCalls = message["tool_calls"];
        auto msg = ChatMessage::Assistant(response, toolCalls);
        if(!message["reasoning_content"].isNull())
            msg.setReasoningContent(message["reasoning_content"].toString());
        this->messages_.addMessage(msg);

        if(toolCalls.isArray() && toolCalls.size() > 0)
        {
            this->handleToolCalls(toolCalls, maxIterations);
        }
        return response;
    }
}

void ChatSession::setSystemPrompt(StringView systemPrompt)
{
    this->messages_.setSystemPrompt(systemPrompt);
}

void ChatSession::handleToolCalls(const JsonValue &toolCalls, int maxInteractions)
{
    if(toolCalls.isArray() && toolCalls.size() > 0)
    {
        for(auto& item: toolCalls.getArray())
        {
            std::string response = this->handleToolCall(item);
            std::string id = item["id"];
            this->messages_.addToolMessage(response, id);
        }
        if(maxInteractions > 0)
            this->makeChatCompletion(maxInteractions-1);
        else
            aInfo("max interactions reached");
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
