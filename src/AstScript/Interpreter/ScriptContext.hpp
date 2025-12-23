///
/// @file      ScriptContext.hpp
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

#pragma once

#include "AstGlobal.h"

AST_NAMESPACE_BEGIN


class SymbolScope;          ///< 符号作用域
class StackFrame;           ///< 栈帧
class Interpreter;          ///< 解释器
class Expr;                 ///< 表达式
class Symbol;               ///< 符号



/// @brief     获取解释器指针
/// @details   ~
/// @return    Interpreter* 解释器指针
AST_SCRIPT_CAPI Interpreter* aScriptContext_GetInterpreter();


/// @brief     设置解释器指针
/// @details   ~
/// @param     interpreter 解释器指针
AST_SCRIPT_CAPI void aScriptContext_SetInterpreter(Interpreter* interpreter);


/// @brief     移除解释器指针
/// @details   ~
/// @param     interpreter 解释器指针
AST_SCRIPT_CAPI void aScriptContext_RemoveInterpreter(Interpreter* interpreter);


/// @brief     获取当前符号作用域
/// @details   ~
/// @return    SymbolScope* 符号作用域指针
AST_SCRIPT_CAPI SymbolScope* aScriptContext_CurrentSymbolScope();


/// @brief     查找符号
/// @details   ~
/// @param     name 符号名
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScriptContext_FindSymbol(StringView name);


/// @brief     查找符号
/// @details   ~
/// @param     symbol 符号指针
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScriptContext_FindSymbol(const Symbol* symbol);



/// @brief     解析符号
/// @details   ~
/// @param     name 符号名
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScriptContext_ResolveSymbol(StringView name);


/// @brief     解析符号
/// @details   ~
/// @param     symbol 符号
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScriptContext_ResolveSymbol(const Symbol* symbol);



AST_NAMESPACE_END
