///
/// @file      Variable.cpp
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

#include "Variable.hpp"
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN


Variable::Variable(StringView name, Expr *expr, bool bind)
    : name_(name.to_string()), expr_(expr), bind_(bind)
{};

Variable::Variable(Expr *expr, bool bind)
    : name_{}, expr_(expr), bind_(bind)
{};

Value *Variable::eval() const
{
    if(expr_.get() == nullptr) {
        return nullptr;
    }
    return expr_->eval();
}

err_t Variable::setValue(Value *val)
{
    if(!val){
        return eErrorNullInput;
    }
    if(bind_){
        // 双向绑定，设置表达式的值
        return expr_->setValue(val);
    }else{
        // 单向绑定，直接替换表达式
        expr_ = val;
        return eNoError;
    }
}

err_t Variable::setExpr(Expr *expr)
{
    expr_ = expr;
    bind_ = false;
    return eNoError;
}

err_t Variable::bind(Expr *expr)
{
    expr_ = expr;
    bind_ = true;
    return eNoError;
}

AST_NAMESPACE_END

