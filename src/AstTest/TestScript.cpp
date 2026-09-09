///
/// @file      TestScript.cpp
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

#include "TestScript.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/Interpreter.hpp"


AST_NAMESPACE_BEGIN

errc_t aTestScriptParse(StringView script)
{
    Expr* expr1 = aParseExpr(script);
    if(!expr1)
    {
        aError(_("解析输入表达式失败: %s"), script.data());
        return eErrorInvalidParam;
    }
    std::string exprStr1 = aFormatExpr(expr1);
    Expr* expr2 = aParseExpr(exprStr1);
    if(!expr2)
    {
        aError(_("解析格式化后的表达式失败: %s"), exprStr1.c_str());
        return eErrorInvalidParam;
    }
    std::string exprStr2 = aFormatExpr(expr2);
    if(exprStr1 != exprStr2)
    {
        aError(_("第一个格式化后的表达式与第二个格式化后的表达式不相等"));
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptSyntaxError(StringView str)
{
    Expr* expr = aParseExpr(str);
    if(expr)
    {
        aError(_("期望出现语法错误"));
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptEvalRuntimeError(StringView str)
{
    Expr* expr = aParseExpr(str);
    if(!expr)
    {
        aError(_("解析表达式时出现意外错误: '%.*s'"), str.size(), str.data());
        return eErrorInvalidParam;
    }
    SharedPtr<Value> value = aEvalExpr(expr);
    if(value)
    {
        aError(_("期望出现运行时错误"));
        return eErrorInvalidParam;
    }
    return eNoError;
}

errc_t aTestScriptEvalBool(StringView str, bool expectValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError(_("求值 bool 表达式失败: '%.*s'"), str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsBool(value.get()))
        {
            aError(_("期望得到 bool 值"));
            return eErrorInvalidType;
        }
        bool actualValue = aValueUnboxBool(value.get());
        if(actualValue != expectValue)
        {
            aError(_("期望得到 bool 值 %d，实际得到 %d"), expectValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalInt(StringView str, int expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError(_("求值 int 表达式失败: '%.*s'"), str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsInt(value.get()))
        {
            aError(_("期望得到 int 值"));
            return eErrorInvalidType;
        }
        int actualValue = aValueUnboxInt(value.get());
        if(actualValue != expectedValue)
        {
            aError(_("期望得到 int 值 %d，实际得到 %d"), expectedValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}


errc_t aTestScriptEvalDouble(StringView str, double expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError(_("求值 double 表达式失败: '%.*s'"), str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsDouble(value.get()))
        {
            aError(_("期望得到 double 值"));
            return eErrorInvalidType;
        }
        double actualValue = aValueUnboxDouble(value.get());
        if(actualValue != expectedValue)
        {
            aError(_("期望得到 double 值 %f，实际得到 %f"), expectedValue, actualValue);
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalString(StringView str, StringView expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError(_("求值 string 表达式失败: %s"), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(auto valString = dynamic_cast<ValString*>(value.get()))
        {
            if(valString->value() != std::string(expectedValue))
            {
                aError(_("期望得到 string 值 %s，实际得到 %s"), expectedValue.data(), valString->value().c_str());
                return eErrorInvalidValue;
            }
        }else{
            aError(_("期望得到 string 值"));
            return eErrorInvalidType;
        }
    }
    return eNoError;
}

errc_t aTestScriptEvalQuantity(StringView str, const Quantity &expectedValue)
{
    SharedPtr<Value> value = aEval(str);
    if(!value)
    {
        aError(_("求值 quantity 表达式失败: '%.*s'"), str.size(), str.data());
        return eErrorNullPtr;
    }
    if(value.get()){
        if(!aValueIsQuantity(value.get()))
        {
            aError(_("期望得到 quantity 值"));
            return eErrorInvalidType;
        }
        Quantity actualValue = aValueUnboxQuantity(value.get());
        if(actualValue != expectedValue)
        {
            aError(_("期望得到 quantity 值 %s，实际得到 %s"), expectedValue.toString().c_str(), actualValue.toString().c_str());
            return eErrorInvalidValue;
        }
    }
    return eNoError;
}

AST_NAMESPACE_END
