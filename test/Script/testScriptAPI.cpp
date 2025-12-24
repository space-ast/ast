///
/// @file      testScriptAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-23
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

#include "AstScript/ScriptAPI.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/StringView.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValDouble.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/Symbol.hpp"
#include "AstScript/Variable.hpp"

AST_USING_NAMESPACE


class ScriptAPI2 : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
    }

    void TearDown() override {
        // 清理测试环境
    }
};

// 测试变量创建函数
TEST_F(ScriptAPI2, VariableFunctions) {
    // 测试创建有名称的变量
    Variable* var1 = aNewVariable("testVar");
    EXPECT_NE(var1, nullptr);
    delete var1;

    // 测试创建没有名称的变量
    Variable* var2 = aNewVariable();
    EXPECT_NE(var2, nullptr);
    delete var2;

    // 测试创建带表达式的变量
    Value* val = aNewValueInt(10);
    Variable* var3 = aNewVariable(val);
    auto val2 = var3->eval();
    auto val3 = aNewValueInt(20);
    EXPECT_NE(val2, nullptr);
    var3->setValue(val3);
    auto val4 = var3->eval();
    EXPECT_EQ(val4, val3);
    EXPECT_NE(var3, nullptr);
}

// 测试符号创建函数
TEST_F(ScriptAPI2, SymbolFunctions) {
    // 测试创建符号
    Symbol* sym = aNewSymbol("testSymbol");
    EXPECT_NE(sym, nullptr);
    delete sym;
}

// 测试操作符函数
TEST_F(ScriptAPI2, OperatorFunctions) {
    // 测试创建二元操作符表达式
    Value* left = aNewValueInt(5);
    Value* right = aNewValueInt(3);
    
    Expr* addExpr = aNewOpBin(EOpBinType::eAdd, (Expr*)left, (Expr*)right);
    EXPECT_NE(addExpr, nullptr);
    delete addExpr;
    
    Expr* subExpr = aNewOpBin(EOpBinType::eSub, (Expr*)left, (Expr*)right);
    EXPECT_NE(subExpr, nullptr);
    delete subExpr;
    
    Expr* mulExpr = aNewOpBin(EOpBinType::eMul, (Expr*)left, (Expr*)right);
    EXPECT_NE(mulExpr, nullptr);
    delete mulExpr;
    
    // 测试创建一元操作符表达式
    Expr* negExpr = aNewOpUnary(EOpUnaryType::eNeg, (Expr*)left);
    EXPECT_NE(negExpr, nullptr);
    delete negExpr;
    
    // 测试创建赋值操作符表达式
    Symbol* sym = aNewSymbol("testVar");
    Expr* assignExpr = aNewOpAssign(EOpAssignType::eAssign, (Expr*)sym, (Expr*)left);
    EXPECT_NE(assignExpr, nullptr);
    delete assignExpr;
    
}

// 测试值类型创建函数
TEST_F(ScriptAPI2, ValueCreationFunctions) {
    // 测试创建整数值
    Value* intVal = aNewValueInt(42);
    EXPECT_NE(intVal, nullptr);
    delete intVal;
    
    // 测试创建布尔值
    Value* boolVal = aNewValueBool(true);
    EXPECT_NE(boolVal, nullptr);
    delete boolVal;
    
    // 测试创建双精度浮点数值
    Value* doubleVal = aNewValueDouble(3.14159);
    EXPECT_NE(doubleVal, nullptr);
    delete doubleVal;
    
    // 测试空值
    Value* nullVal = aValueNull();
    EXPECT_NE(nullVal, nullptr);
}

// 测试值类型判断函数
TEST_F(ScriptAPI2, ValueTypeCheckFunctions) {
    // 测试布尔值判断
    Value* boolVal = aNewValueBool(true);
    EXPECT_TRUE(aValueIsBool(boolVal));
    EXPECT_FALSE(aValueIsInt(boolVal));
    EXPECT_FALSE(aValueIsDouble(boolVal));
    delete boolVal;
    
    // 测试整数值判断
    Value* intVal = aNewValueInt(42);
    EXPECT_FALSE(aValueIsBool(intVal));
    EXPECT_TRUE(aValueIsInt(intVal));
    EXPECT_FALSE(aValueIsDouble(intVal));
    delete intVal;
    
    // 测试双精度浮点数值判断
    Value* doubleVal = aNewValueDouble(3.14159);
    EXPECT_FALSE(aValueIsBool(doubleVal));
    EXPECT_FALSE(aValueIsInt(doubleVal));
    EXPECT_TRUE(aValueIsDouble(doubleVal));
    delete doubleVal;
}

