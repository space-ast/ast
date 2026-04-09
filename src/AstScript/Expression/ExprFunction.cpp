///
/// @file      ExprFunction.cpp
/// @brief     函数定义表达式实现
/// @details   实现Julia风格的函数定义
/// @author    axel
/// @date      2025-01-10
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

#include "AstScript/ExprFunction.hpp"
#include "AstScript/Basic/ExprVisitor.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/String.hpp"

AST_NAMESPACE_BEGIN

ExprFunction::ExprFunction(const std::string& name, const std::vector<std::string>& params, Expr* body)
    : name_(name), params_(params), body_(body)
{
}

void ExprFunction::accept(ExprVisitor& visitor)
{
    visitor.visit(*this);
}

Value* ExprFunction::eval() const
{
    // 函数定义表达式本身不直接求值
    // 函数对象会被注册到符号表中
    // 返回null表示成功定义函数
    return nullptr;
}

std::string ExprFunction::getExpression(Object* context) const
{
    std::string result = "function " + name_ + "(";
    
    // 添加参数列表
    for (size_t i = 0; i < params_.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += params_[i];
    }
    
    result += ") ";
    
    // 添加函数体
    if (body_) {
        result += body_->getExpression(context);
    }
    
    result += " end";
    
    return result;
}

AST_NAMESPACE_END
