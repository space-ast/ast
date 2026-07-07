///
/// @file      ChatSession.hpp
/// @brief     聊天会话管理
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

#pragma once

#include "AstGlobal.h"
#include "AstAI/LLMClient.hpp"
#include "AstAI/ChatAgent.hpp"
#include "AstAI/LLMConfig.hpp"
#include "AstAI/ChatTool.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstAI/ChatTools.hpp"
#include "AstAI/AssistantAgent.hpp"
#include <string>
#include <vector>
#include <functional>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/

class AssistantAgent;

/// @brief 聊天会话
class AST_AI_API ChatSession {
public:
    /// @brief 构造函数
    /// @param api_key API密钥
    /// @param model 模型名称
    /// @param base_url API基础URL
    ChatSession();

    /// @brief 对话，内部会处理工具调用循环直到没有更多工具调用或最大交互轮数到达，然后返回最终响应内容
    /// @param message 用户消息
    /// @return 最终响应内容（执行工具调用循环后的最终结果）
    std::string chat(StringView message, int maxIterForToolCalls = 100);

    /// @brief 流式输出对话，内部会处理工具调用循环直到没有更多工具调用或最大交互轮数到达，然后返回最终响应内容
    /// @param message 用户消息
    /// @param handler 事件处理函数
    /// @return 最终响应内容（执行工具调用循环后的最终结果）
    std::string chatStream(StringView message, ChatEventHandler& handler, int maxIterForToolCalls = 100);

    /// @brief 发送消息，不会处理工具调用，直接返回响应消息
    /// @param message 用户消息
    /// @return 错误码，0表示成功，消息追加到messages_末尾
    errc_t sendMessage(StringView message);

    /// @brief 设置系统提示
    /// @param systemPrompt 系统提示
    void setSystemPrompt(StringView systemPrompt);

    /// @brief 获取消息历史
    /// @return 消息历史
    ChatMessages& messages(){return messages_;}

    /// @brief 获取聊天智能体
    /// @return 聊天智能体
    AssistantAgent& agent() const;

    /// @brief 设置聊天智能体（转移所有权）
    void setAgent(std::unique_ptr<AssistantAgent> agent){agent_ = std::move(agent);}

    /// @brief 获取工具集合
    /// @return 工具集合
    ChatTools& tools(){return agent().tools();}

    /// @brief 获取当前使用的AI接口
    LLMClient& client(){return agent().client();}

    /// @brief 获取最后一次错误信息
    const std::string& lastError() const {return agent().lastError();}

    /// @brief 获取LLM配置
    LLMConfig& config() { return agent().config(); }
    /// @brief 获取LLM配置（只读）
    const LLMConfig& config() const { return agent().config(); }

    /// @brief 生成聊天完成响应
    /// @return 错误码，0表示成功，成功时响应消息追加到messages_末尾
    errc_t makeChatCompletion();

    /// @brief 处理工具调用
    /// @param toolCalls 工具调用列表
    void handleToolCalls(const JsonValue& toolCalls);

private:
    mutable std::unique_ptr<AssistantAgent> agent_;     ///< 助手智能体
    ChatMessages messages_;                             ///< 消息历史
};

/*! @} */

AST_NAMESPACE_END