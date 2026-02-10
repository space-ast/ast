///
/// @file      Interpreter.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-19
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

#pragma once

#include "AstGlobal.h"
#include "SymbolTable.hpp"
#include "SymbolScope.hpp"
#include "CallStack.hpp"
#include "ScriptContext.hpp"
#include <string>

AST_NAMESPACE_BEGIN


/// @brief 解释器
/// @details
/// 解释器用于解释执行脚本中的代码。
/// @ingroup Script
class AST_SCRIPT_API Interpreter
{
public:
    Interpreter() = default;
    ~Interpreter();

    /// @brief 获取当前作用域
    /// @details
    /// 当前作用域是解释器当前正在执行的代码所在的作用域。
    /// @return 当前作用域
    SymbolScope* currentScope() { return &symbolScope_; }
    
    /// @brief 解释执行代码
    /// @details
    /// 解释执行代码时，会将代码解析为抽象语法树（AST），
    /// 并根据符号表和调用栈执行代码。
    /// @param code 要解释执行的代码
    void interpret(StringView code);


    /// @brief 获取错误字符串
    /// @details
    /// 获取解释器执行代码时产生的错误字符串。
    /// @return 错误字符串
    const std::string& errString() const { return errString_; }


    /// @brief 设置错误字符串
    /// @details
    /// 设置解释器执行代码时产生的错误字符串。
    /// @param err 错误字符串
    void setErrString(StringView err) { errString_ = err.to_string(); }


    /// @brief 清除错误字符串
    /// @details
    /// 清除解释器执行代码时产生的错误字符串。
    void clearErrString() { errString_.clear(); }

protected:
    SymbolScope symbolScope_;       ///< 当前作用域
    std::string errString_;        ///< 错误字符串
};

/// @brief 解释器上下文守卫
/// @details
/// 解释器上下文守卫用于在解释器执行代码时，自动设置和移除当前解释器。
/// @todo 暂时不支持子解释器，只能区分全局解释器和当前解释器。
class InterpreterContext {
public:
    InterpreterContext(Interpreter& interpreter)
        : InterpreterContext(&interpreter)
    {
    }
    InterpreterContext(Interpreter* interpreter)
        : interpreter_(interpreter)
    {
        aScript_SetInterpreter(interpreter);
    }
    ~InterpreterContext()
    {
        aScript_RemoveInterpreter(interpreter_);
    }
private:
    Interpreter* interpreter_;
};



AST_NAMESPACE_END