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
/// @return    Interpreter* 解释器指针
AST_SCRIPT_CAPI Interpreter* aScript_GetInterpreter();


/// @brief     设置解释器指针
/// @param     interpreter 解释器指针
AST_SCRIPT_CAPI void aScript_SetInterpreter(Interpreter* interpreter);


/// @brief     移除解释器指针
/// @details   如果解释器指针与传入指针相同，则移除解释器指针，然后将解释器上下文切换为全局解释器。
/// @param     interpreter 解释器指针
AST_SCRIPT_CAPI void aScript_RemoveInterpreter(Interpreter* interpreter);


/// @brief     获取当前符号作用域
/// @details   ~
/// @return    SymbolScope* 符号作用域指针
AST_SCRIPT_CAPI SymbolScope* aScript_CurrentSymbolScope();


/// @brief     查找符号
/// @details   ~
/// @param     name 符号名
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScript_FindSymbol(StringView name);


/// @brief     查找符号
/// @details   ~
/// @param     symbol 符号指针
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScript_FindSymbol(const Symbol* symbol);



/// @brief     解析符号
/// @details   ~
/// @param     name 符号名
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScript_ResolveSymbol(StringView name);


/// @brief     解析符号
/// @details   ~
/// @param     symbol 符号
/// @return    Expr* 表达式指针
AST_SCRIPT_API Expr* aScript_ResolveSymbol(const Symbol* symbol);


/// @brief     设置错误字符串
/// @details   ~
/// @param     err 错误字符串
AST_SCRIPT_CAPI void aScript_SetErrString(StringView err);


/// @brief     格式化错误字符串
/// @details   ~
/// @param     fmt 格式化字符串
/// @param     args 可变参数列表
AST_SCRIPT_CAPI void aScript_FormartErrStringV(StringView fmt, va_list args);


/// @brief     格式化错误字符串
/// @details   ~
/// @param     fmt 格式化字符串
/// @param     ... 可变参数
AST_SCRIPT_CAPI void aScript_FormatErrString(StringView fmt, ...);



AST_NAMESPACE_END
