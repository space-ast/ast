///
/// @file      ChatMessage.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/StringView.hpp"
#include "AstUtil/JsonValue.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class JsonValue;


/// @brief 消息角色
enum class EChatRole {
    eUser,      ///< 用户
    eAssistant, ///< 助手
    eSystem,    ///< 系统
    eTool       ///< 工具
};


AST_AI_API std::string toString(EChatRole role);


/// @brief 聊天消息
class ChatMessage 
{
public:
    /// @brief 构造函数
    ChatMessage() = default;

    /// @brief 构造函数
    /// @param role 消息角色
    /// @param content 消息内容
    ChatMessage(EChatRole role, StringView content)
        : role_(role), content_(content) 
    {}

    /// @brief 析构函数
    ~ChatMessage() = default;

    /// @brief 创建系统提示消息
    /// @param content 系统提示
    /// @return 系统提示消息
    static ChatMessage System(StringView content)
    {
        return ChatMessage(EChatRole::eSystem, content);
    }

    /// @brief 创建用户消息
    /// @param content 用户消息内容
    /// @return 用户消息
    static ChatMessage User(StringView content)
    {
        return ChatMessage(EChatRole::eUser, content);
    }

    /// @brief 创建助手消息
    /// @param content 助手消息内容
    /// @return 助手消息
    static ChatMessage Assistant(StringView content)
    {
        return ChatMessage(EChatRole::eAssistant, content);
    }

    static ChatMessage Assistant(StringView content, const JsonValue& toolCalls)
    {
        ChatMessage msg(EChatRole::eAssistant, content);
        msg.setToolCalls(toolCalls);
        return msg;
    }
    
    

    /// @brief 创建工具消息
    /// @param content 工具消息内容
    /// @param toolCallId 工具调用ID
    /// @return 工具消息
    static ChatMessage Tool(StringView content, StringView toolCallId)
    {
        ChatMessage msg(EChatRole::eTool, content);
        msg.setToolCallId(toolCallId);
        return msg;
    }

    /// @brief 转换为JSON值
    /// @return JSON值
    AST_AI_API
    JsonValue toJson() const;

    /// @brief 设置消息角色
    void setRole(EChatRole role) { role_ = role; }
    /// @brief 获取消息角色
    EChatRole role() const { return role_; }

    /// @brief 设置角色名称
    void setName(const std::string& name) { name_ = name; }
    /// @brief 获取角色名称
    std::string name() const { return name_; }

    /// @brief 设置消息内容
    void setContent(StringView content) { content_ = std::string(content); }
    /// @brief 获取消息内容
    std::string content() const { return content_; }

    /// @brief 设置推理内容
    void setReasoningContent(StringView reasoningContent) { reasoningContent_ = std::string(reasoningContent); }
    /// @brief 获取推理内容
    std::string reasoningContent() const { return reasoningContent_; }
    
    /// @brief 设置工具调用ID
    void setToolCallId(StringView toolCallId) { toolCallId_ = std::string(toolCallId); }
    
    /// @brief 获取工具调用ID
    std::string toolCallId() const { return toolCallId_; }
    

    /// @brief 设置工具调用列表
    void setToolCalls(const JsonValue& toolCalls) { toolCalls_ = toolCalls; }

    /// @brief 获取工具调用列表
    const JsonValue& toolCalls() const { return toolCalls_; }

private:
    EChatRole        role_{EChatRole::eUser};           ///< 消息角色
    std::string      name_;                             ///< 角色名称
    std::string      content_;                          ///< 消息内容
    std::string      reasoningContent_;                 ///< 推理内容（当角色为ASSISTANT时使用）
    std::string      toolCallId_;                       ///< 工具调用ID（当角色为TOOL时使用）
    JsonValue        toolCalls_;                        ///< 工具调用列表
};


/*! @} */

AST_NAMESPACE_END
