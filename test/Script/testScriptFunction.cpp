///
/// @file      testScriptFunction.cpp
/// @brief     Julia风格脚本函数测试
/// @details   测试函数定义和调用功能
/// @author    axel
/// @date      2025-12-19
/// @copyright 版权所有 (C) 2025-present, ast项目.

#include "AstScript/ScriptAPI.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

// 测试Julia标准函数定义语法
TEST(ScriptFunction, StandardSyntax)
{
    // 标准语法: function name(params) ... end
    const char* script = "function add(x, y)\n    x + y\nend";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed: " << exprStr << std::endl;
        EXPECT_TRUE(exprStr.find("function add(x, y)") != std::string::npos);
        EXPECT_TRUE(exprStr.find("end") != std::string::npos);
        delete expr;
    }
}

// 测试函数调用解析
TEST(ScriptFunction, FunctionCallParsing)
{
    const char* script = "add(1, 2)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed call: " << exprStr << std::endl;
        EXPECT_EQ(exprStr, "add(1, 2)");
        delete expr;
    }
}

// 测试带return的函数
TEST(ScriptFunction, WithReturn)
{
    const char* script = "function max(a, b)\n    if a > b\n        return a\n    else\n        return b\n    end\nend";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed with return: " << exprStr << std::endl;
        delete expr;
    }
}

// 测试嵌套函数调用
TEST(ScriptFunction, NestedCall)
{
    const char* script = "add(square(2), 3)";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed nested: " << exprStr << std::endl;
        EXPECT_EQ(exprStr, "add(square(2), 3)");
        delete expr;
    }
}

// 测试多参数函数
TEST(ScriptFunction, MultiParam)
{
    const char* script = "function max(a, b, c)\n    if a > b\n        if a > c\n            a\n        else\n            c\n        end\n    else\n        if b > c\n            b\n        else\n            c\n        end\n    end\nend";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed multi-param: " << exprStr << std::endl;
        delete expr;
    }
}

// 测试无参函数
TEST(ScriptFunction, NoParam)
{
    const char* script = "function pi()\n    3.14159\nend";
    Expr* expr = aParseExpr(script);
    EXPECT_NE(expr, nullptr);
    
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "Parsed no-param: " << exprStr << std::endl;
        EXPECT_TRUE(exprStr.find("function pi()") != std::string::npos);
        delete expr;
    }
}

GTEST_MAIN()
