///
/// @file      ExprVisitor.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-21
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

AST_NAMESPACE_BEGIN

class Expr; 
class Symbol; 
class Variable; 
class ValNull; 
class ValBool; 
class ValInt; 
class ValDouble; 
class ValString; 
class ValQuantity; 
class OpAssign; 
class OpBin; 
class OpUnary; 
class Function; 
class Macro; 
class ExprBlock; 
class ExprCondition; 
class ExprIf; 
class ExprWhile; 
class ExprForRange;
class ExprRange;
class ValRange;

class ExprVisitor{
public:
    virtual ~ExprVisitor() = default;
    // virtual void visit(Expr& expr) = 0;
    virtual void visit(Symbol& symbol) = 0;
    
    virtual void visit(Variable& var) = 0;
    virtual void visit(ValNull& val) = 0;
    virtual void visit(ValInt& val) = 0;
    virtual void visit(ValDouble& val) = 0;
    virtual void visit(ValString& val) = 0;
    virtual void visit(ValBool& val) = 0;
    virtual void visit(ValQuantity& val) = 0;
    virtual void visit(ValRange& val) = 0;
    
    virtual void visit(OpAssign& op) = 0;
    virtual void visit(OpBin& op) = 0;
    virtual void visit(OpUnary& op) = 0;

    virtual void visit(Function& func) = 0;
    virtual void visit(Macro& macro) = 0;
    
    virtual void visit(ExprBlock& block) = 0;
    virtual void visit(ExprCondition& expr) = 0;
    virtual void visit(ExprIf& expr) = 0;
    virtual void visit(ExprWhile& expr) = 0;
    virtual void visit(ExprForRange& expr) = 0;
    virtual void visit(ExprRange& expr) = 0;

};

AST_NAMESPACE_END