///
/// @file      ScriptingTool.cpp
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

#include "ScriptingToolProfile.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/ScriptExecutor.hpp"

AST_NAMESPACE_BEGIN

static void setVariableList(ScriptExecutor& executor, const VariableList& variableList)
{
    for(auto& var : variableList)
    {
        errc_t rc = executor.setVariable(var);
        if(rc)
            aWarning("failed to set variable: '%s'", var->getName().c_str());
    }
}

static void getVariableList(ScriptExecutor& executor, VariableList& variableList)
{
    for(auto& var : variableList)
    {
        errc_t rc = executor.getVariable(var);
        if(rc)
            aWarning("failed to get variable: '%s'", var->getName().c_str());
    }
}

errc_t ScriptingToolProfile::execute()
{
    ScopedPtr<ScriptExecutor> executor = aNewScriptExecutor(language_);
    if(!executor)
    {
        aError("failed to create script executor for language: '%s'", toString(language_).c_str());
        return eErrorInvalidParam;
    }
    errc_t rc;
    
    // 初始化脚本执行器
    rc = executor->initialize();
    if(rc)
    {
        aError("failed to initialize script executor");
        return rc;
    }

    // 设置全局变量值
    setVariableList(*executor, parameters_);
    setVariableList(*executor, attributes_);
    setVariableList(*executor, calcObjects_);

    // 执行脚本
    std::string errorMessage;
    rc = executor->execute(scriptStatements_, &errorMessage);
    if(rc)
    {
        aError("failed to execute script: %s", errorMessage.c_str());
        return rc;
    }
    // 获取全局变量值
    getVariableList(*executor, attributes_);
    // 这里只需要同步 attributes_ 中的变量值吧?
    return eNoError;
}

Variable* ScriptingToolProfile::getParameter(StringView name) const
{
    for(auto& var : parameters_)
    {
        if(name == var->getName())
            return var;
    }
    return nullptr;
}


ScriptingToolProfile* ScriptingToolProfile::New()
{
    return new ScriptingToolProfile();
}

AST_NAMESPACE_END
