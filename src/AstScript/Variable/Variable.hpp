///
/// @file      Variable.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-19
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
#include "AstUtil/StringView.hpp"
#include "AstUtil/SharedPtr.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 变量
/// @details
/// 变量是脚本语言中的基本元素，表示存储数据的命名位置
/// 变量可以存储各种类型的值，如整数、浮点数、字符串、
/// 变量也可以与其他变量单向/双向绑定，实现引用传递的效果
class AST_SCRIPT_API Variable: public Expr
{
public:
    AST_EXPR(Variable)
    
    Variable(StringView name, Expr* expr=nullptr, bool bind = false);
    Variable(Expr* expr=nullptr, bool bind = false);

    /// @brief 计算变量的值
    /// @return 变量的值
    Value* eval() const final;

    /// @brief 设置变量的值
    /// @param val 要设置的值
    /// @return 错误码
    err_t setValue(Value* val) final;

    /// @brief 设置变量的表达式
    /// @param expr 要设置的表达式
    /// @return 错误码
    err_t setExpr(Expr* expr);

    /// @brief 绑定变量到表达式
    /// @param expr 要绑定的表达式
    /// @return 错误码
    err_t bind(Expr* expr);

    
    /// @brief 获取变量的表达式
    /// @param context 上下文对象
    /// @return 变量的表达式字符串
    std::string getExpression(Object* context=nullptr) const override{return name_;}
public:
    const std::string& name() const { return name_; }
    Expr* expr() const { return expr_.get(); }
    
protected:
    std::string name_;            ///< 变量的名称
    SharedPtr<Expr> expr_;        ///< 变量的值，或者绑定的表达式
    bool bind_;                   ///< 是否与表达式双向绑定
};


AST_NAMESPACE_END