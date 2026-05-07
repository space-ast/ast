///
/// @file      ChatToolsDefault.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-27
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

class ChatTools;
class ChatSession;
class JsonValue;





/// @brief 初始化智能体工具集合
/// @param tools 工具集合
void aInitAgentTools(ChatTools& tools);


/// @brief 初始化智能体会话
/// @param session 会话
/// @details 初始化智能体会话
void aInitAgentSession(ChatSession& session);


/// @brief 获取智能体系统提示
/// @return 智能体系统提示
const char* aAgentSystemPrompt();


/// @brief 获取默认的智能体工具集合
AST_AI_API ChatTools& aAgentTools();

/// @brief 获取默认的智能体工具集合的JSON表示
AST_AI_API JsonValue aAgentToolsJson();

/// @brief 获取默认的智能体工具集合的JSON字符串表示
AST_AI_API std::string aAgentToolsJsonStr();

/// @brief 使用智能体工具集合处理单次工具调用
AST_AI_API std::string aAgentToolsHandleToolCall(const JsonValue& toolCall);

/// @brief 使用智能体工具集合处理单次工具调用
AST_AI_API std::string aAgentToolsHandleToolCall(const std::string& toolCallStr);


/*! @} */

AST_NAMESPACE_END
