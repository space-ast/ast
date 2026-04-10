///
/// @file      ExprCall.cpp
/// @brief     函数调用表达式实现
/// @details   ~
/// @author    axel
/// @date      2025-12-24
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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

#include "ExprCall.hpp"
#include "AstScript/Function.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

Value* ExprCall::eval() const
{
    // 1. 求值函数表达式
    // 如果函数表达式是变量，则获取变量的值（应该是函数对象）
    // 否则直接求值
    Value* funcVal = func_->eval();
    if (!funcVal) {
        aError("Failed to evaluate function expression");
        return aValueNull();
    }

    // 检查是否是函数对象
    Function* func = dynamic_cast<Function*>(funcVal);
    if (!func) {
        // 如果不是函数，检查是否是变量，可能是通过变量间接调用
        Variable* var = dynamic_cast<Variable*>(funcVal);
        if (var) {
            func = dynamic_cast<Function*>(var);
        }
    }
    
    if (!func) {
        aError("Expression is not a function");
        return aValueNull();
    }

    // 2. 求值所有参数
    std::vector<Value*> argValues;
    for (const auto& arg : args_) {
        Value* argVal = arg->eval();
        if (!argVal) {
            aError("Failed to evaluate argument");
            // 清理已求值的参数
            for (auto v : argValues) {
                delete v;
            }
            return aValueNull();
        }
        argValues.push_back(argVal);
    }

    // 3. 调用函数
    Value* result = func->call(argValues);

    // 清理参数值
    for (auto v : argValues) {
        delete v;
    }

    return result;
}

std::string ExprCall::getExpression(Object* context) const
{
    std::string result;
    
    // 获取函数表达式的字符串表示
    result = func_->getExpression(context);
    
    // 添加参数列表
    result += "(";
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) {
            result += ", ";
        }
        result += args_[i]->getExpression(context);
    }
    result += ")";
    
    return result;
}

AST_NAMESPACE_END
