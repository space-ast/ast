///
/// @file      testPolynomial.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-11
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "AstUtil/Polynomial.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE


TEST(Polynomial, Parse)
{
    {
        // 测试基本多项式（带变量名）
        Polynomial poly1;
        err_t err1 = poly1.parse("2x^2 + 3x - 1", "x");
        EXPECT_EQ(err1, 0);
        EXPECT_EQ(poly1.eval(0), -1.0);
        EXPECT_EQ(poly1.eval(1), 4.0);
        EXPECT_EQ(poly1.eval(2), 13.0);
    }

    {
        // 测试基本多项式（不带变量名，默认使用x）
        Polynomial poly2;
        err_t err2 = poly2.parse("2x^2 + 3x - 1");
        EXPECT_EQ(err2, 0);
        EXPECT_EQ(poly2.eval(0), -1.0);
        EXPECT_EQ(poly2.eval(1), 4.0);
        EXPECT_EQ(poly2.eval(2), 13.0);
    }

    {
        // 测试无常数项的多项式（不带变量名）
        Polynomial poly3;
        err_t err3 = poly3.parse("3x^3 + 5x");
        EXPECT_EQ(err3, 0);
        EXPECT_EQ(poly3.eval(0), 0.0);
        EXPECT_EQ(poly3.eval(1), 8.0);
    }

    {
        // 测试只有常数项的多项式（不带变量名）
        Polynomial poly4;
        err_t err4 = poly4.parse("-7");
        EXPECT_EQ(err4, 0);
        EXPECT_EQ(poly4.eval(0), -7.0);
        EXPECT_EQ(poly4.eval(100), -7.0);
    }

    {
        // 测试无指数的项（不带变量名）
        Polynomial poly5;
        err_t err5 = poly5.parse("x + 1");
        EXPECT_EQ(err5, 0);
        EXPECT_EQ(poly5.eval(0), 1.0);
        EXPECT_EQ(poly5.eval(5), 6.0);
    }

    {
        // 测试带空格的多项式（不带变量名）
        Polynomial poly6;
        err_t err6 = poly6.parse("  4x^3   -   2x^2   +   3x   -   5  ");
        EXPECT_EQ(err6, 0);
        EXPECT_EQ(poly6.eval(1), 0.0);
    }
    {
        // 测试带空格的多项式（不带变量名）
        Polynomial poly;
        err_t err = poly.parse("  4 x ^ 3   -   2 x ^ 2   +   3 x   -   5  ");
        EXPECT_EQ(err, 0);
        EXPECT_EQ(poly.eval(1), 0.0);
        std::string expr = poly.toString("x");
        printf("expr: %s\n", expr.c_str());
    }
    {
        // 测试不同变量名（需要显式指定）
        Polynomial poly7;
        err_t err7 = poly7.parse("2y^2 + 3y + 1", "y");
        EXPECT_EQ(err7, 0);
        EXPECT_EQ(poly7.eval(0), 1.0);
        EXPECT_EQ(poly7.eval(1), 6.0);
        std::string expr = poly7.toString("y");
        printf("expr: %s\n", expr.c_str());
    }
    {
        // 测试动态变量名识别
        Polynomial poly8;
        err_t err8 = poly8.parse("2y^2 + 3y + 1");
        EXPECT_EQ(err8, 0);
        EXPECT_EQ(poly8.eval(0), 1.0);
        EXPECT_EQ(poly8.eval(1), 6.0);
    }
    {
        // 测试另一个动态变量名
        Polynomial poly9;
        err_t err9 = poly9.parse("5z^3 - 2z + 7");
        EXPECT_EQ(err9, 0);
        EXPECT_EQ(poly9.eval(0), 7.0);
        EXPECT_EQ(poly9.eval(1), 10.0);
        EXPECT_EQ(poly9.eval(2), 43.0);
    }
    {
        // 测试浮点数
        Polynomial poly;
        err_t err = poly.parse("2.5a^2 + 3a - 1.5");
        EXPECT_EQ(err, 0);
        EXPECT_EQ(poly.eval(0), -1.5);
        EXPECT_EQ(poly.eval(1), 4.0);
        EXPECT_EQ(poly.eval(2), 14.5);
    }
}


TEST(Polynomial, ParseFailed)
{
    {
        // 测试不匹配的变量名
        Polynomial poly;
        err_t err = poly.parse("5x + z");
        EXPECT_NE(err, 0);
    }
    {
        // 测试无效的指数
        Polynomial poly;
        err_t err = poly.parse("2.5a^2.5 + 3a - 1.5");
        EXPECT_NE(err, 0);
        std::string expr = poly.toString("a");
        printf("expr: %s\n", expr.c_str());
    }
}


GTEST_MAIN();

