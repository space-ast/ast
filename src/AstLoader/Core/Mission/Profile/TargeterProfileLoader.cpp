///
/// @file      TargeterProfileLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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

#include "TargeterProfileLoader.hpp"
#include "AstLoader/DifferentialCorrectorProfileLoader.hpp"
#include "AstLoader/ScriptingToolProfileLoader.hpp"
#include "AstCore/DifferentialCorrectorProfile.hpp"
#include "AstCore/ScriptingToolProfile.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <string>

AST_NAMESPACE_BEGIN

errc_t aLoadTargeterProfile(const Value& value, SharedPtr<TargeterProfile>& profile, Object* scope)
{
    std::string type = value["Type"];
    if(type == "DifferentialCorrector")
    {
        auto differentialCorrector = aNewObject<DifferentialCorrectorProfile>(scope);
        profile = differentialCorrector;
        return aLoadDifferentialCorrectorProfile(value, *differentialCorrector);
    }
    else if(type == "ScriptingTool")
    {

        auto scriptingTool = aNewObject<ScriptingToolProfile>(scope);
        profile = scriptingTool;
        return aLoadScriptingToolProfile(value, *scriptingTool);
    }
    else
    {
        aWarning("unsupported profile type '%s'", type.c_str());
    }
    return eNoError;
}


AST_NAMESPACE_END
