///
/// @file      ExprCondition.hpp
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
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN

/// @brief     条件表达式 (三元运算符 ? :)
class ExprCondition: public Expr
{
public:
    AST_EXPR(ExprCondition)

public:
    ExprCondition(Expr* condition, Expr* thenExpr, Expr* elseExpr)
        : condition_(condition), thenExpr_(thenExpr), elseExpr_(elseExpr)
    {};
    virtual ~ExprCondition() = default;
    
    // accept函数由AST_EXPR宏自动生成，无需手动声明
    Value* eval() const override;
    err_t setValue(Value* val) override;
    std::string getExpression(Object* context=nullptr) const override;

private:
    SharedPtr<Expr> condition_; ///< 条件表达式
    SharedPtr<Expr> thenExpr_;  ///< 条件为真时的表达式
    SharedPtr<Expr> elseExpr_;  ///< 条件为假时的表达式
};

AST_NAMESPACE_END