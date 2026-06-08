///
/// @file      ExprExpandVisitor.cpp
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

#include "ExprExpandVisitor.hpp"
#include "AstScript/AstScript.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN


void ExprExpandVisitor::visitValue(Value& value)
{
    setResult(&value);
}

void ExprExpandVisitor::visit(Symbol& symbol)
{
    setResult(symbol.resolve());
}
void ExprExpandVisitor::visit(Variable& var)
{
    setResult(&var);
}
void ExprExpandVisitor::visit(ValNull& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValInt& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValDouble& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValString& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValBool& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValQuantity& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValRange& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValDict& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(ValNamedVector& val)
{
    visitValue(val);
}
void ExprExpandVisitor::visit(OpAssign& op)
{
    setResult(new OpAssign(op.op(), aExpandExpr(op.left()), aExpandExpr(op.right())));
}
void ExprExpandVisitor::visit(OpBin& op)
{
    setResult(new OpBin(op.op(), aExpandExpr(op.left()), aExpandExpr(op.right())));
}
void ExprExpandVisitor::visit(OpUnary& op)
{
    setResult(new OpUnary(op.op(), aExpandExpr(op.expr())));
}
void ExprExpandVisitor::visit(Function& func)
{
    setResult(&func);
}
void ExprExpandVisitor::visit(Macro& macro)
{
    setResult(&macro);
}
void ExprExpandVisitor::visit(ExprBlock& block)
{
    auto& exprs = block.children();
    ExprBlock::VectorType expandedExprs;
    for(auto& expr: exprs){
        if(!expr)
            continue;
        expandedExprs.push_back(expr->expand());
    }
    setResult(new ExprBlock(expandedExprs));
}
void ExprExpandVisitor::visit(ExprCondition& expr)
{
    setResult(new ExprCondition(aExpandExpr(expr.condition()), aExpandExpr(expr.thenExpr()), aExpandExpr(expr.elseExpr())));
}
void ExprExpandVisitor::visit(ExprIf& expr)
{
    auto& conditionBranches = expr.conditionBranches();
    auto* elseBlock = expr.elseBlock();
    auto expanded = new ExprIf();
    for(auto& branch: conditionBranches){
        expanded->addElseif(aExpandExpr(branch.condition), aExpandExpr(branch.block));
    }
    if(elseBlock)
        expanded->setElse(aExpandExpr(elseBlock));
    setResult(expanded);
}
void ExprExpandVisitor::visit(ExprWhile& expr)
{
    setResult(new ExprWhile(aExpandExpr(expr.condition()), aExpandExpr(expr.body())));
}
void ExprExpandVisitor::visit(ExprForRange& expr)
{
    setResult(new ExprForRange(aExpandExpr(expr.variable()), aExpandExpr(expr.range()), aExpandExpr(expr.body())));
}
void ExprExpandVisitor::visit(ExprRange& expr)
{
    setResult(new ExprRange(aExpandExpr(expr.start()), aExpandExpr(expr.stop()), aExpandExpr(expr.step())));
}
void ExprExpandVisitor::visit(ExprVector& expr)
{
    auto& elems = expr.children();
    ExprContainer::VectorType expanded;
    for (auto& e : elems)
    {
        if (e)
            expanded.push_back(e->expand());
    }
    setResult(new ExprVector(expanded));
}
void ExprExpandVisitor::visit(ExprCatHorizontal& expr)
{
    auto& elems = expr.children();
    ExprContainer::VectorType expanded;
    for (auto& e : elems)
    {
        if (e)
            expanded.push_back(e->expand());
    }
    setResult(new ExprCatHorizontal(expanded));
}
void ExprExpandVisitor::visit(ExprCatVertical& expr)
{
    auto& elems = expr.children();
    ExprContainer::VectorType expanded;
    for (auto& e : elems)
    {
        if (e)
            expanded.push_back(e->expand());
    }
    setResult(new ExprCatVertical(expanded));
}
void ExprExpandVisitor::visit(ExprCall& expr)
{
    auto* expandedFunc = aExpandExpr(expr.getFunc());
    std::vector<SharedPtr<Expr>> expandedArgs;
    for (auto& arg : expr.getArgs())
    {
        if (arg)
            expandedArgs.push_back(arg->expand());
    }
    setResult(new ExprCall(expandedFunc, expandedArgs));
}
void ExprExpandVisitor::visit(ExprMacroExpand& expr)
{
    auto* expandedMacro = aExpandExpr(expr.getMacro());
    std::vector<SharedPtr<Expr>> expandedArgs;
    for (auto& arg : expr.getArgs())
    {
        if (arg)
            expandedArgs.push_back(arg->expand());
    }
    setResult(new ExprMacroExpand(expandedMacro, expandedArgs));
}
void ExprExpandVisitor::visit(ExprAttribute& expr)
{
    setResult(&expr);
}
void ExprExpandVisitor::visit(ExprCalculation& expr)
{
    setResult(&expr);
}
void ExprExpandVisitor::visit(Expr& expr)
{
    setResult(&expr);
}

AST_NAMESPACE_END
