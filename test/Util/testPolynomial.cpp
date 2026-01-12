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

TEST(Polynomial, TODO)
{
/*

# Extract from IERS conventions 2010 (equations 5.43 and 5.44)

# Mean Anomaly of the Moon
 F1 ≡ l = 134.96340251◦ + 1717915923.2178″t + 31.8792″t² + 0.051635″t³ − 0.00024470″t⁴

# Mean Anomaly of the Sun
 F2 ≡ l' = 357.52910918◦ + 129596581.0481″t − 0.5532″t² + 0.000136″t³ − 0.00001149″t⁴

# L − Ω
 F3 ≡ F = 93.27209062◦ + 1739527262.8478″t − 12.7512″t² − 0.001037″t³ + 0.00000417″t⁴

# Mean Elongation of the Moon from the Sun
 F4 ≡ D = 297.85019547◦ + 1602961601.2090″t − 6.3706″t² + 0.006593″t³ − 0.00003169″t⁴

# Mean Longitude of the Ascending Node of the Moon
 F5 ≡ Ω = 125.04455501◦ − 6962890.5431″t + 7.4722″t² + 0.007702″t³ − 0.00005939″t⁴

# Mean Mercury longitude
 F6 ≡ LMe = 4.402608842 + 2608.7903141574 × t

# Mean Venus longitude
 F7 ≡ LVe = 3.176146697 + 1021.3285546211 × t

# Mean Earth longitude
 F8 ≡ LE  = 1.753470314 +  628.3075849991 × t

# Mean Mars longitude
 F9 ≡ LMa = 6.203480913 +  334.0612426700 × t

# Mean Jupiter longitude
F10 ≡ LJ  = 0.599546497 +   52.9690962641 × t

# Mean Saturn longitude
F11 ≡ LSa = 0.874016757 +   21.3299104960 × t

# Mean Uranus longitude
F12 ≡ LU  = 5.481293872 +    7.4781598567 × t

# Mean Neptune longitude
F13 ≡ LNe = 5.311886287 +    3.8133035638 × t

# General accumulated precession
F14 ≡ pA  = 0.02438175 × t + 0.00000538691 × t²

*/

/*

# Extract from IERS conventions 2003 (equations 40 and 41)

# Mean Anomaly of the Moon
 F1 ≡ l = 134.96340251° + 1717915923.2178''t + 31.8792''t² + 0.051635''t³ - 0.00024470''t⁴

# Mean Anomaly of the Sun
 F2 ≡ l' = 357.52910918° + 129596581.0481"t - 0.5532"t² + 0.000136"t³ - 0.00001149"t⁴

# L - Ω
 F3 ≡ F = 93.27209062° + 1739527262.8478″t - 12.7512″t² - 0.001037″t³ + 0.00000417″t⁴

# Mean Elongation of the Moon from the Sun
 F4 ≡ D = 297.85019547° + 1602961601.2090″t - 6.3706″t² + 0.006593″t³ - 0.00003169″t⁴

# Mean Longitude of the Ascending Node of the Moon
 F5 ≡ Ω = 125.04455501° - 6962890.5431″t + 7.4722″t² + 0.007702″t³ - 0.00005939″t⁴

# Mean Mercury longitude
 F6 ≡ LMe = 4.402608842 + 2608.7903141574 × t

# Mean Venus longitude
 F7 ≡ LVe = 3.176146697 + 1021.3285546211 × t

# Mean Earth longitude
 F8 ≡ LE  = 1.753470314 +  628.3075849991 × t

# Mean Mars longitude
 F9 ≡ LMa = 6.203480913 +  334.0612426700 × t

# Mean Jupiter longitude
F10 ≡ LJ  = 0.599546497 +   52.9690962641 × t

# Mean Saturn longitude
F11 ≡ LSa = 0.874016757 +   21.3299104960 × t

# Mean Uranus longitude
F12 ≡ LU  = 5.481293872 +    7.4781598567 × t

# Mean Neptune longitude
F13 ≡ LNe = 5.311886287 +    3.8133035638 × t

# General accumulated precession
F14 ≡ pA  = 0.02438175 × t + 0.00000538691 × t²

*/
}


GTEST_MAIN();

