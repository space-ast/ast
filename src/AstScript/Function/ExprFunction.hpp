///
/// @file      ExprFunction.hpp
/// @brief     Julia风格函数定义表达式
/// @details   支持 function name(params) ... end 和 name(params) = expr 语法
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
#include "Expr.hpp"
#include "ExprBlock.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/// @brief     函数定义表达式
/// @details   Julia风格函数定义，支持：
///            - 标准语法: function name(params) ... end
///            - 简洁语法: name(params) = expression
/// @ingroup Script
class ExprFunction: public Expr
{
public:
    AST_EXPR(ExprFunction)

    /// @brief 默认构造函数
    ExprFunction();

    /// @brief 构造函数（简洁语法）
    /// @param name 函数名
    /// @param params 参数名列表
    /// @param body 函数体表达式
    ExprFunction(const std::string& name,
                 const std::vector<std::string>& params,
                 Expr* body);

    /// @brief 析构函数
    ~ExprFunction() override = default;

    /// @brief 获取函数名
    const std::string& name() const { return name_; }

    /// @brief 获取参数名列表
    const std::vector<std::string>& params() const { return params_; }

    /// @brief 获取参数个数
    size_t paramCount() const { return params_.size(); }

    /// @brief 获取函数体
    Expr* body() const { return body_.get(); }

    /// @brief 设置函数体
    /// @param body 函数体表达式
    void setBody(Expr* body) { body_ = body; }

    /// @brief 是否是简洁语法
    bool isShorthand() const { return isShorthand_; }

    /// @brief 求值（返回函数值）
    Value* eval() const override;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象
    std::string getExpression(Object* context = nullptr) const override;

private:
    std::string name_;                           ///< 函数名
    std::vector<std::string> params_;           ///< 参数名列表
    SharedPtr<Expr> body_;                      ///< 函数体表达式
    bool isShorthand_ = false;                  ///< 是否是简洁语法
};

/// @brief     return语句表达式
/// @details   Julia风格return语句
/// @ingroup Script
class ExprReturn: public Expr
{
public:
    AST_EXPR(ExprReturn)

    /// @brief 构造函数
    /// @param value 返回值表达式（可选）
    ExprReturn(Expr* value = nullptr);

    /// @brief 析构函数
    ~ExprReturn() override = default;

    /// @brief 获取返回值表达式
    Expr* value() const { return value_.get(); }

    /// @brief 求值
    /// @details 如果返回值表达式，返回其求值结果；否则返回null
    Value* eval() const override;

    /// @brief 获取表达式的字符串表示
    std::string getExpression(Object* context = nullptr) const override;

private:
    SharedPtr<Expr> value_;                     ///< 返回值表达式
};

/// @brief 创建函数定义表达式
/// @param name 函数名
/// @param params 参数名列表
/// @param body 函数体表达式
/// @return 函数定义表达式
AST_SCRIPT_API ExprFunction* aNewExprFunction(const std::string& name,
                                               const std::vector<std::string>& params,
                                               Expr* body);

/// @brief 创建return语句表达式
/// @param value 返回值表达式（可选）
/// @return return语句表达式
AST_SCRIPT_API ExprReturn* aNewExprReturn(Expr* value = nullptr);

AST_NAMESPACE_END
