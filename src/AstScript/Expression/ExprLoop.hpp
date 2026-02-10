///
/// @file      ExprLoop.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-24
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
#include "Expr.hpp"
#include "ExprBlock.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN

/// @brief     循环表达式基类
/// @ingroup Script
class ExprLoop: public Expr
{
public:
    using Expr::Expr;
    ~ExprLoop() override = default;
};


/// @brief     while 循环表达式
/// @details   Julia语法: while condition; body end
/// @ingroup Script
class ExprWhile: public ExprLoop
{
public:
    AST_EXPR(ExprWhile)

    /// @brief 构造函数
    ExprWhile();
    
    /// @brief 构造函数
    /// @param condition 循环条件表达式
    /// @param body 循环体
    ExprWhile(Expr* condition, Expr* body);
    
    ~ExprWhile() override = default;

    /// @brief 求值
    /// @return Value* 求值结果
    Value* eval() const override;

    /// @brief 设置表达式的值
    err_t setValue(Value* val) override;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象，用于解析变量等
    /// @return std::string 表达式的字符串表示
    std::string getExpression(Object* context=nullptr) const override;

protected:
    SharedPtr<Expr> condition_; ///< 循环条件表达式
    SharedPtr<Expr> body_;      ///< 循环体
};



/// @brief     for 循环表达式（范围风格）
/// @details   Julia语法: for variable in range; body end
/// @ingroup Script
class ExprForRange: public ExprLoop
{
public:
    AST_EXPR(ExprForRange)

    /// @brief 构造函数
    ExprForRange();
    
    /// @brief 构造函数
    /// @param variable 循环变量
    /// @param range 范围表达式
    /// @param body 循环体
    ExprForRange(Expr* variable, Expr* range, Expr* body);
    
    ~ExprForRange() override = default;

    /// @brief 求值
    /// @return Value* 求值结果
    Value* eval() const override;

    /// @brief 设置表达式的值
    err_t setValue(Value* val) override;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象，用于解析变量等
    /// @return std::string 表达式的字符串表示
    std::string getExpression(Object* context=nullptr) const override;

protected:
    SharedPtr<Expr> variable_; ///< 循环变量
    SharedPtr<Expr> range_;    ///< 范围表达式
    SharedPtr<Expr> body_;     ///< 循环体
};


AST_NAMESPACE_END