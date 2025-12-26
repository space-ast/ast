///
/// @file      ExprRange.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-26
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
#include "Expr.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN


/// @brief 范围表达式
/// @details 
/// 范围表达式用于表示一个范围，包含起始值、结束值和步长（可选）。
/// 步长表达式可选，默认值为1。
class ExprRange : public Expr
{
public:
    AST_EXPR(ExprRange)
    
    /// @brief 范围表达式构造函数
    /// @param start 范围的起始值表达式
    /// @param stop 范围的结束值表达式
    /// @param step 范围的步长表达式（可选）
    ExprRange(Expr* start, Expr* stop, Expr* step=nullptr);
    ~ExprRange() override = default;

    /// @brief 获取范围的起始值表达式
    /// @return 范围的起始值表达式
    Expr* start() const { return start_.get(); }

    /// @brief 获取范围的结束值表达式
    /// @return 范围的结束值表达式
    Expr* stop() const { return stop_.get(); }

    /// @brief 获取范围的步长表达式
    /// @return 范围的步长表达式
    /// @warning 可能返回nullptr，此时表示步长表达式未指定，默认值为1
    Expr* step() const { return step_.get(); }


    /// @brief 评估范围表达式
    /// @return 范围表达式的结果值
    Value* eval() const override;


    /// @brief 获取范围表达式的字符串表示
    /// @param context 上下文对象（可选）
    /// @return 范围表达式的字符串表示
    std::string getExpression(Object *context) const override;

protected:
    SharedPtr<Expr> start_;
    SharedPtr<Expr> stop_;
    SharedPtr<Expr> step_;
};


AST_NAMESPACE_END
