///
/// @file      Agent.cpp
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

#include "AgentInit.hpp"
#include "AstAI/ChatTools.hpp"
#include "AstAI/ChatSession.hpp"
#include "AstAI/AgentUtil.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/JsonValue.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


const char* defaultSystemPrompt = u8R"(
你是你个专业工程师，擅长航天任务设计与分析，能够熟练使用航天任务设计软件，你需要协助用户使用航天进行任务设计与分析。
更具体一点的描述:
- 正在使用的软件是一个航天任务设计软件，类似于STK软件
- 你正在协助用户建立一个航天任务分析场景
- 场景中的对象包括：卫星、地面站、传感器等
要求:
- 如果可能，建议单次同时调用多个工具函数，提高调用效率和响应速度
)";



void aInitAgentTools(ChatTools& tools)
{
    tools.clearTools();
    // 初始化默认工具
    {
        auto tool = tools.addTool(aFindClasses);
        tool->setName("find_classes");
        tool->setDescription(u8"查找软件支持的所有对象类型");
        tool->setParameters(aFindClassesParamSchema());
    }
    {
        auto tool = tools.addTool(aCreateObject);
        tool->setName("create_object");
        tool->setDescription(u8"创建一个新的对象，如果创建成功，将会返回所创建对象的详细属性信息（与get_object_attributes函数接口返回的属性信息相同）");
        tool->setParameters(aCreateObjectParamSchema());
    }
    {
        auto tool = tools.addTool(static_cast<FCallTool>(aFindObjects));
        tool->setName("find_objects");
        tool->setDescription(u8"查找当前设计场景中的对象");
        tool->setParameters(aFindObjectsParamSchema());
    }
    {
        auto tool = tools.addTool(aGetObjectAttributes);
        tool->setName("get_object_attributes");
        tool->setDescription(u8"获取指定对象的详细属性信息");
        tool->setParameters(aGetObjectAttributesParamSchema());
    }
    {
        auto tool = tools.addTool(aSetObjectAttribute);
        tool->setName("set_object_attribute");
        tool->setDescription(u8"设置指定对象的属性值");
        tool->setParameters(aSetObjectAttributeParamSchema());
    }
    if(0)
    {
        auto tool = tools.addTool(aShowEditDialog);
        tool->setName("show_edit_dialog");
        tool->setDescription(u8"显示指定对象的编辑界面");
        tool->setParameters(aShowEditDialogParamSchema());
    }
}

void aInitAgentSession(ChatSession& session)
{
    session.setSystemPrompt(aAgentSystemPrompt());
    aInitAgentTools(session.tools());
}

const char* aAgentSystemPrompt()
{
    return defaultSystemPrompt;
}


ChatTools& aAgentTools()
{
    static ChatTools tools(true);
    return tools;
}

JsonValue aAgentToolsJson()
{
    return aAgentTools().toJson();
}


std::string aAgentToolsJsonStr()
{
    return aAgentToolsJson().toJsonString();
}


std::string aAgentToolsHandleToolCall(const JsonValue& toolCall)
{
    return aAgentTools().handleToolCall(toolCall);
}

std::string aAgentToolsHandleToolCall(const std::string& toolCallStr)
{
    return aAgentTools().handleToolCall(JsonValue::FromString(toolCallStr));
}



AST_NAMESPACE_END
