///
/// @file      chat.cxx
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-17
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


#include "AstAI/AgentSession.hpp"
#include "AstAI/ChatAgent.hpp"
#include "AstAI/AssistantAgent.hpp"
#include "AstAI/ChatConsole.hpp"
#include "AstUtil/IO.hpp"
#include <memory>

AST_USING_NAMESPACE

int main()
{
    char buffer[1024];
    ChatSession session;
    while (true)
    {
        ast_printf("我: ");
        char* str = fgets(buffer, sizeof(buffer), stdin);
        if (str && buffer[0] == '\n')
            continue;
        ChatConsole console;
        session.setAgent(std::unique_ptr<AssistantAgent>(AssistantAgent::NewDefaultAgent()));
        std::string response = session.chatStream(buffer, console);
        A_UNUSED(response);
    }
    return 0;
}