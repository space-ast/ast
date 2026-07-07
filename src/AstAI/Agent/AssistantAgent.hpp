///
/// @file      AssistantAgent.hpp
/// @brief     聊天助手智能体 — 身份 + 能力 + 配置 + 客户端的纯定义，可跨会话复用
/// @details   与 ChatSession 分离：Agent 持有"谁在说话"，Session 持有"跟谁在聊什么"
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

#pragma once

#include "AstGlobal.h"
#include "AstAI/ChatAgent.hpp"
#include "AstAI/ChatEventHandler.hpp"
#include "AstAI/LLMClient.hpp"
#include "AstAI/LLMConfig.hpp"
#include "AstAI/ChatTool.hpp"
#include "AstAI/ChatTools.hpp"
#include "AstAI/ChatMessage.hpp"
#include "AstAI/ChatMessages.hpp"
#include <string>
#include <memory>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

/// @brief 聊天智能体
/// @details 定义智能体的身份、能力、配置和LLM客户端。
///          智能体本身无状态，同一个智能体实例可参与多个不同的对话。
///
class AST_AI_API AssistantAgent : public ChatAgent
{
public:
    // —— 工厂方法 ——

    /// @brief 创建空的默认Agent
    static AssistantAgent DefaultAgent();

    static AssistantAgent* NewDefaultAgent();

    /// @brief 创建航天工程师Agent（注册默认航天工具和系统提示词）
    static AssistantAgent SpaceEngineer();

    /// @brief 创建航天工程师Agent（注册默认航天工具和系统提示词）
    static AssistantAgent* NewSpaceEngineer();

public:
    AssistantAgent() = default;
    explicit AssistantAgent(StringView systemPrompt);
    ~AssistantAgent() = default;
    AssistantAgent(AssistantAgent&&) = default;
    AssistantAgent& operator=(AssistantAgent&&) = default;

    // —— 身份 ——

    /// @brief 设置系统提示词
    void setSystemPrompt(StringView prompt);
    /// @brief 获取系统提示词
    const std::string& systemPrompt() const;

    // —— 能力 ——

    /// @brief 添加工具（通过函数指针/lambda）
    /// @return 工具指针（非拥有，生命周期由ChatAgent管理）
    template<typename Func>
    ChatTool* addTool(Func func)
    {
       return tools_.addTool(func);
    }

    /// @brief 添加工具（通过unique_ptr）
    void addTool(std::unique_ptr<ChatTool> tool);

    /// @brief 获取工具集合
    ChatTools& tools();

    // —— 配置 ——

    /// @brief 获取LLM配置（可修改）
    LLMConfig& config();
    /// @brief 获取LLM配置（只读）
    const LLMConfig& config() const;

    // —— 客户端 ——

    /// @brief 设置LLM客户端
    void setClient(std::unique_ptr<LLMClient> client);
    /// @brief 获取LLM客户端
    LLMClient& client();

    // —— 交互 ——

    errc_t run(ChatMessages& messages) override;


    /// @brief 执行多步推理（包含工具调用循环）
    /// @param messages 消息历史
    /// @param maxSteps 最大推理步数
    /// @return 错误码，0表示成功
    errc_t run(ChatMessages& messages, int maxSteps);

    /// @brief 执行单步推理
    /// @param messages 消息历史
    /// @return 错误码，0表示成功
    errc_t runOneStep(ChatMessages& messages);


    /// @brief 处理工具调用
    /// @param toolCalls 工具调用数组
    /// @param messages 消息历史
    void handleToolCalls(const JsonValue& toolCalls, ChatMessages& messages);

    /// @brief 处理单个工具调用
    /// @param toolCall 单个工具调用
    std::string handleToolCall(const JsonValue& toolCall);

    // —— 流式交互 ——
    errc_t runStream(ChatMessages& messages, ChatEventHandler& handler) override;


    /// @brief 执行多步推理（含工具调用循环，流式）
    /// @details 每步推理调用 runOneStepStream()，实时分派文本/推理事件。
    ///          工具调用请求和执行结果事件在各步之间触发。
    /// @param messages 消息历史
    /// @param handler  事件处理器
    /// @param maxSteps 最大推理步数（默认 10）
    /// @return 错误码，0表示成功
    errc_t runStream(ChatMessages& messages, ChatEventHandler& handler, int maxSteps);


    /// @brief 执行单步推理（流式）
    /// @details 在 LLM 生成过程中实时分发事件到 handler。
    ///          流式完成后将完整消息追加到 messages。
    /// @param messages 消息历史
    /// @param handler  事件处理器
    /// @return 错误码，0表示成功
    errc_t runOneStepStream(ChatMessages& messages, ChatEventHandler& handler);

    /// @brief 获取最后一次错误信息
    const std::string& lastError() const { return lastError_; }
private:
    errc_t runOneStep(const ChatMessages& messages, ChatMessage& response);

    /// @brief 构建 LLM 请求 JSON（runOneStep / runOneStepStream 共用）
    JsonValue buildRequestJson(const ChatMessages& messages, bool stream) const;

    /// @brief 解析 LLM 响应 JSON → ChatMessage（runOneStep / runOneStepStream 共用）
    errc_t parseResponseMessage(JsonValue& response, ChatMessage& outMessage);

private:
    std::string                    systemPrompt_;
    ChatTools                      tools_;
    LLMConfig                      config_;
    std::unique_ptr<LLMClient>     client_;                 ///< LLM客户端（run()前必须设置）
    std::string                    lastError_;              ///< 最后一次错误信息
    int                            maxToolIterations_{10};  ///< 最大工具调用次数（默认10次）
};

/*! @} */

AST_NAMESPACE_END
