///
/// @file      ScriptingToolProfileLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-30
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

#include "ScriptingToolProfileLoader.hpp"
#include "VariableListLoader.hpp"
#include "AstCore/ScriptingToolProfile.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

errc_t aLoadScriptingToolProfile(const Value& value, ScriptingToolProfile& profile)
{
    // 检查类型
    {
        std::string type = value["Type"];
        if(type != "ScriptingTool")
        {
            aError("invalid type: %s", type.c_str());
            return -1;
        }
    }

    // 读取脚本类型
    std::string languageStr = value["Language"];
    EScriptLanguage language;
    if(languageStr == "JScript")
        language = EScriptLanguage::eJScript;
    else if(languageStr == "VBScript")
        language = EScriptLanguage::eVBScript;
    else if(languageStr == "MATLAB")
        language = EScriptLanguage::eMATLAB;
    else if(languageStr == "Python")
        language = EScriptLanguage::ePython;
    else if(languageStr == "Julia")
        language = EScriptLanguage::eJulia;
    else if(languageStr == "Lua")
        language = EScriptLanguage::eLua;
    else
    {
        language = EScriptLanguage::ePython;
        aWarning("invalid language: %s, use 'Python' by default", languageStr.c_str());
    }

    profile.setLanguage(language);

    // 读取脚本内容
    profile.setScriptStatements(value["ScriptStatements"].toString());
    errc_t rc;

    // 将ScriptingToolProfile的父作用域作为加载时的作用域
    Object* scope = profile.getParentScope();

    // 读取属性列表
    rc = aLoadAttributes(value["Attributes"], profile.attributes(), scope);
    if(rc != 0) aWarning("failed to load attributes");

    // 读取参数列表
    rc = aLoadParameters(value["Parameters"], profile.parameters());
    if(rc != 0) aWarning("failed to load parameters");
    
    // 读取计算对象列表
    rc = aLoadCalcObjects(value["CalcObjects"], profile.calcObjects(), scope);
    if(rc != 0) aWarning("failed to load calc objects");
    
    return 0;
}

AST_NAMESPACE_END
