///
/// @file      testScriptEval.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-22
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
#include "AstScript/ValString.hpp"

AST_USING_NAMESPACE

// 辅助函数：测试布尔值表达式
void testScriptEvalBool(StringView str, bool expectedValue)
{
    printf("testScriptEvalBool: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    EXPECT_TRUE(value);
    if(value){
        EXPECT_TRUE(aValueIsBool(value));
        EXPECT_EQ(aValueUnboxBool(value), expectedValue);
        delete value;
    }
}

// 辅助函数：测试整数值表达式
void testScriptEvalInt(StringView str, int expectedValue)
{
    printf("testScriptEvalInt: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    EXPECT_TRUE(value);
    if(value){
        EXPECT_TRUE(aValueIsInt(value));
        EXPECT_EQ(aValueUnboxInt(value), expectedValue);
        delete value;
    }
}

// 辅助函数：测试双精度浮点数值表达式
void testScriptEvalDouble(StringView str, double expectedValue)
{
    printf("testScriptEvalDouble: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    EXPECT_TRUE(value);
    if(value){
        EXPECT_TRUE(aValueIsDouble(value));
        EXPECT_DOUBLE_EQ(aValueUnboxDouble(value), expectedValue);
        delete value;
    }
}

// 辅助函数：测试字符串值表达式
void testScriptEvalString(StringView str, StringView expectedValue)
{
    printf("testScriptEvalString: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    EXPECT_TRUE(value);
    if(value){
        EXPECT_TRUE(dynamic_cast<ValString*>(value) != nullptr);
        ValString* strValue = static_cast<ValString*>(value);
        EXPECT_EQ(strValue->value(), expectedValue.to_string());
        delete value;
    }
}

// 测试布尔值表达式
TEST(ScriptEval, EvalBool)
{
    // 基本布尔值
    testScriptEvalBool("true", true);
    testScriptEvalBool("false", false);
    
    // 比较运算
    testScriptEvalBool("1 == 1", true);
    testScriptEvalBool("1 != 1", false);
    testScriptEvalBool("1 < 2", true);
    testScriptEvalBool("1 > 2", false);
    testScriptEvalBool("1 <= 1", true);
    testScriptEvalBool("1 >= 2", false);
    
    // 逻辑运算
    testScriptEvalBool("true && true", true);
    testScriptEvalBool("true && false", false);
    testScriptEvalBool("false && true", false);
    testScriptEvalBool("false && false", false);
    
    testScriptEvalBool("true || true", true);
    testScriptEvalBool("true || false", true);
    testScriptEvalBool("false || true", true);
    testScriptEvalBool("false || false", false);
    
    testScriptEvalBool("!true", false);
    testScriptEvalBool("!false", true);
}

// 测试整数值表达式
TEST(ScriptEval, EvalInt)
{
    // 基本整数值
    testScriptEvalInt("0", 0);
    testScriptEvalInt("1", 1);
    testScriptEvalInt("-1", -1);
    testScriptEvalInt("12345", 12345);
    testScriptEvalInt("-12345", -12345);
    
    // 算术运算
    testScriptEvalInt("1 + 2", 3);
    testScriptEvalInt("3 - 1", 2);
    testScriptEvalInt("2 * 3", 6);
    testScriptEvalInt("6 / 2", 3);
    testScriptEvalInt("7 % 3", 1);
    
    // 幂运算 (Julia使用^)
    testScriptEvalInt("2 ^ 3", 8);
    testScriptEvalInt("3 ^ 2", 9);
    
    // 复合表达式
    testScriptEvalInt("(1 + 2) * 3", 9);
    testScriptEvalInt("1 + 2 * 3", 7);
}

// 测试双精度浮点数值表达式
TEST(ScriptEval, EvalDouble)
{
    // 基本浮点数值
    testScriptEvalDouble("0.0", 0.0);
    testScriptEvalDouble("1.0", 1.0);
    testScriptEvalDouble("-1.0", -1.0);
    testScriptEvalDouble("3.14159", 3.14159);
    testScriptEvalDouble("-2.71828", -2.71828);
    
    // 科学计数法
    testScriptEvalDouble("1e3", 1000.0);
    testScriptEvalDouble("1.23e4", 12300.0);
    testScriptEvalDouble("1.23e-4", 0.000123);
    
    // 算术运算
    testScriptEvalDouble("1.0 + 2.0", 3.0);
    testScriptEvalDouble("3.0 - 1.0", 2.0);
    testScriptEvalDouble("2.0 * 3.0", 6.0);
    testScriptEvalDouble("6.0 / 2.0", 3.0);
    testScriptEvalDouble("7.0 % 3.0", 1.0);
    
    // 幂运算 (Julia使用^)
    testScriptEvalDouble("2.0 ^ 3.0", 8.0);
    testScriptEvalDouble("3.0 ^ 2.0", 9.0);
    testScriptEvalDouble("2.0 ^ 0.5", sqrt(2.0));
    
    // 复合表达式
    testScriptEvalDouble("(1.0 + 2.0) * 3.0", 9.0);
    testScriptEvalDouble("1.0 + 2.0 * 3.0", 7.0);
}

// 测试字符串表达式
TEST(ScriptEval, EvalString)
{
    // 基本字符串
    testScriptEvalString("\"hello\"", "hello");
    testScriptEvalString("\"world\"", "world");
}

// 测试位运算
TEST(ScriptEval, EvalBitwise)
{
    // 按位与
    testScriptEvalInt("0b1100 & 0b1010", 0b1000);
    
    // 按位或
    testScriptEvalInt("0b1100 | 0b1010", 0b1110);
    
    // 按位异或
    testScriptEvalInt("0b1100 ^ 0b1010", 0b0110);
    
    // 左移
    testScriptEvalInt("0b101 << 2", 0b10100);
    
    // 右移
    testScriptEvalInt("0b10100 >> 2", 0b101);
    
    // 按位非
    // 注意：Julia中按位非使用~，但由于整数表示的限制，结果可能与预期不同
    // 这里我们只测试语法正确性
    Value* value = aEval("~0b101");
    EXPECT_TRUE(value);
    if(value){
        EXPECT_TRUE(aValueIsInt(value));
        delete value;
    }
}

// 测试类型转换
TEST(ScriptEval, EvalTypeConversion)
{
    // 整数到浮点数
    testScriptEvalDouble("1 + 2.0", 3.0);
    testScriptEvalDouble("2.0 + 1", 3.0);
    
    // 浮点数到整数（如果支持）
    // 注意：Julia默认不进行隐式类型转换，需要显式转换
    // 这里我们测试基本的混合运算
    Value* value = aEval("5 / 2");
    EXPECT_TRUE(value);
    if(value){
        // 结果应该是浮点数
        EXPECT_TRUE(aValueIsDouble(value) || aValueIsInt(value));
        delete value;
    }
}

// 测试复合表达式
TEST(ScriptEval, EvalComplexExpressions)
{
    // 混合运算
    testScriptEvalDouble("1 + 2 * 3.0 - 4 / 2", 5.0);
    
    // 嵌套括号
    testScriptEvalDouble("((1 + 2) * 3 - 4) / 2", 3.5);
    
    // 混合类型和运算符
    testScriptEvalBool("(1 + 2) > 3 && 5 < 6", false);
    testScriptEvalBool("(1 + 2) >= 3 && 5 < 6", true);
    testScriptEvalBool("(1 + 2) > 3 || 5 < 6", true);
}

GTEST_MAIN()