///
/// @file      ScriptAPI.cpp
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

#include "ScriptAPI.hpp"
#include "AstScript/AllHeaders.hpp"
#include "AstScript/IteratePredefined.hpp"
#include "AstScript/ExprExpandVisitor.hpp"
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

Expr *aNewExprRange(Expr *start, Expr *stop, Expr *step)
{
    if(!start || !stop){
        return nullptr;
    }
    return new ExprRange(start, stop, step);
}

Value *aNewValueString(StringView value)
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

Value* aNewValueDict()
{
    return new ValDict();
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

Expr* aExpandExpr(Expr* expr)
{
    if(!expr){
        return nullptr;
    }
    ExprExpandVisitor visitor;
    expr->accept(visitor);
    return visitor.takeResult();
}

Expr *aExpand(StringView script)
{
    SharedPtr<Expr> expr = aParseExpr(script);
    if(!expr){
        return nullptr;
    }
    expr = aExpandExpr(expr);
    return expr.take();
}

Value *aEval(StringView script)
{
    SharedPtr<Value> value;
    {
        SharedPtr<Expr> expr = aParseExpr(script);
        if(!expr.get()){
            return nullptr;
        }
        value = expr->eval();
    }
    return value.take();
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
    return value && (value)->type() == &ValBool::staticType;
}

bool aValueIsDouble(Value *value)
{
    return value && (value)->type() == &ValDouble::staticType;
}

bool aValueIsInt(Value *value)
{
    return value && (value)->type() == &ValInt::staticType;
}

bool aValueIsArithmetic(Value *value)
{
    if(!value){
        return false;
    }
    auto type = value->type();
    return type == &ValInt::staticType || type == &ValDouble::staticType || type == &ValBool::staticType;
}

bool aValueIsQuantity(Value *value)
{
    return value && (value)->type() == &ValQuantity::staticType;
}

bool aValueIsString(Value *value)
{
    return value && (value)->type() == &ValString::staticType;
}


double aValueToDouble(Value *value)
{
    return value->toDouble();
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
        opbin = invalidOpBin; // 无效运算符
        break;
    }
}

/// @brief 尝试将左值解析为变量并执行赋值/绑定操作
/// @return true 表示成功找到变量并执行操作，false 表示左值不是变量
static bool tryAssignToVar(Expr* left, Expr* right, bool useBind)
{
    // 外部已检查空指针，无需重复检查
    // if(!left || !right){
    //     return false;
    // }
    Variable* var = aobject_cast<Variable*>(left);
    if (!var) {
        Symbol* sym = aobject_cast<Symbol*>(left);
        if (sym) {
            var = aobject_cast<Variable*>(sym->resolve());
        }
    }
    if (var) {
        if (useBind)
            var->bind(right);
        else
            var->setExpr(right);
        return true;
    }
    return false;
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
        if (!tryAssignToVar(left, right, false)) {
            aError("Left is not a variable");
            return nullptr;
        }
        return left->eval();
    }
    case eBindAssign:
    {
        if (!tryAssignToVar(left, right, true)) {
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

IterateFunc aGetIterateFunc(Class *type)
{
    return iterate_get_func(type);
}


Value *aDoOpBin(EOpBinType op, Value *left, Value *right)
{
    return opbin(op, left, right);
}

Value *aIterateBegin(Value *container, int &index)
{
    return iterate_begin(container, index);
}

Value* aIterateNext(Value* container, int& index)
{
    return iterate_next(container, index);
}


AST_NAMESPACE_END

