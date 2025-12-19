///
/// @file      AstScriptAPI.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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


AST_NAMESPACE_BEGIN

enum OpBinType{
    eAdd,       ///< 加法
    eSub,       ///< 减法
    eMul,       ///< 乘法
    eDiv,       ///< 除法
    eMod,       ///< 取模
};

class Expr;         ///< 表达式
class Value;        ///< 值
class Macro;        ///< 宏
class Function;     ///< 函数


/// @brief 解析脚本表达式
/// @param script 脚本文本
/// @return 解析得到的表达式对象
AST_SCRIPT_CAPI Expr* aParseExpr(StringView script);


/// @brief 创建二元运算符表达式对象
/// @param op 运算符类型
/// @param left 左操作数
/// @param right 右操作数
/// @return 二元运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpBin(OpBinType op, Expr* left, Expr* right);


/// @brief 创建字符串值对象
AST_SCRIPT_CAPI Value* aNewValueString(StringView value);

/// @brief 创建整数值对象
AST_SCRIPT_CAPI Value* aNewValueInt(int value);

/// @brief 创建布尔值对象
AST_SCRIPT_CAPI Value* aNewValueBool(bool value);

/// @brief 创建双精度浮点数值对象
AST_SCRIPT_CAPI Value* aNewValueDouble(double value);


AST_NAMESPACE_END
