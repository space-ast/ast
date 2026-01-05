///
/// @file      ExprCondition.cpp
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

#include "ExprCondition.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ScriptAPI.hpp"

AST_NAMESPACE_BEGIN

// accept函数由AST_EXPR宏自动生成，无需手动实现

Value* ExprCondition::eval() const {
    if (A_UNLIKELY(!condition_ || !thenExpr_ )) {
        aError("condition, thenExpr is null");
        return nullptr;
    }
    
    // 计算条件表达式的值
    SharedPtr<Value> conditionValue = condition_->eval();
    if (A_UNLIKELY(!conditionValue)) {
        aError("condition expression evaluation failed");
        return nullptr;
    }
    
    // 判断条件是否为真
    bool isTrue = false;
    if (aValueIsBool(conditionValue.get())) {
        isTrue = aValueUnboxBool(conditionValue.get());
    } else {
        aError("condition expression is not bool");
        // 非布尔值转为布尔值：非零即真
        // isTrue = true; // 默认为真，除非是null
        return nullptr;
    }
    
    // 根据条件执行相应的分支
    if (isTrue) {
        return thenExpr_->eval();
    } else if(elseExpr_){
        return elseExpr_->eval();
    }
    return nullptr;
}


err_t ExprCondition::setValue(Value* val) {
    // 条件表达式是只读的，无法直接设置值
    (void)val; // 避免未使用参数警告
    return eErrorReadonly;
}

std::string ExprCondition::getExpression(Object* context) const {
    std::string expr;
    expr += '(';
    expr += condition_->getExpression(context);
    expr += " ? ";
    expr += thenExpr_->getExpression(context);
    expr += " : ";
    expr += elseExpr_->getExpression(context);
    expr += ')';
    return expr;
}

AST_NAMESPACE_END