///
/// @file      ChatConsole.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-15
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

#include "ChatConsole.hpp"
#include "AstUtil/IO.hpp"
#include <stdio.h>

AST_NAMESPACE_BEGIN

void ChatConsole::onTextChunk(const std::string& text)
{
    ast_printf("%s", text.c_str());
    fflush(stdout);
}

void ChatConsole::onThought(const std::string& thought)
{
    ast_printf("%s", thought.c_str());
    fflush(stdout);
}

void ChatConsole::onToolCallRequest(const std::string& /*toolCallId*/,
                       const std::string& functionName,
                       const std::string& arguments) 
{
    ast_printf("\n  [调用工具: %s(%s)]\n", functionName.c_str(), arguments.c_str());
}

void ChatConsole::onToolCallResult(const std::string& /*toolCallId*/,
                      const std::string& functionName,
                      const std::string& result) 
{
    #ifdef _AST_DEBUG_CHAT_CONSOLE
    // 工具调用结果太多了，一般不打印
    ast_printf("  [工具结果: %s -> %s]\n", functionName.c_str(), result.c_str());
    ast_printf("AI: ");
    fflush(stdout);
    #endif
}

void ChatConsole::onComplete()
{
    ast_printf("\n");
}

void ChatConsole::onError(const std::string& error)
{
    ast_printf("\n  [错误: %s]\n", error.c_str());
}

AST_NAMESPACE_END
