///
/// @file      VBScriptExecutor.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-29
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

#include "VBScriptExecutor.hpp"
#include "AstUtil/Encode.hpp"
#include "AstUtil/ParseFormat.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <activscp.h>
#endif

AST_NAMESPACE_BEGIN

VBScriptExecutor::VBScriptExecutor()
    : ActiveScriptExecutor(L"VBScript")
{
}

#if defined _WIN32

static errc_t setVBVariableByLiteral(VBScriptExecutor& exec, StringView name, StringView literal)
{
    std::string varname = std::string(name);
    std::string cmd = "Dim " + varname + "\n" + varname + " = " + std::string(literal) + "\n";
    return exec.execute(cmd);
}


errc_t VBScriptExecutor::setVariable(StringView name, StringView value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("Dim " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;
    // 字符串在使用Dim声明后，使用setScriptVariable设置值，避免字符串转义等复杂问题
    std::wstring wname = aUtf8ToWide(name);
    std::wstring wval  = aUtf8ToWide(value);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(wval.c_str());
    rc = aActiveScriptSetVariable(*global, wname, v);
    VariantClear(&v);
    return rc;
}


errc_t VBScriptExecutor::setVariable(StringView name, double value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("Dim " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;
    // 浮点数在使用Dim声明后，使用setScriptVariable设置值，避免nan、inf等边界问题
    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_R8;
    v.dblVal = value;
    return aActiveScriptSetVariable(*global, wname, v);
}

errc_t VBScriptExecutor::setVariable(StringView name, int value)
{
    return setVBVariableByLiteral(*this, name, aFormatInt(value));
}
errc_t VBScriptExecutor::setVariable(StringView name, bool value)
{
    const char* boolLiteral = value ? "True" : "False";
    return setVBVariableByLiteral(*this, name, boolLiteral);
}

#endif


AST_NAMESPACE_END
