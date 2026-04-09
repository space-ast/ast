///
/// @file      Function.hpp
/// @brief     函数对象
/// @details   用于表示脚本中的函数定义
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

#pragma once

#include "AstGlobal.h"
#include "AstScript/Macro.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 函数对象
/// @details
/// 函数对象用于表示脚本中的函数定义，可以包含参数和函数体。
/// @ingroup Script
class Function: public Macro
{
public:
    AST_EXPR(Function)

    /// @brief 默认构造函数
    Function() = default;

    /// @brief 构造函数
    /// @param name 函数名
    /// @param params 参数名列表
    /// @param body 函数体表达式
    Function(const std::string& name, 
             const std::vector<std::string>& params,
             Expr* body)
        : name_(name), params_(params), body_(body) {}

    ~Function() override = default;

    /// @brief 获取函数名
    /// @return 函数名
    const std::string& name() const { return name_; }

    /// @brief 获取参数名列表
    /// @return 参数名列表
    const std::vector<std::string>& params() const { return params_; }

    /// @brief 获取参数个数
    /// @return 参数个数
    size_t paramCount() const { return params_.size(); }

    /// @brief 获取函数体
    /// @return 函数体表达式
    Expr* body() const { return body_.get(); }

    /// @brief 设置函数体
    /// @param body 函数体表达式
    void setBody(Expr* body) { body_ = body; }

    /// @brief 求值函数（用于调用函数时执行）
    /// @param args 参数值列表
    /// @return 函数执行结果
    Value* call(const std::vector<Value*>& args) const;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象
    /// @return 字符串表示
    std::string getExpression(Object* context = nullptr) const override;

private:
    std::string name_;                           ///< 函数名
    std::vector<std::string> params_;            ///< 参数名列表
    SharedPtr<Expr> body_;                       ///< 函数体表达式
};

/// @brief 创建函数对象
/// @param name 函数名
/// @param params 参数名列表
/// @param body 函数体表达式
/// @return 函数对象
AST_SCRIPT_API Function* aNewFunction(const std::string& name,
                                       const std::vector<std::string>& params,
                                       Expr* body);

AST_NAMESPACE_END
