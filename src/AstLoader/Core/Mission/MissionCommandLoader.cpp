///
/// @file      MissionCommandLoader.cpp
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

#include "MissionCommandLoader.hpp"
#include "AstUtil/StringView.hpp"
#include "ValXMLLoader.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/End.hpp"
#include "AstCore/Continue.hpp"
#include "AstCore/Break.hpp"
#include "AstCore/Return.hpp"
#include "AstCore/Stop.hpp"
#include "AstCore/TargeterSequence.hpp"


AST_NAMESPACE_BEGIN


errc_t aLoadMissionCommand(const Value& value, SharedPtr<MissionCommand>& missionCommand)
{
    std::string type = value["Type"];
    if(type == "Sequence")
    {
        SharedPtr<Sequence> sequence = new Sequence();
        missionCommand = sequence;
        return aLoadSequence(value, *sequence);
    }else if(type == "InitialState")
    {
        SharedPtr<InitialState> initialState = new InitialState();
        missionCommand = initialState;
        return aLoadInitialState(value, *initialState);
    }else if(type == "Maneuver")
    {
        SharedPtr<Maneuver> maneuver = new Maneuver();
        missionCommand = maneuver;
        return aLoadManeuver(value, *maneuver);
    }else if(type == "Propagate")
    {
        SharedPtr<Propagate> propagate = new Propagate();
        missionCommand = propagate;
        return aLoadPropagate(value, *propagate);
    }
    else if(type == "End")
    {
        SharedPtr<End> end = new End();
        missionCommand = end;
        // End命令仅仅是标识结束，不执行任何操作，所以应该不用加载任何参数
        // return aLoadEnd(value, *end);
        return eNoError;
    }
    else if(type == "Stop")
    {
        SharedPtr<Stop> stop = new Stop();
        missionCommand = stop;
        // return aLoadStop(value, *stop);
        return eNoError;
    }
    else if(type == "Return")
    {
        SharedPtr<Return> ret = new Return();
        missionCommand = ret;
        return aLoadReturn(value, *ret);
    }
    else if(type == "TargeterSequence")
    {
        SharedPtr<TargeterSequence> targeterSequence = new TargeterSequence();
        missionCommand = targeterSequence;
        return aLoadTargeterSequence(value, *targeterSequence);
    }
    else
    {
        aError("unsupport object type '%.*s'", type.size(), type.data());
    }
    return -1;
}

errc_t aLoadMissionCommand(StringView filePath, SharedPtr<MissionCommand>& missionCommand)
{
    errc_t rc;
    SharedPtr<Value> value;
    rc = aLoadValue(filePath, value);
    if(rc)  return rc;
    if(!value) return eErrorNullPtr;
    return aLoadMissionCommand(*value, missionCommand);
}

AST_NAMESPACE_END
