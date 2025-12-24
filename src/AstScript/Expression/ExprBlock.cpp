///
/// @file      ExprBlock.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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

#include "ExprBlock.hpp"
#include "AstScript/Value.hpp"


AST_NAMESPACE_BEGIN

ExprBlock::ExprBlock(const std::vector<SharedPtr<Expr>> &exprs)
    :exprs_(exprs)
{
    
}


/// @brief 求值
/// @return Value* 求值结果
Value* ExprBlock::eval() const
{
    SharedPtr<Value> result = nullptr;
    for (auto expr : exprs_) {
        result = expr->eval();
        if(!result) {
            return nullptr;
        }
    }
    return result.take();
}


/// @brief 设置表达式的值
err_t ExprBlock::setValue(Value* val)
{
    // 代码块不能直接设置值
    return eErrorReadonly;
}

/// @brief 获取表达式的字符串表示
/// @param context 可选的上下文对象，用于解析变量等
/// @return std::string 表达式的字符串表示
std::string ExprBlock::getExpression(Object* context) const
{
    std::string result = "begin ";
    for (size_t i = 0; i < exprs_.size(); ++i) {
        result += exprs_[i]->getExpression(context);
        if (i < exprs_.size() - 1) {
            result += "; ";
        }
    }
    result += " end";
    return result;
}

/// @brief 添加表达式到代码块
/// @param expr 要添加的表达式
void ExprBlock::addExpr(Expr* expr)
{
    exprs_.push_back(expr);
}



AST_NAMESPACE_END