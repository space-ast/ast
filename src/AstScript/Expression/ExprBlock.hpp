///
/// @file      ExprBlock.hpp
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

#pragma once

#include "AstGlobal.h"
#include "ExprContainer.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief     代码块表达式
class ExprBlock: public ExprContainer
{
public:
    AST_EXPR(ExprBlock)

    using ExprContainer::ExprContainer;

    ~ExprBlock() override = default;

    /// @brief 求值
    /// @return Value* 求值结果
    Value* eval() const override;

    /// @brief 设置表达式的值
    err_t setValue(Value* val) override;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象，用于解析变量等
    /// @return std::string 表达式的字符串表示
    std::string getExpression(Object* context=nullptr) const override;

    /// @brief 添加表达式到代码块
    /// @param expr 要添加的表达式
    void addExpr(Expr* expr);

};



AST_NAMESPACE_END