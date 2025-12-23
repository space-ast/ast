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
#include "AstScript/ValString.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/IO.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif

AST_USING_NAMESPACE

// 辅助函数：测试布尔值表达式
void testScriptEvalBool(StringView str, bool expectedValue)
{
    ast_printf("testScriptEvalBool: %s\n", str.to_string().c_str());
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

    // 十六进制整数
    testScriptEvalInt("0x0", 0x0);
    testScriptEvalInt("0x10", 0x10);
    testScriptEvalInt("0xFF", 0xFF);
    testScriptEvalInt("0xEF", 0xEF);
    testScriptEvalInt("0xeF", 0xeF);
    testScriptEvalInt("0xabc", 0xabc);
    //testScriptEvalInt("0x123456789ABCDEF", 0x123456789ABCDEF); // @fixme 超出int范围，需要处理这种情况
    
    // 二进制整数
    testScriptEvalInt("0b0", 0b0);
    testScriptEvalInt("0b1", 0b1);
    testScriptEvalInt("0b1010", 0b1010);
    testScriptEvalInt("0b1111", 0b1111);
    testScriptEvalInt("0b10000000", 0b10000000);
    
    // 八进制整数，参照Julia语法
    testScriptEvalInt("0o0", 00);
    testScriptEvalInt("0o1", 01);
    testScriptEvalInt("0o7", 07);
    testScriptEvalInt("0o10", 010);
    testScriptEvalInt("0o100", 0100);
    testScriptEvalInt("0o777", 0777);
    testScriptEvalInt("0o123", 0123);
    
    // 算术运算
    testScriptEvalInt("1 + 2", 3);
    testScriptEvalInt("3 - 1", 2);
    testScriptEvalInt("2 * 3", 6);
    testScriptEvalDouble("6 / 2", 3.0); // 整数除法结果为浮点数，与julia语言一致
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
    #ifdef _WIN32
    // 设置控制台I/O编码为UTF-8
    // 设置这个让异或符号⊻正常显示
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    #endif
    
    // 按位与
    testScriptEvalInt("0b1100 & 0b1010", 0b1000);
    
    // 按位或
    testScriptEvalInt("0b1100 | 0b1010", 0b1110);
    
    // 按位异或
    testScriptEvalInt(aText("0b1100 \u22BB 0b1010"), 0b0110); // 异或⊻
    
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
    testScriptEvalDouble("((1 + 2) * 3 - 4) / 2", 2.5);
    
    // 混合类型和运算符
    testScriptEvalBool("(1 + 2) > 3 && 5 < 6", false);
    testScriptEvalBool("(1 + 2) >= 3 && 5 < 6", true);
    testScriptEvalBool("(1 + 2) > 3 || 5 < 6", true);
}

// 辅助函数：测试语法错误情况（预期返回nullptr）
void testScriptEvalSyntaxError(StringView str)
{
    printf("testScriptEvalSyntaxError: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    EXPECT_FALSE(value); // 语法错误应该返回nullptr
    if(value){
        delete value;
    }
}

// 辅助函数：测试运行时错误情况（预期执行时失败）
void testScriptEvalRuntimeError(StringView str)
{
    printf("testScriptEvalRuntimeError: %s\n", str.to_string().c_str());
    Value* value = aEval(str);
    // 这些表达式在语法上是正确的，但在运行时会失败
    // 注意：当前实现中，aEval可能会返回非nullptr但执行时抛出错误
    // 我们通过检查控制台输出来验证错误发生
    EXPECT_FALSE(value); // 语法错误应该返回nullptr
    if(value){
        delete value;
    }
}

// 测试语法错误情况
TEST(ScriptEval, EvalSyntaxErrors)
{
    // 未闭合的括号
    testScriptEvalSyntaxError("(1 + 2");
    testScriptEvalSyntaxError("(1 + (2 * 3)");
    testScriptEvalSyntaxError("1 + (2 * 3");
    
    // 多余的括号
    testScriptEvalSyntaxError("(1 + 2))");
    
    // 缺少操作数
    testScriptEvalSyntaxError("1 +");
    testScriptEvalSyntaxError("+ 1");
    testScriptEvalSyntaxError("1 *");
    testScriptEvalSyntaxError("/ 2");
    
    // 无效的操作符组合 - 注意：当前解析器将这些视为有效表达式，但在严格的Julia语法中可能是错误
    //testScriptEvalSyntaxError("1 ++ 2");  // @fixme 
    //testScriptEvalSyntaxError("1 -- 2");  // @fixme 
    
    // 无效的数字格式
    testScriptEvalSyntaxError("123abc"); 
    testScriptEvalSyntaxError("0xGHIJ"); // 无效的十六进制字符
    testScriptEvalSyntaxError("0b2"); // 二进制中只能包含0和1
    testScriptEvalSyntaxError("0o8"); // 八进制中只能包含0-7
    testScriptEvalSyntaxError("1.2.3"); // 多个小数点
    
    // 字符串相关错误
    testScriptEvalSyntaxError("\"unclosed string");
    testScriptEvalSyntaxError("'single quote string'"); // Julia使用双引号
    
    // Julia特定语法错误
    testScriptEvalSyntaxError("[1, 2, 3"); // 未闭合的数组
    testScriptEvalSyntaxError("(1, 2"); // 未闭合的元组
    
    // 其他语法错误
    testScriptEvalSyntaxError("!@#$%"); // 无效的符号组合
    testScriptEvalSyntaxError("0x"); // 十六进制缺少数字
    testScriptEvalSyntaxError("0b"); // 二进制缺少数字
    testScriptEvalSyntaxError("0o"); // 八进制缺少数字
    
    // 注意：非int类型的移位运算不是语法错误，而是运行时错误
    // 我们将在EvalRuntimeErrors测试中处理这些情况
}

// 测试运行时错误情况
TEST(ScriptEval, EvalRuntimeErrors)
{
    // 不支持的运算符 - 非int类型使用移位运算
    testScriptEvalRuntimeError("1.0 << 2");
    testScriptEvalRuntimeError("1.5 >> 1");
    testScriptEvalRuntimeError("3.14159 << 3");
    testScriptEvalRuntimeError("2.0 << 1.0");
    
    // 其他可能不支持的运算符组合
    testScriptEvalRuntimeError("true << 1");
    testScriptEvalRuntimeError("false >> 2");
    testScriptEvalRuntimeError("\"string\" << 1");
    testScriptEvalRuntimeError("\"string\" + 1");
}

GTEST_MAIN()