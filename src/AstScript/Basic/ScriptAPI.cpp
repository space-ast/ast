///
/// @file      ScriptAPI.cpp
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

#include "ScriptAPI.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/ValDouble.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/ValQuantity.hpp"
#include "AstScript/ValNull.hpp"
#include "AstScript/Variable.hpp"
#include "AstScript/OpBin.hpp"
#include "AstScript/OpAssign.hpp"
#include "AstScript/OpUnary.hpp"
#include "AstScript/Parser.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/OpBinPredefined.hpp"
#include "AstScript/OpAssignPredefined.hpp"
#include "AstScript/OpUnaryPredefined.hpp"
#include "AstScript/OpUnaryPredefined.hpp"
#include "AstScript/Types.hpp"
#include "AstUtil/SharedPtr.hpp"

AST_NAMESPACE_BEGIN


Variable *aNewVariable(StringView name, Expr *expr, bool bind)
{
    return new Variable(name, expr, bind);
}

Variable *aNewVariable(Expr *expr, bool bind)
{
    return new Variable(expr, bind);
}

Symbol *aNewSymbol(StringView name)
{
    return new Symbol(name);
}

Expr *aNewOpAssign(OpAssignType op, Expr *left, Expr *right)
{
    if(!left || !right){
        return nullptr;
    }
    return new OpAssign(op, left, right);
}

Expr *aNewOpBin(OpBinType op, Expr *left, Expr *right)
{
    if(!left || !right){
        return nullptr;
    }
    return new OpBin(op, left, right);
}

Expr *aNewOpUnary(OpUnaryType op, Expr *expr)
{
    if(!expr){
        return nullptr;
    }
    return new OpUnary(op, expr);
}


Value* aNewValueString(StringView value)
{
    return new ValString(value);
}
Value* aNewValueInt(int value)
{
    return new ValInt(value);
}
Value* aNewValueBool(bool value)
{
    return new ValBool(value);
}
Value* aNewValueDouble(double value)
{
    return new ValDouble(value);
}

Value *aNewValueQuantity(const Quantity &value)
{
    return new ValQuantity(value);
}

Value *aValueNull()
{
    static SharedPtr<Value> nullValue{new ValNull{}};
    return nullValue.get();
}


Expr *aParseExpr(StringView script)
{
    return Parser::parseExpr(script);
}

Value *aEval(StringView script)
{
    SharedPtr<Expr> expr = Parser::parseExpr(script);
    if(!expr.get()){
        return nullptr;
    }
    expr = expr->eval();
    return (Value*)expr.taken();
}

Value *aEvalExpr(Expr *expr)
{
    if(!expr){
        return nullptr;
    }
    return expr->eval();
}

bool aValueIsBool(Value *value)
{
    return value && static_cast<ValBool*>(value)->type() == &aValBool_Type;
}

bool aValueIsDouble(Value *value)
{
    return value && static_cast<ValDouble*>(value)->type() == &aValDouble_Type;
}

bool aValueIsInt(Value *value)
{
    return value && static_cast<ValInt*>(value)->type() == &aValInt_Type;
}



bool aValueUnboxBool(Value *value)
{
    if(!aValueIsBool(value)){
        aError("Value is not a bool");
        return false;
    }
    return static_cast<ValBool*>(value)->value();
}

double aValueUnboxDouble(Value *value)
{
    if(!aValueIsDouble(value)){
        aError("Value is not a double");
        return 0.0;
    }
    return static_cast<ValDouble*>(value)->value();
}

int aValueUnboxInt(Value *value)
{
    if(!aValueIsInt(value)){
        aError("Value is not an int");
        return 0;
    }
    return static_cast<ValInt*>(value)->value();
}

std::string aFormatExpr(Expr *expr, Object *context)
{
    return expr->getExpression(context);
}

OpBinFunc aGetOpBinFunc(OpBinType op, Class *leftType, Class *rightType)
{
    return opbin_get_func(op, leftType, rightType);
}

OpAssignFunc aGetOpAssignFunc(OpAssignType op, Class *leftType, Class *rightType)
{
    return opassign_get_func(op, leftType, rightType);
}

Value* aDoOpUnary(OpUnaryType op, Value* value)
{
    return opunary(op, value);
}

OpUnaryFunc aGetOpUnaryFunc(OpUnaryType op, Class *type)
{
    return opunary_get_func(op, type);
}

Value *aDoOpBin(OpBinType op, Value *left, Value *right)
{
    return opbin(op, left, right);
}



AST_NAMESPACE_END