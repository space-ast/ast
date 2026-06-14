///
/// @file      AssistantAgent.cpp
/// @brief     聊天智能体实现
/// @author    axel
/// @date      2026-06-13
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

#include "AssistantAgent.hpp"
#include "AstAI/AgentInit.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"
#include "AstAI/DeepSeek.hpp"
#include <ctime>

AST_NAMESPACE_BEGIN

// #define _AST_DEBUG_CHAT_AGENT

// —— 工厂方法 ——

AssistantAgent AssistantAgent::DefaultAgent()
{
    return AssistantAgent();
}

AssistantAgent* AssistantAgent::NewDefaultAgent()
{
    return new AssistantAgent(DefaultAgent());
}

AssistantAgent AssistantAgent::SpaceEngineer()
{
    AssistantAgent agent;
    agent.setSystemPrompt(aAgentSystemPrompt());
    aInitAgentTools(agent.tools());
    auto extraBody = R"(
        {
            "thinking":{
                "type":"disabled"
            }
        }
    )"_json;
    agent.config().setExtraBody(extraBody);
    return agent;
}

AssistantAgent* AssistantAgent::NewSpaceEngineer()
{
    return new AssistantAgent(SpaceEngineer());
}


// —— 构造函数 ——

AssistantAgent::AssistantAgent(StringView systemPrompt)
    : systemPrompt_(systemPrompt.begin(), systemPrompt.end())
{
}

// —— 身份 ——

void AssistantAgent::setSystemPrompt(StringView prompt)
{
    systemPrompt_ = std::string(prompt);
}

const std::string& AssistantAgent::systemPrompt() const
{
    return systemPrompt_;
}

// —— 能力 ——

void AssistantAgent::addTool(std::unique_ptr<ChatTool> tool)
{
    tools_.addTool(std::move(tool));
}

ChatTools& AssistantAgent::tools()
{
    return tools_;
}

// —— 配置 ——

LLMConfig& AssistantAgent::config()
{
    return config_;
}

const LLMConfig& AssistantAgent::config() const
{
    return config_;
}

// —— 客户端 ——

void AssistantAgent::setClient(std::unique_ptr<LLMClient> client)
{
    client_ = std::move(client);
}

LLMClient& AssistantAgent::client()
{
    auto client = client_.get();
    if(!client)
    {
        client = new DeepSeek();
        client_.reset(client);
    }
    return *client;
}

// —— 交互 ——

errc_t AssistantAgent::runOneStep(ChatMessages& messages)
{
    ChatMessage response;
    errc_t rc = this->runOneStep(messages, response);
    if(rc == 0)
        messages.push_back(response);
    return rc;
}


errc_t AssistantAgent::run(ChatMessages& messages)
{
    return this->run(messages, maxToolIterations_);
}

errc_t AssistantAgent::run(ChatMessages &messages, int maxSteps)
{
    for(int i = 0; i < maxSteps; i++)
    {
        errc_t rc = this->runOneStep(messages);
        if(rc != 0)
            return rc;
        auto& response = messages.back();
        if(response.hasToolCalls())
        {
            this->handleToolCalls(response.toolCalls(), messages);
        }
        else
        {
            break;  // 没有工具调用，对话结束
        }
    }
    return 0;
}


errc_t AssistantAgent::runOneStep(const ChatMessages &messages, ChatMessage &response)
{
    // 1. 构建请求体
    JsonValue json;
    
    JsonValue messagesJson = messages.toJson();
    // 设置系统提示
    if(!systemPrompt_.empty())
    {
        bool hasSystem = messagesJson.size() > 0
                      && messagesJson[0]["role"].toString() == "system";
        if(hasSystem)
        {
            messagesJson[0]["content"] = systemPrompt_;
        }
        else
        {
            ChatMessage systemMessage = ChatMessage::System(systemPrompt_);
            messagesJson.prepend(systemMessage.toJson());
        }
    }
    json["messages"] = messagesJson;
    json["model"] = config_.model();
    json["temperature"] = config_.temperature();
    const auto& extra = config_.extraBody();
    if(!extra.isNull() && extra.isObject())
    {
        if(!extra["thinking"].isNull())
            json["thinking"] = extra["thinking"];
    }
    json["stream"] = false;

    // 设置工具
    if(!tools_.empty())
    {
        json["tools"] = tools_.toJson();
    }

    // 2. 发送请求
    JsonValue res = client().chat(json);

    // 3. 解析响应
    if(!res["error"].isNull())
    {
        lastError_ = res["error"]["message"].toString();
        aError("response error: %s", lastError_.c_str());
        return -1;
    }
    auto& choices = res["choices"];
    if(!choices.isArray() || choices.size() == 0)
    {
        lastError_ = "choices is empty or not array";
        aError("%s", lastError_.c_str());
        return -1;
    }

    JsonValue& msg = choices[0]["message"];
    std::string content = msg["content"].toString();
#ifdef _AST_DEBUG_CHAT_AGENT
    std::string msgJsonStr = msg.toJsonString();
    ast_printf("msgJsonStr: %s\n", msgJsonStr.c_str());
#endif
    ast_printf("ai: %s\n", content.c_str());
    response = ChatMessage::Assistant(
        content,
        msg["tool_calls"]
    );
    // 设置Agent的名称
    if(!name_.empty())
        response.setName(name_);
    if(!msg["reasoning_content"].isNull())
        response.setReasoningContent(msg["reasoning_content"].toString());

    return 0;
}


