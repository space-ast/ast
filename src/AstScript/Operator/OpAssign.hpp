///
/// @file      OpAssign.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
#include "Op.hpp"
#include "AstScript/ScriptAPI.hpp"

AST_NAMESPACE_BEGIN


/// @brief 赋值运算符表达式
/// @details
/// 赋值运算符表达式用于表示赋值操作，将右侧表达式的值赋值给左侧表达式。
/// @ingroup Script
class OpAssign: public Op
{
public:
    AST_EXPR(OpAssign)

    OpAssign(EOpAssignType op, Expr* left, Expr* right)
        :op_{op}
        ,left_{left}
        ,right_{right}
    {};
    Value* eval() const override;
    std::string getExpression(Object* context=nullptr) const override;
protected:
    EOpAssignType op_;
    SharedPtr<Expr> left_;
    SharedPtr<Expr> right_;
};

AST_NAMESPACE_END
