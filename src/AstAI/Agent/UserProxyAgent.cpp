///
/// @file      UserProxyAgent.cpp
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

#include "UserProxyAgent.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/StringUtil.hpp"
#include "AstAI/ChatMessages.hpp"
#include <iostream>

AST_NAMESPACE_BEGIN

errc_t UserProxyAgent::run(ChatMessages& messages)
{
    // 1. 显示输入提示
    if (!prompt_.empty())
    {
        ast_printf("%s", prompt_.c_str());
        fflush(stdout);
    }

    // 2. 读取用户输入
    std::string input;
    if (!std::getline(std::cin, input))
    {
        // EOF 或读取错误 → 视为退出
        messages.addUserMessage("EXIT");
        return 0;
    }

    // 3. 检查退出关键词（大小写不敏感）
    if (!exitKeywords_.empty())
    {
        for (const auto& kw : exitKeywords_)
        {
            if (aEqualsIgnoreCase(input, kw))
            {
                messages.addUserMessage("EXIT");
                return 0;
            }
        }
    }

    // 4. 正常输入 → 追加为用户消息
    messages.addUserMessage(input);
    return 0;
}


errc_t UserProxyAgent::runStream(ChatMessages& messages, ChatEventHandler& handler)
{
    A_UNUSED(handler);
    return run(messages);
}


AST_NAMESPACE_END
