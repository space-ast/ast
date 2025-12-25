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
#include "AstScript/ExprCondition.hpp"
#include "AstScript/Parser.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/OpBinPredefined.hpp"
#include "AstScript/OpAssignPredefined.hpp"
#include "AstScript/OpUnaryPredefined.hpp"
#include "AstScript/Types.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Quantity.hpp"

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

Expr *aNewOpAssign(EOpAssignType op, Expr *left, Expr *right)
{
    if(!left || !right){
        return nullptr;
    }
    return new OpAssign(op, left, right);
}

Expr *aNewOpBin(EOpBinType op, Expr *left, Expr *right)
{
    if(!left || !right){
        return nullptr;
    }
    return new OpBin(op, left, right);
}

Expr *aNewOpUnary(EOpUnaryType op, Expr *expr)
{
    if(!expr){
        return nullptr;
    }
    return new OpUnary(op, expr);
}

Expr* aNewExprCondition(Expr* condition, Expr* thenExpr, Expr* elseExpr)
{
    if(!condition || !thenExpr ){
        return nullptr;
    }
    return new ExprCondition(condition, thenExpr, elseExpr);
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
    return (Value*)expr.take();
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
    return value && (value)->type() == &aValBool_Type;
}

bool aValueIsDouble(Value *value)
{
    return value && (value)->type() == &aValDouble_Type;
}

bool aValueIsInt(Value *value)
{
    return value && (value)->type() == &aValInt_Type;
}

bool aValueIsQuantity(Value *value)
{
    return value && (value)->type() == &aValQuantity_Type;
}


bool aValueIsQuantity(Value* value);

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

Quantity aValueUnboxQuantity(Value *value)
{
    if(!aValueIsQuantity(value)){
        aError("Value is not a quantity");
        return Quantity();
    }
    return static_cast<ValQuantity*>(value)->quantity();
}

std::string aFormatExpr(Expr *expr, Object *context)
{
    return expr->getExpression(context);
}

OpBinFunc aGetOpBinFunc(EOpBinType op, Class *leftType, Class *rightType)
{
    return opbin_get_func(op, leftType, rightType);
}

OpAssignFunc aGetOpAssignFunc(EOpAssignType op, Class *leftType, Class *rightType)
{
    return opassign_get_func(op, leftType, rightType);
}

Value* aDoOpUnary(EOpUnaryType op, Value* value)
{
    return opunary(op, value);
}

static void assignop_split(EOpAssignType op, EOpBinType& opbin)
{
    switch (op)
    {
    case eAddAssign:
        opbin = eAdd;
        break;
    case eSubAssign:
        opbin = eSub;
        break;
    case eMulAssign:
        opbin = eMul;
        break;
    case eDivAssign:
        opbin = eDiv;
        break;;
    case eModAssign:
        opbin = eMod;
        break;
    case ePowAssign:
        opbin = ePow;
        break;
    case eElemMulAssign:
        opbin = eElemMul;
        break;
    case eElemDivAssign:
        opbin = eElemDiv;
        break;
    case eElemModAssign:
        opbin = eElemMod;
        break;
    case eElemPowAssign:
        opbin = eElemPow;
        break;
    case eElemAndAssign:
        opbin = eElemAnd;
        break;
    case eElemOrAssign:
        opbin = eElemOr;
        break;
    default:
        break;
    }
    opbin = invalidOpBin; // 无效运算符
}

Value *aDoOpAssign(EOpAssignType op, Expr *left, Expr *right)
{
    if(!left || !right){
        aError("Left or right is null");
        return nullptr;
    }

    switch (op)
    {
    case eAssign:
    {
        left->setValue(right->eval());
        return left->eval();
    }
    case eDelayAssign:
    {
        if(auto var = dynamic_cast<Variable*>(left)){
            var->setExpr(right);
        }else{
            aError("Left is not a variable");
            return nullptr;
        }
        return left->eval();
    }
    case eBindAssign:
    {
        if(auto var = dynamic_cast<Variable*>(left)){
            var->bind(right);
        }else{
            aError("Left is not a variable");
            return nullptr;
        }
        return left->eval();
    }
    default:
        break;
    }
    // 其他赋值运算符
    {
        EOpBinType opbin;
        assignop_split(op, opbin);
        if(opbin == invalidOpBin){
            aError("Invalid assign operator");
            return nullptr;
        }
        auto val = aDoOpBin(opbin, left->eval(), right->eval());
        left->setValue(val);
        return val;
    }
}

OpUnaryFunc aGetOpUnaryFunc(EOpUnaryType op, Class *type)
{
    return opunary_get_func(op, type);
}

Value *aDoOpBin(EOpBinType op, Value *left, Value *right)
{
    return opbin(op, left, right);
}



AST_NAMESPACE_END