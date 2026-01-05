///
/// @file      ExprVector.hpp
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
#include "ExprContainer.hpp"
#include <vector>

AST_NAMESPACE_BEGIN


/// @brief 向量表达式，逗号分隔，例如 [1, 2, 3]、[a, b, c]
/// @details 
/// 每个向量元素之间用逗号分隔。
/// 向量元素可以是任意表达式，例如 [1 + 2, a * b, c]。
class ExprVector: public ExprContainer
{
public:
    AST_EXPR(ExprVector)

    using ExprContainer::ExprContainer;
    
    /// @brief 求值
    /// @return Value* 求值结果
    Value* eval() const override;

    /// @brief 获取表达式字符串
    /// @param context 上下文对象
    /// @return std::string 表达式字符串
    std::string getExpression(Object* context) const override;

};

AST_NAMESPACE_END
