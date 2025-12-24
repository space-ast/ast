///
/// @file      ScriptContext.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-23
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "ScriptContext.hpp"
#include "AstScript/Symbol.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/IO.hpp"
#include "Interpreter.hpp"  
#include <memory>           // for std::unique_ptr

AST_NAMESPACE_BEGIN


namespace{

/// @brief     脚本上下文类
/// @details   ~
class ScriptContext{
public:
    /// @brief     脚本上下文构造函数
    /// @details   ~
    ScriptContext()
        : interpreter_(nullptr){};


    ScriptContext(Interpreter* interpreter)
        : interpreter_(interpreter){};

    /// @brief     设置解释器指针
    /// @details   ~
    /// @param     interpreter 解释器指针
    void setInterpreter(Interpreter* interpreter)
    {
        interpreter_ = interpreter;
    }
    /// @brief     获取解释器指针
    /// @details   ~
    /// @return    Interpreter* 解释器指针
    Interpreter* interpreter() const
    {
        return interpreter_;
    }
protected:
    Interpreter* interpreter_;
};

}


static Interpreter gInterpreter{};                                   ///< 全局解释器
A_THREAD_LOCAL ScriptContext tCurrentScriptContext{&gInterpreter};   ///< 当前脚本上下文


Interpreter *aScript_GetInterpreter()
{
    return tCurrentScriptContext.interpreter();
}


void aScript_SetInterpreter(Interpreter* interpreter)
{
    tCurrentScriptContext.setInterpreter(interpreter);
}

void aScript_RemoveInterpreter(Interpreter* interpreter)
{
    if(tCurrentScriptContext.interpreter() == interpreter)
    {
        // 移除解释器后，将当前解释器设置为全局解释器
        // @todo: 目前还不支持子解释器
        tCurrentScriptContext.setInterpreter(&gInterpreter);
    }
}

SymbolScope *aScript_CurrentSymbolScope()
{
    auto interpreter = aScript_GetInterpreter();
    if(interpreter)
    {
        return interpreter->currentScope();
    }
    return nullptr;
}

Expr *aScript_FindSymbol(StringView name)
{
    auto symbolScope = aScript_CurrentSymbolScope();
    if(A_UNLIKELY(!symbolScope))
    {
        aError("symbol scope is null");
        return nullptr;
    }
    return symbolScope->findSymbol(name);
}

Expr *aScript_FindSymbol(const Symbol *symbol)
{
    if(A_UNLIKELY(!symbol))
    {
        aError("symbol is null");
        return nullptr;
    }
    return aScript_FindSymbol(symbol->name());
}

Expr *aScript_ResolveSymbol(StringView name)
{
    auto symbolScope = aScript_CurrentSymbolScope();
    if(A_UNLIKELY(!symbolScope))
    {
        aError("symbol scope is null");
        return nullptr;
    }
    return symbolScope->resolveSymbol(name);
}

Expr *aScript_ResolveSymbol(const Symbol *symbol)
{
    if(A_UNLIKELY(!symbol))
    {
        aError("symbol is null");
        return nullptr;
    }
    return aScript_ResolveSymbol(symbol->name());
}

void aScript_SetErrString(StringView err)
{
    
}

void aScript_FormartErrStringV(StringView fmt, va_list args)
{
    char buf[1024] = {0};
    vsnprintf(buf, sizeof(buf), fmt.data(), args);
    aScript_SetErrString(buf);
}

void aScript_FormatErrString(StringView fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    aScript_FormartErrStringV(fmt, args);
    va_end(args);
}

AST_NAMESPACE_END