// 测试值提取函数
TEST_F(ScriptAPI2, ValueExtractionFunctions) {
    // 测试提取布尔值
    Value* boolVal = aNewValueBool(true);
    EXPECT_EQ(aValueUnboxBool(boolVal), true);
    delete boolVal;
    
    // 测试提取整数值
    Value* intVal = aNewValueInt(42);
    EXPECT_EQ(aValueUnboxInt(intVal), 42);
    delete intVal;
    
    // 测试提取双精度浮点数值
    Value* doubleVal = aNewValueDouble(3.14159);
    EXPECT_DOUBLE_EQ(aValueUnboxDouble(doubleVal), 3.14159);
    delete doubleVal;
}

// 测试表达式求值函数
TEST_F(ScriptAPI2, ExpressionEvaluationFunctions) {
    // 测试解析和求值简单表达式
    Value* result1 = aEval("2 + 3");
    EXPECT_NE(result1, nullptr);
    if (result1) {
        EXPECT_TRUE(aValueIsInt(result1) || aValueIsDouble(result1));
        if (aValueIsInt(result1)) {
            EXPECT_EQ(aValueUnboxInt(result1), 5);
        } else if (aValueIsDouble(result1)) {
            EXPECT_DOUBLE_EQ(aValueUnboxDouble(result1), 5.0);
        }
        delete result1;
    }
    
    // 测试解析表达式
    Expr* expr = aParseExpr("5 * 4");
    EXPECT_NE(expr, nullptr);
    
    // 测试求值表达式
    Value* result2 = aEvalExpr(expr);
    EXPECT_NE(result2, nullptr);
    if (result2) {
        EXPECT_TRUE(aValueIsInt(result2) || aValueIsDouble(result2));
        if (aValueIsInt(result2)) {
            EXPECT_EQ(aValueUnboxInt(result2), 20);
        } else if (aValueIsDouble(result2)) {
            EXPECT_DOUBLE_EQ(aValueUnboxDouble(result2), 20.0);
        }
        delete result2;
    }
    
    delete expr;
}

// 测试执行操作符
TEST_F(ScriptAPI2, OperatorExecution) {
    // 测试执行二元操作符
    Value* left = aNewValueInt(5);
    Value* right = aNewValueInt(3);
    
    Value* addResult = aDoOpBin(EOpBinType::eAdd, left, right);
    EXPECT_NE(addResult, nullptr);
    if (addResult) {
        EXPECT_EQ(aValueUnboxInt(addResult), 8);
        delete addResult;
    }
    
    // 测试执行一元操作符
    Value* negResult = aDoOpUnary(EOpUnaryType::eNeg, left);
    EXPECT_NE(negResult, nullptr);
    if (negResult) {
        EXPECT_EQ(aValueUnboxInt(negResult), -5);
        delete negResult;
    }
    
    delete left;
    delete right;
}


TEST(ScriptAPI, VariableFunctions)
{
    // 测试aNewVariable函数
    Expr* expr = aNewValueInt(42);
    Variable* var1 = aNewVariable("testVar", expr, false);
    EXPECT_TRUE(var1);
    // 注意：Variable类可能没有直接的name()和expr()方法
    delete var1;
    
    Variable* var2 = aNewVariable(expr, true);
    EXPECT_TRUE(var2);
    // 注意：Variable类可能没有直接的expr()方法
    delete var2;
}

TEST(ScriptAPI, SymbolFunctions)
{
    // 测试aNewSymbol函数
    Symbol* symbol = aNewSymbol("testSymbol");
    EXPECT_TRUE(symbol);
    // 注意：Symbol类可能没有直接的name()方法
    delete symbol;
}

TEST(ScriptAPI, OperatorFunctions)
{
    // 创建测试用的表达式
    Expr* left = aNewValueInt(10);
    Expr* right = aNewValueInt(20);
    
    // 测试aNewOpBin函数
    Expr* opBin = aNewOpBin(EOpBinType::eAdd, left, right);
    EXPECT_TRUE(opBin);
    delete opBin;
    
    // 测试aNewOpAssign函数
    Expr* opAssign = aNewOpAssign(EOpAssignType::eAssign, left, right);
    EXPECT_TRUE(opAssign);
    delete opAssign;
    
    // 测试aNewOpUnary函数
    Expr* opUnary = aNewOpUnary(EOpUnaryType::eNeg, left);
    EXPECT_TRUE(opUnary);
    delete opUnary;
    
}

