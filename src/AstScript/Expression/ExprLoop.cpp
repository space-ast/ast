///
/// @file      ExprLoop.cpp
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

#include "ExprLoop.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/Variable.hpp"

AST_NAMESPACE_BEGIN

// ------------------ ExprWhile 实现 ------------------

/// @brief 默认构造函数
ExprWhile::ExprWhile()
    : condition_(nullptr)
    , body_(nullptr)
{
}

/// @brief 构造函数
/// @param condition 循环条件表达式
/// @param body 循环体
ExprWhile::ExprWhile(Expr* condition, Expr* body)
    : condition_(condition)
    , body_(body)
{
}

/// @brief 求值
/// @return Value* 求值结果
Value* ExprWhile::eval() const
{
    if (A_UNLIKELY(!condition_ || !body_)) {
        aError("while loop condition or body is null");
        return nullptr;
    }

    SharedPtr<Value> lastValue;
    
    // while循环的执行逻辑
    while (true) {
        // 计算循环条件
        SharedPtr<Value> conditionValue = condition_->eval();
        if (!conditionValue) {
            aError("while loop condition evaluation failed");
            return nullptr;
        }

        // 判断条件是否为真
        bool isTrue = false;
        if (aValueIsBool(conditionValue)) {
            isTrue = aValueUnboxBool(conditionValue);
        } else {
            aError("while loop condition is not a boolean");
            return nullptr;
        }

        // 如果条件为假，退出循环
        if (!isTrue) {
            break;
        }

        // 执行循环体
        lastValue = body_->eval();
        if (!lastValue) {
            aError("while loop body evaluation failed");
            return nullptr;
        }

    }

    return lastValue.take();
}

/// @brief 设置表达式的值
err_t ExprWhile::setValue(Value* val)
{
    // 循环表达式是只读的，不能直接设置值
    (void)val;
    return eErrorReadonly;
}

/// @brief 获取表达式的字符串表示
/// @param context 可选的上下文对象，用于解析变量等
/// @return std::string 表达式的字符串表示
std::string ExprWhile::getExpression(Object* context) const
{
    if (!condition_ || !body_) {
        return "while error";
    }

    std::string expr;
    expr += "while ";
    expr += condition_->getExpression(context);
    expr += "; \n";
    expr += body_->getExpression(context);
    expr += " end\n";

    return expr;
}

// ------------------ ExprForRange 实现 ------------------

/// @brief 默认构造函数
ExprForRange::ExprForRange()
    : variable_(nullptr)
    , range_(nullptr)
    , body_(nullptr)
{
}

/// @brief 构造函数
/// @param variable 循环变量
/// @param range 范围表达式
/// @param body 循环体
ExprForRange::ExprForRange(Expr* variable, Expr* range, Expr* body)
    : variable_(variable)
    , range_(range)
    , body_(body)
{
}

/// @brief 求值
/// @return Value* 求值结果
Value* ExprForRange::eval() const
{
    if (A_UNLIKELY(!variable_ || !range_ || !body_)) {
        aError("for loop variable, range or body is null");
        return nullptr;
    }

    // 计算范围表达式的值
    SharedPtr<Value> rangeValue = range_->eval();
    if (!rangeValue) {
        aError("for loop range evaluation failed");
        return nullptr;
    }
    int index;
    SharedPtr<Value> iter = aIterateBegin(rangeValue, index);
    SharedPtr<Value> lastValue;
    while(iter){
        // 绑定循环变量到当前索引值
        variable_->setValue(iter);
        
        // 执行循环体
        lastValue = body_->eval();
        if (!lastValue) {
            aError("for loop body evaluation failed");
            return nullptr;
        }
        iter = aIterateNext(rangeValue, index);
    }
    return lastValue.take();
}

/// @brief 设置表达式的值
err_t ExprForRange::setValue(Value* val)
{
    // 循环表达式是只读的，不能直接设置值
    (void)val;
    return eErrorReadonly;
}

/// @brief 获取表达式的字符串表示
/// @param context 可选的上下文对象，用于解析变量等
/// @return std::string 表达式的字符串表示
std::string ExprForRange::getExpression(Object* context) const
{
    if (!variable_ || !range_ || !body_) {
        return "for error";
    }

    std::string expr;
    expr += "for ";
    expr += variable_->getExpression(context);
    expr += " = ";
    expr += range_->getExpression(context);
    expr += "; \n";
    expr += body_->getExpression(context);
    expr += " end\n";

    return expr;
}

AST_NAMESPACE_END