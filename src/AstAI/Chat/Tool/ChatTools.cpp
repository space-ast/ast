///
/// @file      ChatTools.cpp
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

#include "ChatTools.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"
#include "AstAI/AgentInit.hpp"

AST_NAMESPACE_BEGIN


ChatTools::ChatTools(bool initAsDefault)
{
    if(initAsDefault)
    {
        aInitAgentTools(*this);
    }
}

std::string ChatTools::handleToolCall(const JsonValue& toolCall)
{
    auto& function = toolCall["function"];
    std::string name = function["name"].toString();
    JsonValue args = JsonValue::FromString(function["arguments"].toString());
    for(const auto& tool : tools_)
    {
        if(tool->name() == name)
        {
            return tool->call(args);
        }
    }
    aError("unsupported tool call: %s", name.c_str());
    return u8"暂时还不支持调用工具" + name;
}

JsonValue ChatTools::toJson() const
{
    JsonValue tools;
    for(const auto& tool : tools_)
    {
        JsonValue toolJson;
        toolJson["type"] = "function";
        toolJson["function"] = tool->toJson();
        tools.append(toolJson);
    }
    return tools;
}



AST_NAMESPACE_END
