///
/// @file      Variable.hpp
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
#include "AstScript/Expr.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <string>

AST_NAMESPACE_BEGIN

class Value;

/// @brief 变量
/// @details
/// 变量是脚本语言中的基本元素，表示存储数据的命名位置
/// 变量可以存储各种类型的值，如整数、浮点数、字符串、
/// 变量也可以与其他变量单向/双向绑定，实现引用传递的效果
/// @ingroup Script
class AST_SCRIPT_API Variable: public Expr
{
public:
    AST_OBJECT(Variable)
    AST_PROPERT(value)
    AST_EXPR_EXTRA(Variable)

    static Variable* New();
    
    Variable(StringView name, Expr* expr=nullptr, bool bind = false);
    Variable(Expr* expr=nullptr, bool bind = false);

    /// @brief 计算变量的值
    /// @return 变量的值
    Value* eval() const final;

    /// @brief 设置变量的值
    /// @param val 要设置的值
    /// @return 错误码
    errc_t setValue(Value* val) final;

    /// @brief 设置变量的表达式
    /// @param expr 要设置的表达式
    /// @return 错误码
    errc_t setExpr(Expr* expr);

    /// @brief 设置变量的表达式为双向绑定
    /// @param expr 要设置的表达式
    /// @return 错误码
    errc_t setBindExpr(Expr* expr);

    /// @brief 设置变量的表达式
    /// @param value 要设置的值
    /// @return 错误码
    errc_t setExpr(Value* value);

    /// @brief 设置变量的表达式
    /// @param value 要设置的字符串
    /// @return 错误码
    errc_t setExpr(StringView value);

    /// @brief 设置变量的表达式
    /// @param value 要设置的字符串
    /// @return 错误码
    errc_t setExpr(const std::string& value);

    /// @brief 设置变量的表达式
    /// @param quantity 要设置的数量
    /// @return 错误码
    errc_t setExpr(const Quantity& quantity);

    /// @brief 设置变量的表达式
    /// @param value 要设置的浮点数
    /// @return 错误码
    errc_t setExpr(double value);

    /// @brief 设置变量的表达式
    /// @param value 要设置的整数
    /// @return 错误码
    errc_t setExpr(int value);

    /// @brief 设置变量的表达式
    /// @param value 要设置的布尔值
    /// @return 错误码
    errc_t setExpr(bool value);

    // 通过模板捕获所有其他类型并删除，防止隐式转换为 `bool` 类型
    template<typename T>
    errc_t setExpr(T value) = delete;
    
    /// @brief 绑定变量到表达式
    /// @param expr 要绑定的表达式
    /// @return 错误码
    errc_t bind(Expr* expr);

    
    /// @brief 获取变量的表达式
    /// @param context 上下文对象
    /// @return 变量的表达式字符串
    std::string getExpression(Object* context=nullptr) const override{return name_;}
public:
    const std::string& name() const { return name_; }
    const std::string& getName() const override { return name_; }
    void setName(StringView name) override { name_ = std::string(name); }
    Expr* expr() const { return expr_.get(); }
PROPERTIES:
    std::string value() const;
    errc_t setValue(StringView value);
protected:
    std::string name_;            ///< 变量的名称
    SharedPtr<Expr> expr_;        ///< 变量的值，或者绑定的表达式
    bool bind_;                   ///< 是否与表达式双向绑定
};


AST_NAMESPACE_END