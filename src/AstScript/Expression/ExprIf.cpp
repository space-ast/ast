///
/// @file      ExprIf.cpp
/// @brief     if条件语句表达式实现
/// @details   实现if-elseif-else条件结构的求值和字符串表示
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

#include "ExprIf.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValNull.hpp"

AST_NAMESPACE_BEGIN

ExprIf::ExprIf(){}

ExprIf::ExprIf(Expr * condition, Expr * block)
    :ExprIf()
{
    addElseif(condition, block);
}

Value* ExprIf::eval() const {
    if(conditionBranches_.empty()){
        aError("if statement must have at least one if branch");
        return nullptr;
    }

    SharedPtr<Value> elseifCondVal;
    // 检查所有条件分支
    for (const auto& elseif : conditionBranches_) {
        elseifCondVal = elseif.condition->eval();
        if (!elseifCondVal) {
            return nullptr;
        }
        
        if (!aValueIsBool(elseifCondVal)) {
            aError("expect boolean value in if condition");
            return nullptr;
        }
        if (aValueUnboxBool(elseifCondVal)) {
            return elseif.block->eval();
        }
    }
    
    // 如果没有匹配的条件且有else分支，则执行else分支
    if (elseBlock_) {
        return elseBlock_->eval();
    }
    
    // 没有匹配的条件且没有else分支，返回null
    return aValueNull();
}

err_t ExprIf::setValue(Value* val) {
    // if语句是不可赋值的
    return eError;
}

std::string ExprIf::getExpression(Object* context) const {
    std::string expr;
    bool first = true;
    
    for (const auto& elseif : conditionBranches_) {
        if(first){
            expr += "if ";
            first = false;
        }
        else{
            expr += " elseif ";
        }
        expr += elseif.condition->getExpression(context) + " " + elseif.block->getExpression(context);
    }
    
    if (elseBlock_) {
        expr += " else " + elseBlock_->getExpression(context);
    }
    
    return expr;
}

void ExprIf::addElseif(Expr* condition, Expr* block) {
    conditionBranches_.push_back({condition, block});
}

void ExprIf::setElse(Expr* block) {
    elseBlock_ = block;
}

AST_NAMESPACE_END