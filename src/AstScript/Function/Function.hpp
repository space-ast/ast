///
/// @file      Function.hpp
/// @brief     函数对象
/// @details   用于表示脚本中定义的函数
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
/// @details 函数对象用于表示脚本中的函数定义
/// @ingroup Script
class Function: public Macro
{
public:
    AST_EXPR(Function)

    Function() = default;

    Function(const std::string& name, 
             const std::vector<std::string>& params,
             Expr* body)
        : name_(name), params_(params), body_(body) {}

    ~Function() override = default;

    const std::string& name() const { return name_; }
    const std::vector<std::string>& params() const { return params_; }
    size_t paramCount() const { return params_.size(); }
    Expr* body() const { return body_.get(); }
    void setBody(Expr* body) { body_ = body; }

    /// @brief 调用函数
    Value* call(const std::vector<Value*>& args) const;

    std::string getExpression(Object* context = nullptr) const override;

private:
    std::string name_;
    std::vector<std::string> params_;
    SharedPtr<Expr> body_;
};

AST_NAMESPACE_END