void AssistantAgent::handleToolCalls(const JsonValue &toolCalls, ChatMessages &messages)
{
    if(!toolCalls.isArray())
        return;
    // 复制toolCalls，避免在处理toolcall过程中修改messages从而导致toolCalls引用失效
    JsonValue toolCallsTemp = toolCalls;
    for(auto& item: toolCallsTemp.getArray())
    {
        #ifdef _AST_DEBUG_CHAT_AGENT
        ast_printf("toolCall: %s\n", item.toJsonString().c_str());
        clock_t start = clock();
        #endif

        std::string response = this->handleToolCall(item);

        #ifdef _AST_DEBUG_CHAT_AGENT
        clock_t end = clock();
        ast_printf("handleToolCall cost: %ld ms\n", (end - start) * 1000 / CLOCKS_PER_SEC);
        ast_printf("response: %s\n", response.c_str());
        #endif

        std::string id = item["id"];
        messages.addToolMessage(response, id);
    }
}

std::string AssistantAgent::handleToolCall(const JsonValue& toolCall)
{
    return tools_.handleToolCall(toolCall);
}


// ── 流式推理 ─────────────────────────────────────────────────────


errc_t AssistantAgent::runStream(ChatMessages& messages, ChatEventHandler& handler)
{
    return this->runStream(messages, handler, maxToolIterations_);
}



errc_t AssistantAgent::runStream(ChatMessages& messages,
                                  ChatEventHandler& handler,
                                  int maxSteps)
{
    for (int i = 0; i < maxSteps; i++)
    {
        // 单步流式推理 — 实时分发 text/thought 事件
        errc_t rc = this->runOneStepStream(messages, handler);
        if (rc != 0)
            return rc;

        // 检查是否有工具调用
        auto& response = messages.back();
        if (!response.hasToolCalls())
            break;  // 对话结束，没有工具调用

        // 处理工具调用 — 分发 ToolCallRequest 和 ToolCallResult 事件
        const JsonValue& toolCalls = response.toolCalls();
        JsonValue toolCallsTemp = toolCalls;
        for (auto& item : toolCallsTemp.getArray())
        {
            // 提取工具信息
            std::string tcId   = item["id"].toString();
            std::string tcName = item["function"]["name"].toString();
            std::string tcArgs = item["function"]["arguments"].toString();

            // 通知用户：LLM 请求调用工具
            handler.onToolCallRequest(tcId, tcName, tcArgs);

            // 执行工具
            std::string tcResult = this->handleToolCall(item);

            // 将工具结果加入消息历史
            messages.addToolMessage(tcResult, tcId);

            // 通知用户：工具执行完毕
            handler.onToolCallResult(tcId, tcName, tcResult);
        }
    }

    handler.onComplete();
    return 0;
}



errc_t AssistantAgent::runOneStepStream(ChatMessages& messages,
                                         ChatEventHandler& handler)
{
    // 1. 构建请求体（与非流式 runOneStep 相同，但 stream: true）
    JsonValue json;

    JsonValue messagesJson = messages.toJson();

    // 设置系统提示
    if (!systemPrompt_.empty())
    {
        bool hasSystem = messagesJson.size() > 0
                      && messagesJson[0]["role"].toString() == "system";
        if (hasSystem)
        {
            messagesJson[0]["content"] = systemPrompt_;
        }
        else
        {
            ChatMessage systemMessage = ChatMessage::System(systemPrompt_);
            messagesJson.prepend(systemMessage.toJson());
        }
    }

    json["messages"]    = messagesJson;
    json["model"]       = config_.model();
    json["temperature"] = config_.temperature();
    const auto& extra = config_.extraBody();
    if (!extra.isNull() && extra.isObject())
    {
        if (!extra["thinking"].isNull())
            json["thinking"] = extra["thinking"];
    }
    json["stream"] = true;  // 流式模式

    // 设置工具
    if (!tools_.empty())
    {
        json["tools"] = tools_.toJson();
    }

    // 2. 发送流式请求
    JsonValue accumulatedResult;
    errc_t rc = client().chatStream(json, handler, accumulatedResult);
    if (rc != 0)
    {
        lastError_ = "stream request failed, error code: " + std::to_string(rc);
        return rc;
    }

    // 3. 解析累计结果（与非流式 runOneStep 逻辑相同）
    if (!accumulatedResult["error"].isNull())
    {
        lastError_ = accumulatedResult["error"]["message"].toString();
        aError("response error: %s", lastError_.c_str());
        return -1;
    }

    auto& choices = accumulatedResult["choices"];
    if (!choices.isArray() || choices.size() == 0)
    {
        lastError_ = "choices is empty or not array";
        aError("%s", lastError_.c_str());
        return -1;
    }

    JsonValue& msg = choices[0]["message"];
    std::string content = msg["content"].toString();

#ifdef _AST_DEBUG_CHAT_AGENT
    std::string msgJsonStr = msg.toJsonString();
    ast_printf("msgJsonStr: %s\n", msgJsonStr.c_str());
#endif

    ChatMessage response = ChatMessage::Assistant(
        content,
        msg["tool_calls"]
    );

    // 设置 Agent 名称
    if (!name_.empty())
        response.setName(name_);

    if (!msg["reasoning_content"].isNull())
        response.setReasoningContent(msg["reasoning_content"].toString());

    messages.push_back(response);

    return 0;
}


AST_NAMESPACE_END