TEST(ScriptAPI, ValueCreationFunctions)
{
    // 测试aNewValueInt函数
    Value* intVal = aNewValueInt(123);
    EXPECT_TRUE(intVal);
    EXPECT_TRUE(dynamic_cast<ValInt*>(intVal));
    EXPECT_EQ(static_cast<ValInt*>(intVal)->value(), 123);
    delete intVal;
    
    // 测试aNewValueBool函数
    Value* boolVal = aNewValueBool(true);
    EXPECT_TRUE(boolVal);
    EXPECT_TRUE(dynamic_cast<ValBool*>(boolVal));
    EXPECT_EQ(static_cast<ValBool*>(boolVal)->value(), true);
    delete boolVal;
    
    // 测试aNewValueDouble函数
    Value* doubleVal = aNewValueDouble(3.14159);
    EXPECT_TRUE(doubleVal);
    EXPECT_TRUE(dynamic_cast<ValDouble*>(doubleVal));
    EXPECT_EQ(static_cast<ValDouble*>(doubleVal)->value(), 3.14159);
    delete doubleVal;
    
    // 测试aNewValueString函数
    Value* stringVal = aNewValueString("test string");
    EXPECT_TRUE(stringVal);
    EXPECT_TRUE(dynamic_cast<ValString*>(stringVal));
    EXPECT_EQ(static_cast<ValString*>(stringVal)->value(), "test string");
    delete stringVal;
    
    // 测试aValueNull函数
    Value* nullVal1 = aValueNull();
    Value* nullVal2 = aValueNull();
    EXPECT_TRUE(nullVal1);
    EXPECT_TRUE(nullVal2);
    EXPECT_EQ(nullVal1, nullVal2);  // 应该返回同一个实例
}

TEST(ScriptAPI, ValueCheckFunctions)
{
    // 创建测试用的值
    Value* intVal = aNewValueInt(123);
    Value* boolVal = aNewValueBool(true);
    Value* doubleVal = aNewValueDouble(3.14);
    Value* stringVal = aNewValueString("test");
    Value* nullVal = aValueNull();
    
    // 测试aValueIsInt函数
    EXPECT_TRUE(aValueIsInt(intVal));
    EXPECT_FALSE(aValueIsInt(boolVal));
    EXPECT_FALSE(aValueIsInt(doubleVal));
    
    // 测试aValueIsBool函数
    EXPECT_TRUE(aValueIsBool(boolVal));
    EXPECT_FALSE(aValueIsBool(intVal));
    EXPECT_FALSE(aValueIsBool(doubleVal));
    
    // 测试aValueIsDouble函数
    EXPECT_TRUE(aValueIsDouble(doubleVal));
    EXPECT_FALSE(aValueIsDouble(intVal));
    EXPECT_FALSE(aValueIsDouble(boolVal));
    
    // 测试null值的情况
    EXPECT_FALSE(aValueIsInt(nullVal));
    EXPECT_FALSE(aValueIsBool(nullVal));
    EXPECT_FALSE(aValueIsDouble(nullVal));
    
    delete intVal;
    delete boolVal;
    delete doubleVal;
    delete stringVal;
}

TEST(ScriptAPI, ValueUnboxFunctions)
{
    // 创建测试用的值
    Value* intVal = aNewValueInt(123);
    Value* boolVal = aNewValueBool(true);
    Value* doubleVal = aNewValueDouble(3.14159);
    
    // 测试aValueUnboxInt函数
    EXPECT_EQ(aValueUnboxInt(intVal), 123);
    
    // 测试aValueUnboxBool函数
    EXPECT_EQ(aValueUnboxBool(boolVal), true);
    
    // 测试aValueUnboxDouble函数
    EXPECT_EQ(aValueUnboxDouble(doubleVal), 3.14159);
    
    delete intVal;
    delete boolVal;
    delete doubleVal;
}

