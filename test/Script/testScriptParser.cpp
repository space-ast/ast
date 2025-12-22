///
/// @file      testScriptParser.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-20
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

AST_USING_NAMESPACE

void testScriptParser(StringView str)
{
    Expr* expr = aParseExpr(str);
    EXPECT_NE(expr, nullptr);
    std::cout << "Input expression: " << str << std::endl;
    if (expr) {
        std::string exprStr = aFormatExpr(expr);
        std::cout << "1st Parsed expression: " << exprStr << std::endl;
        delete expr;
        {
			Expr* expr2 = aParseExpr(exprStr);
			EXPECT_NE(expr2, nullptr);
            if (expr2) {
                std::string exprStr2 = aFormatExpr(expr2);
                std::cout << "2nd Parsed expression: " <<  exprStr2 << std::endl;
                EXPECT_EQ(exprStr, exprStr2);
				delete expr2;
            }
            else {
				std::cout << "Failed to re-parse expression." << std::endl;
            }
        }
    } else {
        std::cout << "Failed to parse expression." << std::endl;
    }
}

TEST(ScriptParser, ArithmeticExpression)
{
    // 基本算术运算
    testScriptParser("1 + 2 * 3");
    testScriptParser("(4 - 5) / 6");
    testScriptParser("7%8");
    testScriptParser("-(9 + 10)");
    
    // 浮点数支持
    testScriptParser("123.456");
    testScriptParser("0.123 + 4.56");
    testScriptParser("100 - 0.5");
    testScriptParser("3.14 * 2.0");
    testScriptParser("10 / 3.0");
    
    // 幂指数运算:
    testScriptParser("2^3");
    testScriptParser("2^3^6");
    testScriptParser("2^3 + 4");
    testScriptParser("2 * 3^4");
    testScriptParser("(2 + 3)^4");
    
    // 复杂算术表达式
    testScriptParser("1 + 2 * 3 - 4 / 5");
    testScriptParser("(1 + 2) * (3 - 4) / 5");
    testScriptParser("1 + (2 * 3 - 4) / 5");
}

TEST(ScriptParser, ComparisonExpression)
{
    // 比较运算符
    testScriptParser("1 == 2");
    testScriptParser("3 != 4");
    testScriptParser("5 < 6");
    testScriptParser("7 > 8");
    testScriptParser("9 <= 10");
    testScriptParser("11 >= 12");
    
    // 带比较的复杂表达式
    testScriptParser("1 + 2 == 3");
    testScriptParser("4 * 5 > 6 + 7");
    testScriptParser("(8 - 9) != 0");
}

TEST(ScriptParser, LogicalExpression)
{
    // 逻辑运算符
    testScriptParser("true && false");
    testScriptParser("true || false");
    testScriptParser("!true");
    testScriptParser("!false");
    
    // 复合逻辑表达式
    testScriptParser("true && (false || true)");
    testScriptParser("!true || false");
    testScriptParser("(1 == 2) && (3 != 4)");
}

TEST(ScriptParser, BitwiseExpression)
{
    // 按位运算符
    testScriptParser("1 & 2");
    testScriptParser("3 | 4");
    testScriptParser("~7");
    testScriptParser("8 << 2");
    testScriptParser("16 >> 1");
    
    // 复合按位表达式
    testScriptParser("(1 & 2) | (3 & 4)");
    testScriptParser("8 << 2 >> 1");
}

TEST(ScriptParser, LiteralExpression)
{
    // 数值字面量
    testScriptParser("42");
    testScriptParser("0");
    testScriptParser("-123");
    testScriptParser("3.14159");
    
    // 十六进制整数
    testScriptParser("0x0");
    testScriptParser("0x10");
    testScriptParser("0xFF");
    testScriptParser("0xEF");
    testScriptParser("0xEe");
    testScriptParser("0xabc");
    // testScriptParser("0x123456789ABCDEF"); // @fixme 超出int范围，需要处理这种情况
    
    // 二进制整数
    testScriptParser("0b0");
    testScriptParser("0b1");
    testScriptParser("0b1010");
    testScriptParser("0b1111");
    testScriptParser("0b10000000");
    
    // 八进制整数，参照Julia语法
    testScriptParser("0o0");
    testScriptParser("0o1");
    testScriptParser("0o7");
    testScriptParser("0o10");
    testScriptParser("0o100");
    testScriptParser("0o777");
    testScriptParser("0o123");
    // testScriptParser("0Oabc"); // 这应该会失败，因为八进制只能包含0-7
    
    // 字符串字面量
    testScriptParser("\"hello\"");
    testScriptParser("\"world\"");
    
    // 布尔字面量
    testScriptParser("true");
    testScriptParser("false");
    
    // null字面量
    testScriptParser("null");
}

TEST(ScriptParser, AssignmentExpression)
{
    // 赋值表达式
    testScriptParser("x = 10");
    testScriptParser("y = x + 5");
    testScriptParser("z = (a + b) * c");
}

TEST(ScriptParser, ConditionalExpression)
{
    // 条件表达式
    testScriptParser("true ? 1 : 2");
    testScriptParser("false ? 3 : 4");
    testScriptParser("(x == 10) ? \"yes\" : \"no\"");
    testScriptParser("a > b ? a : b");
}

TEST(ScriptParser, ComplexExpression)
{
    // 复杂表达式组合
    testScriptParser("x = 10 + y * 2");
    testScriptParser("(a > b ? a : b) + (c < d ? c : d)");
    testScriptParser("true && (x == 10) ? \"valid\" : \"invalid\"");
    testScriptParser("(1 + 2 * 3 - 4) / 5 && 6 < 7");
    testScriptParser("!(x == 10) || (y != 20) && true");
}


GTEST_MAIN()