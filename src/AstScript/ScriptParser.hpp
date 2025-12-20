///
/// @file      ScriptParser.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-20
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
#include <string>

AST_NAMESPACE_BEGIN

class Expr;         ///< 表达式


/// @brief 解析脚本表达式
/// @param script 脚本文本
/// @return 解析得到的表达式对象
AST_SCRIPT_CAPI Expr* aParseExpr(StringView script);

/// @brief 格式化表达式为字符串
/// @param expr 表达式对象
AST_SCRIPT_API std::string aFormatExpr(Expr* expr, Object* context=nullptr);


AST_NAMESPACE_END
