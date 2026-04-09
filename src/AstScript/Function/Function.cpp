///
/// @file      Function.cpp
/// @brief     函数对象实现
/// @details   ~
/// @author    axel
/// @date      2025-12-19
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

#include "Function.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/Logger.hpp"
#include <sstream>

AST_NAMESPACE_BEGIN

Value* Function::call(const std::vector<Value*>& args) const
{
    // 参数个数检查
    if (args.size() != params_.size()) {
        aError("Function %s expects %zu arguments but got %zu", 
               name_.c_str(), params_.size(), args.size());
        return aValueNull();
    }

    // 创建新的作用域
    // 注意：这里需要实现作用域栈的逻辑
    // 简化实现：直接在全局作用域中创建参数变量
    for (size_t i = 0; i < params_.size(); ++i) {
        auto var = new Variable(params_[i], args[i], false);
        // 将变量添加到当前作用域
        // TODO: 实现作用域管理
        A_UNUSED(var);
    }

    // 执行函数体
    if (body_) {
        return body_->eval();
    }

    return aValueNull();
}

std::string Function::getExpression(Object* context) const
{
    std::ostringstream oss;
    oss << "function " << name_ << "(";
    for (size_t i = 0; i < params_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << params_[i];
    }
    oss << ") = ";
    if (body_) {
        oss << body_->getExpression(context);
    }
    return oss.str();
}

Function* aNewFunction(const std::string& name,
                       const std::vector<std::string>& params,
                       Expr* body)
{
    return new Function(name, params, body);
}

AST_NAMESPACE_END
