///
/// @file      ExprContainer.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
#include <vector>

AST_NAMESPACE_BEGIN


/// @brief 表达式容器类
/// @ingroup Script
class ExprContainer: public Expr
{
public:
    using VectorType = std::vector<SharedPtr<Expr>>;

    ExprContainer() = default;

    ExprContainer(const VectorType& elems)
        : elems_(elems) {}
    
    /// @brief 获取向量元素
    /// @param index 元素索引
    /// @return 向量元素指针
    Expr* at(size_t index) const { return elems_[index].get(); }
    
    /// @brief 获取向量元素数量
    /// @return 元素数量
    size_t size() const { return elems_.size(); }

    /// @brief 添加表达式到容器
    /// @param expr 要添加的表达式
    void push_back(Expr* expr) { elems_.push_back(expr); }

    /// @brief 获取表达式容器
    /// @return 表达式容器引用
    const VectorType& children() const { return elems_; }
protected:
    VectorType elems_;      ///< 表达式容器
};

AST_NAMESPACE_END
