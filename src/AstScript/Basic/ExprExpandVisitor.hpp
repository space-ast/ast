///
/// @file      ExprExpandVisitor.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-04
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "ExprVisitor.hpp"
#include "AstScript/Expr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class ExprExpandVisitor final: public ExprVisitor
{
public:
    ExprExpandVisitor() = default;
    ~ExprExpandVisitor() override = default;

    void visit(Symbol& symbol) override;
    void visit(Variable& var) override;
    void visit(ValNull& val) override;
    void visit(ValInt& val) override;
    void visit(ValDouble& val) override;
    void visit(ValString& val) override;
    void visit(ValBool& val) override;
    void visit(ValQuantity& val) override;
    void visit(ValRange& val) override;
    void visit(ValDict& val) override;
    void visit(ValNamedVector& val) override;
    void visit(OpAssign& op) override;
    void visit(OpBin& op) override;
    void visit(OpUnary& op) override;
    void visit(Function& func) override;
    void visit(Macro& macro) override;
    void visit(ExprBlock& block) override;
    void visit(ExprCondition& expr) override;
    void visit(ExprIf& expr) override;
    void visit(ExprWhile& expr) override;
    void visit(ExprForRange& expr) override;
    void visit(ExprRange& expr) override;
    void visit(ExprVector& expr) override;
    void visit(ExprCatHorizontal& expr) override;
    void visit(ExprCatVertical& expr) override;
    void visit(ExprCall& expr) override;
    void visit(ExprMacroExpand& expr) override;
    void visit(ExprAttribute& expr) override;
    void visit(ExprCalculation& expr) override;
    void visit(Expr& expr) override;
public:
    void visitValue(Value& value);
public:
    Expr* takeResult()  {return result_.take();}
    void setResult(Expr* result) {result_ = result;}
private:
    SharedPtr<Expr> result_;
};


/*! @} */

AST_NAMESPACE_END

