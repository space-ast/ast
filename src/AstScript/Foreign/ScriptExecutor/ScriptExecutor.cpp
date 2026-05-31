///
/// @file      ScriptExecutor.cpp
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

#include "AstScript/Variable.hpp"
#include "AstScript/Value.hpp"
#include "ScriptExecutor.hpp"
#include "VBScriptExecutor.hpp"
#include "JScriptExecutor.hpp"
#include "PythonExecutor.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

// #define AST_DEBUG_SCRIPT_EXECUTOR


errc_t ScriptExecutor::setVariable(Variable* var)
{
    if(!var) return eErrorNullInput;
    SharedPtr<Value> value = var->eval();
    if(!value)
    {
        aError("failed to evaluate variable: '%s'", var->getName().c_str());
        return eErrorInvalidParam;
    }

    #ifdef AST_DEBUG_SCRIPT_EXECUTOR
    aInfo("setVariable: `%s = %s`", var->getName().c_str(), value->getExpression().c_str());
    #endif

    if(value->isBool())
    {
        bool val = value->toBool();
        return this->setVariable(var->getName(), val);
    }
    else if(value->isInt())
    {
        int val = value->toInt();
        return this->setVariable(var->getName(), val);
    }
    else if(value->isDouble())
    {
        double val = value->toDouble();
        return this->setVariable(var->getName(), val);
    }
    else if(value->isQuantity())
    {
        double val = value->toDouble();
        return this->setVariable(var->getName(), val);
    }
    else // if(value->isString())
    {
        std::string val = value->toString();
        return this->setVariable(var->getName(), val);
    }
    return eNoError;
}


errc_t ScriptExecutor::getVariable(Variable* var)
{
    if(!var) return eErrorNullInput;
    // 先尝试获取 double 类型的变量
    {
        double value;
        errc_t rc = this->getVariable(var->getName(), value);
        if(rc == eNoError)
        {
            #ifdef AST_DEBUG_SCRIPT_EXECUTOR
            aInfo("getVariable: `%s = %lf`", var->getName().c_str(), value);
            #endif

            return var->setValue(aNewValueDouble(value));
        }
    }
    // 如果double类型转换失败，则尝试获取string类型
    {
        std::string value;
        errc_t rc = this->getVariable(var->getName(), value);
        if(rc == eNoError)
        {
            #ifdef AST_DEBUG_SCRIPT_EXECUTOR
            aInfo("getVariable: `%s = %s`", var->getName().c_str(), value.c_str());
            #endif
            
            return var->setValue(aNewValueString(value));
        }
    }
    aError("failed to get variable: '%s'", var->getName().c_str());
    return eErrorInvalidParam;
}


errc_t ScriptExecutor::evaluate(StringView expression, ScriptResult* resultOut)
{
    // 一些脚本语言的语句执行也有返回值，此时能直接通过执行语句来计算表达式的值
    return this->execute(expression, resultOut);
}

errc_t ScriptExecutor::execute(StringView script, std::string* error)
{
    if(!error)
    {
        return this->execute(script);
    }
    else{
        ScriptResult result;
        errc_t rc = execute(script, &result);
        *error = std::move(result.error_);
        return rc;
    }
}


std::string toString(EScriptLanguage type)
{
    switch(type)
    {
    case EScriptLanguage::ePython:
        return "Python";
    case EScriptLanguage::eMATLAB:
        return "MATLAB";
    case EScriptLanguage::eJavaScript:
        return "JavaScript";
    case EScriptLanguage::eJScript:
        return "JScript";
    case EScriptLanguage::eVBScript:
        return "VBScript";
    case EScriptLanguage::eJulia:
        return "Julia";
    case EScriptLanguage::eLua:
        return "Lua";
    case EScriptLanguage::eAstScript:
        return "AstScript";
    default:
    {
        aError("unsupported script type");
        return "Unknown";
    }
    }
}

ScriptExecutor* aNewScriptExecutor(EScriptLanguage type)
{
    switch(type)
    {
#ifdef _WIN32  // JScript 和 VBScript 只在 Windows 上支持
    case EScriptLanguage::eVBScript:
        return new VBScriptExecutor();
    case EScriptLanguage::eJScript:
        return new JScriptExecutor();
#endif
    case EScriptLanguage::ePython:
        return new PythonExecutor();
    default:
    {
        aError("unsupported script executor for language: '%s'", toString(type).c_str());
        return nullptr;
    }
    }
}


AST_NAMESPACE_END
