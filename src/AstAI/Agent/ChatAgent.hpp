///
/// @file      AssistantAgent.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-14
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

class ChatMessages;
class ChatEventHandler;

/// @brief 聊天智能体抽象基类
/// @details 定义所有聊天智能体的公共接口，包括名称标识和运行方法。
class AST_AI_API ChatAgent
{
public:
    virtual ~ChatAgent() = default;

    /// @brief 执行智能体逻辑
    /// @param messages 消息历史（可修改）
    /// @return 错误码，0表示成功
    virtual errc_t run(ChatMessages& messages) = 0;

    /// @brief 执行智能体逻辑（流式）
    /// @param messages 消息历史（可修改）
    /// @param handler  事件处理器（实时接收 delta 事件）
    /// @return 错误码，0表示成功
    virtual errc_t runStream(ChatMessages& messages, ChatEventHandler& handler) = 0;

    // —— 名称 ——

    /// @brief 设置智能体名称（用于多Agent场景下的标识）
    void setName(StringView name) { name_ = std::string(name); }

    /// @brief 获取智能体名称
    const std::string& name() const { return name_; }

protected:
    std::string name_{}; ///< 智能体名称
};

/*! @} */

AST_NAMESPACE_END
