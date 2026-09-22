///
/// @file      JScriptExecutor.cpp
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

#include "JScriptExecutor.hpp"
#include "AstUtil/StringView.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <activscp.h>
#endif


AST_NAMESPACE_BEGIN

JScriptExecutor::JScriptExecutor()
    : ActiveScriptExecutor(L"JScript")
{
}


#if defined _WIN32


errc_t JScriptExecutor::setVariable(StringView name, StringView value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("var " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;

    std::wstring wname = aUtf8ToWide(name);
    std::wstring wval  = aUtf8ToWide(value);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(wval.c_str());
    rc = aActiveScriptSetVariable(*global, wname, v);
    VariantClear(&v);
    return rc;
}

errc_t JScriptExecutor::setVariable(StringView name, double value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("var " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;

    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_R8;
    v.dblVal = value;
    rc = aActiveScriptSetVariable(*global, wname, v);
    VariantClear(&v);
    return rc;

}
errc_t JScriptExecutor::setVariable(StringView name, int value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("var " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;

    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_I4;
    v.lVal = value;
    rc = aActiveScriptSetVariable(*global, wname, v);
    VariantClear(&v);
    return rc;
}

errc_t JScriptExecutor::setVariable(StringView name, bool value)
{
    auto global = getGlobal();
    if (!global) return eErrorNullPtr;

    errc_t rc = execute("var " + std::string(name) + "\n");
    if(rc != eNoError)
        return rc;

    std::wstring wname = aUtf8ToWide(name);
    VARIANT v; VariantInit(&v);
    v.vt = VT_BOOL;
    // 必须写入规范的 VARIANT_TRUE/VARIANT_FALSE(-1/0)：C++ 的 bool 隐式转成 short 只会得到 1，
    // 而 1 不是规范的 VARIANT_TRUE。部分 jscript.dll（如 Server 2022 上的旧版本）会把它原样存回。
    v.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
    rc = aActiveScriptSetVariable(*global, wname, v);
    VariantClear(&v);
    return rc;
}


#endif

AST_NAMESPACE_END

