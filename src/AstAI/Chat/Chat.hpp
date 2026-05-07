///
/// @file      Chat.hpp
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
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 聊天接口
/// @param message 消息内容
/// @note 该接口会阻塞调用，直到收到响应或超时
/// @return 响应内容
AST_AI_API std::string aChat(StringView message);

/// @brief 聊天接口
/// @param message 消息内容
/// @param response 响应内容
/// @return 错误码
/// @note 该接口会阻塞调用，直到收到响应或超时
AST_AI_API errc_t aChat(StringView message, std::string& response);


/// @brief 清除聊天消息历史
/// @note 该接口会清空所有聊天消息，不包括系统提示
AST_AI_API void aChatClearMessages();


/// @brief 清除所有聊天消息
/// @note 该接口会清空所有聊天消息，包括系统提示
AST_AI_API void aChatClearAllMessages();



/*! @} */

AST_NAMESPACE_END
