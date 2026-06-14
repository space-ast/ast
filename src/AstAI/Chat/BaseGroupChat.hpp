///
/// @file      BaseGroupChat.hpp
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class ChatMessage;

class AST_AI_API BaseGroupChat
{
public:
    virtual ~BaseGroupChat() = default;

    /// @brief 运行一次聊天，返回最终的响应消息，过程中会处理工具调用、多个Agent交互等逻辑
    /// @param message 输入消息
    /// @param response 输出消息
    /// @return errc_t 
    virtual errc_t run(const ChatMessage& message, ChatMessage& response) = 0;
};


/*! @} */

AST_NAMESPACE_END
