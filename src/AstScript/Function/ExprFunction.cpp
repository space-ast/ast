///
/// @file      ExprFunction.cpp
/// @brief     Julia风格函数定义表达式实现
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.

#include "ExprFunction.hpp"
#include "Function.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include <sstream>

AST_NAMESPACE_BEGIN

ExprFunction::ExprFunction()
    : name_(), params_(), body_(), isShorthand_(false)
{
}

ExprFunction::ExprFunction(const std::string& name,
                          const std::vector<std::string>& params,
                          Expr* body)
    : name_(name), params_(params), body_(body), isShorthand_(true)
{
}

Value* ExprFunction::eval() const
{
    Function* func = new Function(name_, params_, body_.get());
    return func;
}

std::string ExprFunction::getExpression(Object* context) const
{
    std::ostringstream oss;
    
    if (isShorthand_) {
        // 简洁语法: name(params) = body
        oss << name_ << "(";
        for (size_t i = 0; i < params_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << params_[i];
        }
        oss << ") = ";
        if (body_) oss << body_->getExpression(context);
    } else {
        // 标准语法: function name(params) ... end
        oss << "function " << name_ << "(";
        for (size_t i = 0; i < params_.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << params_[i];
        }
        oss << ")\n";
        if (body_) oss << body_->getExpression(context);
        oss << "\nend";
    }
    return oss.str();
}

// ExprReturn 实现
ExprReturn::ExprReturn(Expr* value) : value_(value) {}

Value* ExprReturn::eval() const
{
    if (value_) return value_->eval();
    return aValueNull();
}

std::string ExprReturn::getExpression(Object* context) const
{
    std::ostringstream oss;
    oss << "return";
    if (value_) oss << " " << value_->getExpression(context);
    return oss.str();
}

// 工厂函数
ExprFunction* aNewExprFunction(const std::string& name,
                               const std::vector<std::string>& params,
                               Expr* body)
{
    return new ExprFunction(name, params, body);
}

ExprReturn* aNewExprReturn(Expr* value)
{
    return new ExprReturn(value);
}

AST_NAMESPACE_END
