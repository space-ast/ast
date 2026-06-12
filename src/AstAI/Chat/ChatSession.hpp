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
#include "AstAI/OpenAI.hpp"
#include "AstAI/ChatTool.hpp"
#include "AstAI/ChatMessages.hpp"
#include "AstAI/ChatTools.hpp"
#include <string>
#include <vector>
#include <functional>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Chat
    @{
*/



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

    /// @brief 发送消息，不会处理工具调用，直接返回响应消息
    /// @param message 用户消息
    /// @return 响应内容，如果失败则返回 nullptr
    const ChatMessage* sendMessage(StringView message);

    /// @brief 执行工具调用循环
    /// @param message 含工具调用的输入消息
    /// @param maxIterForToolCalls 最大工具调用轮数
    /// @return 错误码，0表示成功，非0表示失败
    errc_t loopToolCalls(const ChatMessage& message, int maxIterForToolCalls = 100);

    /// @brief 处理工具调用
    /// @param message 含工具调用的输入消息
    /// @return 错误码，0表示成功，非0表示失败
    void handleToolCalls(const ChatMessage& message);

    /// @brief 设置系统提示
    /// @param systemPrompt 系统提示
    void setSystemPrompt(StringView systemPrompt);

    /// @brief 获取消息历史
    /// @return 消息历史
    ChatMessages& messages(){return messages_;}

    /// @brief 获取工具集合
    /// @return 工具集合
    ChatTools& tools(){return tools_;}

    /// @brief 获取当前使用的AI接口
    /// @note 目前还不支持指定或者切换client，只能使用对象内部默认的AI接口
    OpenAI& client();

    /// @brief 获取最后一次错误信息
    const std::string& lastError() const {return lastError_;}

    /// @brief 生成聊天完成响应
    /// @return 响应内容，如果失败则返回 nullptr
    const ChatMessage* makeChatCompletion();
private:

    /// @brief 处理工具调用
    /// @param toolCalls 工具调用列表
    void handleToolCalls(const JsonValue& toolCalls);

    /// @brief 处理单个工具调用
    /// @param toolCall 单个工具调用
    std::string handleToolCall(const JsonValue& toolCall);

private:
    OpenAI* client_{nullptr};               ///< 当前使用的AI接口
    OpenAI internalClient_;                 ///< 内部默认的AI接口
    ChatMessages messages_;                 ///< 消息历史
    ChatTools tools_;                       ///< 工具集合
    std::string lastError_;                 ///< 最后一次错误信息
};

/*! @} */

AST_NAMESPACE_END