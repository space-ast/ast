///
/// @file      ExprFunction.hpp
/// @brief     Julia风格函数定义表达式
/// @details   支持 function name(params) ... end 和 name(params) = expr 语法
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。

#pragma once

#include "AstGlobal.h"
#include "Expr.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/// @brief     函数定义表达式（Julia风格）
/// @details   支持：
///            - 标准语法: function name(params) ... end
///            - 简洁语法: name(params) = expression
class ExprFunction: public Expr
{
public:
    AST_EXPR(ExprFunction)

    ExprFunction();
    ExprFunction(const std::string& name,
                 const std::vector<std::string>& params,
                 Expr* body);
    ~ExprFunction() override = default;

    const std::string& name() const { return name_; }
    const std::vector<std::string>& params() const { return params_; }
    size_t paramCount() const { return params_.size(); }
    Expr* body() const { return body_.get(); }
    void setBody(Expr* body) { body_ = body; }
    bool isShorthand() const { return isShorthand_; }

    Value* eval() const override;
    std::string getExpression(Object* context = nullptr) const override;

private:
    std::string name_;
    std::vector<std::string> params_;
    SharedPtr<Expr> body_;
    bool isShorthand_ = false;
};

/// @brief     return语句表达式
class ExprReturn: public Expr
{
public:
    AST_EXPR(ExprReturn)
    ExprReturn(Expr* value = nullptr);
    ~ExprReturn() override = default;

    Expr* value() const { return value_.get(); }
    Value* eval() const override;
    std::string getExpression(Object* context = nullptr) const override;

private:
    SharedPtr<Expr> value_;
};

AST_SCRIPT_API ExprFunction* aNewExprFunction(const std::string& name,
                                               const std::vector<std::string>& params,
                                               Expr* body);
AST_SCRIPT_API ExprReturn* aNewExprReturn(Expr* value = nullptr);

AST_NAMESPACE_END