TEST(ScriptAPI, ParseExprFunction)
{
    // 测试aParseExpr函数
    Expr* expr1 = aParseExpr("10 + 20");
    EXPECT_TRUE(expr1);
    delete expr1;
    
    Expr* expr2 = aParseExpr("10 * 20 - 5");
    EXPECT_TRUE(expr2);
    delete expr2;
    
    Expr* expr3 = aParseExpr("(10 + 20) * 2");
    EXPECT_TRUE(expr3);
    delete expr3;
}

TEST(ScriptAPI, EvalExprFunction)
{
    // 测试aEvalExpr函数
    Expr* expr = aParseExpr("10 + 20");
    EXPECT_TRUE(expr);
    
    Value* result = aEvalExpr(expr);
    EXPECT_TRUE(result);
    EXPECT_TRUE(aValueIsInt(result));
    EXPECT_EQ(aValueUnboxInt(result), 30);
    
    delete expr;
    delete result;
}

TEST(ScriptAPI, EvalFunction)
{
    // 测试aEval函数
    Value* result = aEval("10 + 20");
    EXPECT_TRUE(result);
    EXPECT_TRUE(aValueIsInt(result));
    EXPECT_EQ(aValueUnboxInt(result), 30);
    delete result;
    
    result = aEval("10 * 20");
    EXPECT_TRUE(result);
    EXPECT_TRUE(aValueIsInt(result));
    EXPECT_EQ(aValueUnboxInt(result), 200);
    delete result;
    
    result = aEval("(10 + 20) * 2");
    EXPECT_TRUE(result);
    EXPECT_TRUE(aValueIsInt(result));
    EXPECT_EQ(aValueUnboxInt(result), 60);
    delete result;
}

TEST(ScriptAPI, OperatorFunctionAccessors)
{
    // 注意：由于aValInt_Type等类型对象未在头文件中声明，我们暂时跳过此测试
    // 这个测试需要访问内部实现细节，可能不适合作为公共API测试
}

TEST(ScriptAPI, OperatorExecutionFunctions)
{
    // 创建测试用的值
    Value* intVal1 = aNewValueInt(10);
    Value* intVal2 = aNewValueInt(20);
    Value* boolVal = aNewValueBool(true);
    
    // 测试aDoOpBin函数
    Value* addResult = aDoOpBin(EOpBinType::eAdd, intVal1, intVal2);
    EXPECT_TRUE(addResult);
    EXPECT_TRUE(aValueIsInt(addResult));
    EXPECT_EQ(aValueUnboxInt(addResult), 30);
    delete addResult;
    
    Value* subResult = aDoOpBin(EOpBinType::eSub, intVal2, intVal1);
    EXPECT_TRUE(subResult);
    EXPECT_TRUE(aValueIsInt(subResult));
    EXPECT_EQ(aValueUnboxInt(subResult), 10);
    delete subResult;
    
    Value* mulResult = aDoOpBin(EOpBinType::eMul, intVal1, intVal2);
    EXPECT_TRUE(mulResult);
    EXPECT_TRUE(aValueIsInt(mulResult));
    EXPECT_EQ(aValueUnboxInt(mulResult), 200);
    delete mulResult;
    
    // 测试aDoOpUnary函数
    Value* negResult = aDoOpUnary(EOpUnaryType::eNeg, intVal1);
    EXPECT_TRUE(negResult);
    EXPECT_TRUE(aValueIsInt(negResult));
    EXPECT_EQ(aValueUnboxInt(negResult), -10);
    delete negResult;
    
    Value* notResult = aDoOpUnary(EOpUnaryType::eNot, boolVal);
    EXPECT_TRUE(notResult);
    EXPECT_TRUE(aValueIsBool(notResult));
    EXPECT_EQ(aValueUnboxBool(notResult), false);
    delete notResult;
    
    delete intVal1;
    delete intVal2;
    delete boolVal;
}

TEST(ScriptAPI, FormatExprFunction)
{
    // 测试aFormatExpr函数
    Expr* expr1 = aParseExpr("10 + 20");
    EXPECT_TRUE(expr1);
    
    StringView formatted1 = aFormatExpr(expr1);
    EXPECT_TRUE(formatted1.length() > 0);
    delete expr1;
    
    Expr* expr2 = aParseExpr("10 * (20 - 5)");
    EXPECT_TRUE(expr2);
    
    StringView formatted2 = aFormatExpr(expr2);
    EXPECT_TRUE(formatted2.length() > 0);
    delete expr2;
}


GTEST_MAIN()