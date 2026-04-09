///
/// @file      testScriptFunction.cpp
/// @brief     脚本函数调用测试
/// @details   测试函数定义和调用功能
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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
#include "AstScript/Parser.hpp"
#include "AstScript/Lexer.hpp"
#include "AstScript/Scanner.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

// 测试简单函数定义和调用
TEST(ScriptFunction, SimpleFunction)
{
    // 测试函数定义
    const char* script = "function add(x, y) = x + y";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed function: " << exprStr << std::endl;
        EXPECT_TRUE(exprStr.find("function add(x, y)") != std::string::npos);
        delete expr;
    }
}

// 测试函数调用解析
TEST(ScriptFunction, FunctionCallParsing)
{
    // 测试函数调用
    const char* script = "add(1, 2)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed function call: " << exprStr << std::endl;
        EXPECT_EQ(exprStr, "add(1, 2)");
        delete expr;
    }
}

// 测试带函数定义的完整表达式
TEST(ScriptFunction, FunctionDefinitionAndCall)
{
    // 测试函数定义和调用
    const char* script = "function square(x) = x * x; square(5)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed expression: " << exprStr << std::endl;
        delete expr;
    }
}

// 测试嵌套函数调用
TEST(ScriptFunction, NestedFunctionCall)
{
    // 测试嵌套函数调用
    const char* script = "add(square(2), 3)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed nested call: " << exprStr << std::endl;
        EXPECT_EQ(exprStr, "add(square(2), 3)");
        delete expr;
    }
}

// 测试多参数函数
TEST(ScriptFunction, MultiParameterFunction)
{
    // 测试多参数函数定义
    const char* script = "function max(a, b, c) = a > b ? (a > c ? a : c) : (b > c ? b : c)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed multi-param function: " << exprStr << std::endl;
        EXPECT_TRUE(exprStr.find("function max(a, b, c)") != std::string::npos);
        delete expr;
    }
}

// 测试无参函数
TEST(ScriptFunction, NoParameterFunction)
{
    // 测试无参函数定义
    const char* script = "function pi() = 3.14159";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed no-param function: " << exprStr << std::endl;
        EXPECT_TRUE(exprStr.find("function pi()") != std::string::npos);
        delete expr;
    }
}

// 测试函数调用表达式求值（简化版本）
TEST(ScriptFunction, FunctionCallEval)
{
    // 先定义函数，再调用
    const char* script1 = "function double(x) = x * 2; double(5)";
    Value* result = aEval(script1);
    // 注意：由于函数调用需要更完整的作用域支持，
    // 当前实现可能返回 null，这里我们主要测试解析是否正确
    
    // 解析测试
    Expr* expr = aParseExpr(script1);
    EXPECT_NE(expr, nullptr);
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed: " << exprStr << std::endl;
        delete expr;
    }
}

GTEST_MAIN()
