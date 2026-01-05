///
/// @file      ExprFuncCall.cpp
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

#include "ExprCall.hpp"

AST_NAMESPACE_BEGIN


Value* ExprCall::eval() const
{
    // 这里需要实现函数调用的求值逻辑
    // 1. 先求值函数表达式，得到函数对象
    // 2. 求值所有参数
    // 3. 调用函数并返回结果
    // 暂时返回 nullptr，需要根据实际的函数调用机制来实现
    return nullptr;
}

std::string ExprCall::getExpression(Object* context) const
{
    std::string result;
    
    // 如果函数表达式是变量或其他简单表达式，直接使用其字符串表示
    // 如果是复杂表达式（如二元表达式），需要添加括号
    bool needParentheses = false;
    // 这里可以根据 m_func 的类型判断是否需要添加括号
    // 暂时简化处理，直接调用 getExpression
    result = func_->getExpression(context);
    
    // 添加参数列表，使用 Julia 语言的语法：func(args...)
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