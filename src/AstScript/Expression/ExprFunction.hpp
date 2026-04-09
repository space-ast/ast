///
/// @file      ExprFunction.hpp
/// @brief     函数定义表达式
/// @details   实现Julia风格的函数定义语法: function name(params) ... end
/// @author    axel
/// @date      2025-01-10
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

#pragma once

#include "AstGlobal.h"
#include "AstScript/Expr.hpp"
#include "AstScript/Function/Function.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief     函数定义表达式
/// @details   实现Julia风格的函数定义语法: function name(params) ... end
/// @ingroup Script
class ExprFunction: public Expr
{
public:
    AST_EXPR(ExprFunction)

    /// @brief 构造函数
    /// @param name 函数名称
    /// @param params 参数列表（变量名列表）
    /// @param body 函数体表达式
    ExprFunction(const std::string& name, const std::vector<std::string>& params, Expr* body);

    /// @brief 析构函数
    ~ExprFunction() override = default;

    /// @brief 接受表达式访问者
    /// @param visitor 表达式访问者对象
    void accept(ExprVisitor& visitor) override;

    /// @brief 求值
    /// @return Value* 求值结果
    Value* eval() const override;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象，用于解析变量等
    /// @return std::string 表达式的字符串表示
    std::string getExpression(Object* context=nullptr) const override;

    /// @brief 获取函数名称
    /// @return const std::string& 函数名称
    const std::string& getName() const { return name_; }

    /// @brief 获取参数列表
    /// @return const std::vector<std::string>& 参数列表
    const std::vector<std::string>& getParams() const { return params_; }

    /// @brief 获取函数体
    /// @return Expr* 函数体表达式
    Expr* getBody() const { return body_.get(); }

private:
    std::string name_;                              ///< 函数名称
    std::vector<std::string> params_;              ///< 参数列表
    SharedPtr<Expr> body_;                          ///< 函数体表达式
};

AST_NAMESPACE_END
