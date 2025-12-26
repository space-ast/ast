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
#include "AstScript/ScriptParser.hpp"
#include "AstScript/ExprVisitor.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

#define AST_EXPR(CLASS) \
    AST_OBJECT(CLASS)\
    void accept(ExprVisitor& visitor) override{visitor.visit(*this);};

enum EOpBinType{
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

    numOpBinType,                 ///< 运算符类型数量
    invalidOpBin=numOpBinType,   ///< 无效运算符

    // 下面的枚举为软件内部使用，不要直接调用
    eCatVertical,       ///< 垂直拼接
    eCatHorizontal,     ///< 水平拼接
};

enum EOpAssignType{
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


enum EOpUnaryType{
    eNot,       ///< 逻辑非
    eNeg,       ///< 负号
    eInc,       ///< 自增
    eDec,       ///< 自减
    eBitNot,    ///< 按位取反
    numOpUnaryType,   ///< 运算符类型数量

    // 下面的枚举为软件内部使用，不要直接调用
    eIterate,    ///< 迭代器
};

class Expr;         ///< 表达式
class Value;        ///< 值
class Variable;     ///< 变量
class Macro;        ///< 宏
class Function;     ///< 函数
class Symbol;       ///< 符号


/// @brief 创建变量对象
/// @param name 变量名
/// @param expr 变量的值，或与变量绑定的表达式
/// @param bind 是否与表达式双向绑定
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 变量对象
AST_SCRIPT_API Variable* aNewVariable(StringView name, Expr* expr=nullptr, bool bind = false);
AST_SCRIPT_API Variable* aNewVariable(Expr* expr=nullptr, bool bind = false);

/// @brief 创建符号对象
/// @param name 符号名
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 符号对象
AST_SCRIPT_API Symbol* aNewSymbol(StringView name);

/// @brief 创建赋值运算符表达式对象
/// @param op 赋值运算符类型
/// @param left 左操作数
/// @param right 右操作数
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 赋值运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpAssign(EOpAssignType op, Expr* left, Expr* right);


/// @brief 创建二元运算符表达式对象
/// @param op 运算符类型
/// @param left 左操作数
/// @param right 右操作数
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 二元运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpBin(EOpBinType op, Expr* left, Expr* right);

/// @brief 创建一元运算符表达式对象
/// @param op 运算符类型
/// @param expr 操作数
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 一元运算符表达式对象
AST_SCRIPT_CAPI Expr* aNewOpUnary(EOpUnaryType op, Expr* expr);

/// @brief 创建条件表达式对象
/// @param condition 条件表达式
/// @param thenExpr 条件为真时的表达式
/// @param elseExpr 条件为假时的表达式
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 条件表达式对象
AST_SCRIPT_CAPI Expr* aNewExprCondition(Expr* condition, Expr* thenExpr, Expr* elseExpr=nullptr);


/// @brief 创建范围表达式对象
/// @param start 范围的起始值表达式
/// @param stop 范围的结束值表达式
/// @param step 范围的步长表达式，可选，默认值为1
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
/// @return 范围表达式对象
AST_SCRIPT_CAPI Expr* aNewExprRange(Expr* start, Expr* stop, Expr* step=nullptr);


/// @brief 创建字符串值对象
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
AST_SCRIPT_CAPI Value* aNewValueString(StringView value);

/// @brief 创建整数值对象
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
AST_SCRIPT_CAPI Value* aNewValueInt(int value);

/// @brief 创建布尔值对象
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
AST_SCRIPT_CAPI Value* aNewValueBool(bool value);

/// @brief 创建双精度浮点数值对象
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
AST_SCRIPT_CAPI Value* aNewValueDouble(double value);

class Quantity;


/// @brief 创建数量值对象
/// @warning 返回的指针由调用者拥有，需要管理其生命周期
AST_SCRIPT_CAPI Value* aNewValueQuantity(const Quantity& value);

/// @brief 获取空值对象
AST_SCRIPT_CAPI Value* aValueNull();

/// @brief 解析脚本表达式
/// @param script 脚本文本
/// @return 解析得到的表达式对象
AST_SCRIPT_CAPI Expr* aParseExpr(StringView script);


/// @brief 执行脚本表达式
/// @param script 脚本文本
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 表达式执行结果
AST_SCRIPT_CAPI Value* aEval(StringView script);

/// @brief 执行表达式
/// @param expr 表达式对象
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 表达式执行结果
AST_SCRIPT_CAPI Value* aEvalExpr(Expr* expr);


/// @brief 判断值对象是否为布尔值
/// @param value 值对象
/// @return 是否为布尔值
AST_SCRIPT_CAPI bool aValueIsBool(Value* value);

/// @brief 判断值对象是否为双精度浮点数值
/// @param value 值对象
/// @return 是否为双精度浮点数值
AST_SCRIPT_CAPI bool aValueIsDouble(Value* value);

/// @brief 判断值对象是否为整数值
/// @param value 值对象
/// @return 是否为整数值
AST_SCRIPT_CAPI bool aValueIsInt(Value* value);


/// @brief 判断值对象是否为算术值（包括整数、浮点数值和布尔值）
/// @param value 值对象
/// @return 是否为算术值
AST_SCRIPT_CAPI bool aValueIsArithmetic(Value* value);


/// @brief 判断值对象是否为量值
/// @param value 值对象
/// @return 是否为量值
AST_SCRIPT_CAPI bool aValueIsQuantity(Value* value);


/// @brief 将值对象转换为双精度浮点数值
/// @param value 值对象
/// @return 双精度浮点数值
AST_SCRIPT_CAPI double aValueToDouble(Value* value);

/// @brief 解包布尔值
/// @param value 值对象
/// @return 布尔值
AST_SCRIPT_CAPI bool aValueUnboxBool(Value* value);


/// @brief 解包双精度浮点数值
/// @param value 值对象
/// @return 双精度浮点数值
AST_SCRIPT_CAPI double aValueUnboxDouble(Value* value);


/// @brief 解包整数值
/// @param value 值对象
/// @return 整数值
AST_SCRIPT_CAPI int aValueUnboxInt(Value* value);


/// @brief 解包量值
/// @param value 值对象
/// @return 量值
AST_SCRIPT_CAPI Quantity aValueUnboxQuantity(Value* value);


/// @brief 格式化表达式为字符串
/// @param expr 表达式对象
AST_SCRIPT_API std::string aFormatExpr(Expr* expr, Object* context=nullptr);


/// @brief 二元运算函数指针类型 
typedef Value* (*OpBinFunc)(Value* left, Value* right);

/// @brief 赋值运算函数指针类型 
typedef Value* (*OpAssignFunc)(Expr* left, Expr* right);    // right 采用表达式对象，因为需要支持延迟赋值和绑定赋值

/// @brief 一元运算函数指针类型 
typedef Value* (*OpUnaryFunc)(Value* value);


enum {eIterBegin=0};  ///< 迭代开始标识

/// @brief 迭代函数指针类型 
typedef Value* (*IterateFunc)(Value* value, int& index);


//  @brief 获取二进制运算函数指针
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @return 运算函数指针
AST_SCRIPT_CAPI OpBinFunc aGetOpBinFunc(EOpBinType op, Class* leftType, Class* rightType);


//  @brief 获取赋值运算函数指针
//  @param op 运算类型
//  @param leftType 左运算数类型
//  @param rightType 右运算数类型
//  @return 运算函数指针
AST_SCRIPT_CAPI OpAssignFunc aGetOpAssignFunc(EOpAssignType op, Class* leftType, Class* rightType);


/// @brief 获取一元运算函数指针
//  @param op 运算类型
//  @param type 运算数类型
//  @return 运算函数指针
AST_SCRIPT_CAPI OpUnaryFunc aGetOpUnaryFunc(EOpUnaryType op, Class* type);


/// @brief 获取迭代函数指针
//  @param type 迭代对象类型
//  @return 迭代函数指针
AST_SCRIPT_CAPI IterateFunc aGetIterateFunc(Class* type);


/// @brief 执行二元运算符
/// @param op 运算符类型
/// @param left 左运算数
/// @param right 右运算数
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 运算结果
AST_SCRIPT_CAPI Value* aDoOpBin(EOpBinType op, Value* left, Value* right);

/// @brief 执行一元运算符
/// @param op 运算符类型
/// @param value 运算数
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 运算结果
AST_SCRIPT_CAPI Value* aDoOpUnary(EOpUnaryType op, Value* value);


/// @brief 执行赋值运算符
/// @param op 运算符类型
/// @param left 左运算数
/// @param right 右运算数
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 运算结果
AST_SCRIPT_CAPI Value* aDoOpAssign(EOpAssignType op, Expr* left, Expr* right);


/// @brief 执行迭代器的开始元素
/// @param[in] container 迭代容器
/// @param[out] index 迭代索引
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 开始元素
AST_SCRIPT_CAPI Value* aIterateBegin(Value* container, int& index);

/// @brief 执行迭代器的下一个元素
/// @param[in] container 迭代容器
/// @param[in,out] index 迭代索引
/// @warning 返回的 `Value*` 指针由调用者拥有，需要管理其生命周期
/// @return 下一个元素
AST_SCRIPT_CAPI Value* aIterateNext(Value* container, int& index);


// ALIAS

A_ALWAYS_INLINE Value* aNewValue(int value)
{
    return aNewValueInt(value);
}

A_ALWAYS_INLINE Value* aNewValue(double value)
{
    return aNewValueDouble(value);
}

A_ALWAYS_INLINE Value* aNewValue(bool value)
{
    return aNewValueBool(value);
}

A_ALWAYS_INLINE Value* aNewValue(const Quantity& value)
{
    return aNewValueQuantity(value);
}

A_ALWAYS_INLINE Value* aNewValue(StringView value)
{
    return aNewValueString(value);
}


AST_NAMESPACE_END