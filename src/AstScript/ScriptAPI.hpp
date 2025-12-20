///
/// @file      ScriptAPI.hpp
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
#include "ScriptParser.hpp"


AST_NAMESPACE_BEGIN

enum OpBinType{
    eAdd,       ///< 加法
    eSub,       ///< 减法
    eMul,       ///< 乘法
    eDiv,       ///< 除法
    ePow,       ///< 乘方
    eMod,       ///< 取模

    eAnd,       ///< 逻辑与
    eOr,        ///< 逻辑或
    eBitOr,     ///< 位或
    eBitAnd,    ///< 位与
    eBitXor,    ///< 位异或
    eBitLeftShift,   ///< << (逻辑/算术左移)
    eBitRightShift,  ///< >> (算术右移)
    eBitURightShift, ///< >>> (逻辑右移)  U代表Unsigned

    eElemMul,   ///< 按元素乘
    eElemDiv,   ///< 按元素除
    eElemPow,   ///< 按元素乘方
    eElemMod,   ///< 按元素取模
    eElemAnd,   ///< 按元素与
    eElemOr,    ///< 按元素或

    eLt,        ///< 小于
    eLe,        ///< 小于等于
    eEq,        ///< 等于
    eGt,        ///< 大于
    eGe,        ///< 大于等于
    eNe,        ///< 不等于

    numOpBinType,   ///< 运算符类型数量
};

enum OpAssignType{
    eAssign,        ///< 赋值     =
    eDelayAssign,   ///< 延迟赋值 :=
    eBindAssign,    ///< 绑定赋值 =&

    eAddAssign,     ///< 加法赋值
    eSubAssign,     ///< 减法赋值
    eMulAssign,     ///< 乘法赋值
    eDivAssign,     ///< 除法赋值
    eModAssign,     ///< 取模赋值
    ePowAssign,     ///< 乘方赋值

    eElemMulAssign, ///< 按元素乘赋值
    eElemDivAssign, ///< 按元素除赋值   
    eElemModAssign, ///< 按元素取模赋值
    eElemPowAssign, ///< 按元素乘方赋值
    eElemAndAssign, ///< 按元素与赋值
    eElemOrAssign,  ///< 按元素或赋值

    numOpAssignType,   ///< 运算符类型数量
};


enum OpUnaryType{
    eNot,       ///< 逻辑非
    eNeg,       ///< 负号
    eInc,       ///< 自增
    eDec,       ///< 自减
    eBitNot,    ///< 按位取反
    numOpUnaryType,   ///< 运算符类型数量
};

class Expr;         ///< 表达式
class Value;        ///< 值
class Variable;     ///< 变量
class Macro;        ///< 宏
class Function;     ///< 函数



/// @brief 创建变量对象
/// @param name 变量名
/// @param expr 变量的值，或与变量绑定的表达式
/// @param bind 是否与表达式双向绑定
/// @return 变量对象
AST_SCRIPT_API Variable* aNewVariable(StringView name, Expr* expr=nullptr, bool bind = false);
AST_SCRIPT_API Variable* aNewVariable(Expr* expr=nullptr, bool bind = false);


/// @brief 创建赋值运算符表达式对象
/// @param op 赋值运算符类型
/// @param left 左操作数
/// @param right 右操作数
/// @return 赋值运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpAssign(OpAssignType op, Expr* left, Expr* right);


/// @brief 创建二元运算符表达式对象
/// @param op 运算符类型
/// @param left 左操作数
/// @param right 右操作数
/// @return 二元运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpBin(OpBinType op, Expr* left, Expr* right);

/// @brief 创建一元运算符表达式对象
/// @param op 运算符类型
/// @param expr 操作数
/// @return 一元运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpUnary(OpUnaryType op, Expr* expr);

/// @brief 创建字符串值对象
AST_SCRIPT_CAPI Value* aNewValueString(StringView value);

/// @brief 创建整数值对象
AST_SCRIPT_CAPI Value* aNewValueInt(int value);

/// @brief 创建布尔值对象
AST_SCRIPT_CAPI Value* aNewValueBool(bool value);

/// @brief 创建双精度浮点数值对象
AST_SCRIPT_CAPI Value* aNewValueDouble(double value);

class Quantity;
/// @brief 创建数量值对象
AST_SCRIPT_CAPI Value* aNewValueQuantity(const Quantity& value);

/// @brief 获取空值对象
AST_SCRIPT_CAPI Value* aValueNull();


AST_NAMESPACE_END