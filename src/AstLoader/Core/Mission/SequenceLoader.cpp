///
/// @file      SequenceLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-15
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

#include "SequenceLoader.hpp"
#include "AstLoader/ScriptingToolProfileLoader.hpp"
#include "AstCore/Sequence.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "ValXMLLoader.hpp"
#include "MissionCommandLoader.hpp"
#include "AstLoader/SegmentLoader.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadSequence(const Value& dictRoot, Sequence& sequence)
{
    std::string type = dictRoot["Type"].toString();
    if(type != "Sequence" && type != "TargeterSequence")
    {
        aError("invalid type, expect 'Sequence' or 'TargeterSequence'");
        return eErrorInvalidParam;
    }


    // 加载重复次数
    {
        auto& repeatCount = dictRoot["RepeatCount"];
        if(!repeatCount.isNull())
            sequence.setRepeatCount(repeatCount);
    }
    
    // 加载序列命令
    {
        auto& dictSegmentList = dictRoot["SegmentList"];
        auto& items = dictSegmentList.items();
        std::vector<HMissionCommand> commands;
        commands.reserve(items.size());
        for(auto& item: items)
        {
            auto& name = item.first;
            const auto& dictSegment = *item.second;
            HMissionCommand command;
            errc_t rc = aLoadMissionCommand(dictSegment, command);
            if(!rc && command != nullptr)
            {
                command->setName(name);
                commands.push_back(command);
            }else
            {
                aError("failed to load mission command '%s'", name.c_str());
            }
        }
        sequence.setCommands(commands);
    }
    // 加载脚本工具配置
    {
        auto& dictScriptingTool = dictRoot["ScriptingTool"];
        if(!dictScriptingTool.isNull())
        {
            ScriptingToolProfile* tool = aNewObject<ScriptingToolProfile>(&sequence);
            sequence.setScriptingTool(tool);
            errc_t rc = aLoadScriptingToolProfile(dictScriptingTool, *tool);
            if(rc)
            {
                aError("failed to load scripting tool profile");
                return rc;
            }
        }
    }
   
    // 加载公共属性
    aLoadSegment(dictRoot, sequence);

    return 0;
}


errc_t aLoadSequence(StringView filepath, Sequence& sequence)
{
    errc_t rc;
    SharedPtr<Value> value;
    rc = aLoadValue(filepath, value);
    if(rc)  return rc;
    if(!value) return eErrorNullPtr;
    return aLoadSequence(*value, sequence);
}


AST_NAMESPACE_END
