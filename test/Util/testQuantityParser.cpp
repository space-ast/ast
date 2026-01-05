///
/// @file      testQuantityParser.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-18
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

#include "AstTest/AstTestMacro.h"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/Unit.hpp"

AST_USING_NAMESPACE
using namespace _AST units;

TEST(QuantityParser, Basic)
{
    Quantity quantity;
    err_t err = aQuantityParse("123.456 m/s", quantity);
    EXPECT_EQ(quantity, 123.456 * m / s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(quantity.isValid());
}

// 测试基本数值解析（直接返回 Quantity 的函数）
TEST(QuantityParser, ParseToQuantity)
{
    // 整数
    Quantity q1 = aQuantityParse("123");

    EXPECT_EQ(q1, 123);
    EXPECT_TRUE(q1.isValid());
    EXPECT_NEAR(q1.value(), 123.0, 1e-9);
    
    // 小数
    Quantity q2 = aQuantityParse("123.456");
    EXPECT_EQ(q2, 123.456);
    EXPECT_TRUE(q2.isValid());
    EXPECT_NEAR(q2.value(), 123.456, 1e-9);
    
    // 带单位的整数
    Quantity q3 = aQuantityParse("456 m");
    EXPECT_EQ(q3, 456 * m);
    EXPECT_TRUE(q3.isValid());
    EXPECT_NEAR(q3.value(), 456.0, 1e-9);
    
    // 带单位的小数
    Quantity q4 = aQuantityParse("789.123 km");
    EXPECT_EQ(q4, 789.123 * km);
    EXPECT_TRUE(q4.isValid());
    EXPECT_NEAR(q4.value(), 789.123, 1e-9);
}

// 测试直接解析到 value 和 unit 的函数
TEST(QuantityParser, ParseToValueAndUnit)
{
    double value = 0.0;
    Unit unit;
    
    // 整数
    err_t err = aQuantityParse("123", value, unit);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(value, 123.0, 1e-9);
    EXPECT_TRUE(unit.dimension() == EDimension::eUnit);
    
    // 小数
    err = aQuantityParse("123.456", value, unit);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(value, 123.456, 1e-9);
    EXPECT_TRUE(unit.dimension() == EDimension::eUnit);
    
    // 带单位的整数
    err = aQuantityParse("456 m", value, unit);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(value, 456.0, 1e-9);
    EXPECT_EQ(unit, m);

    // 带单位的小数
    err = aQuantityParse("789.123 km", value, unit);
    EXPECT_EQ(err, eNoError);
    EXPECT_NEAR(value, 789.123, 1e-9);
    EXPECT_EQ(unit, km);
}

// 测试复合单位
TEST(QuantityParser, ComplexUnits)
{
    Quantity quantity;
    
    // 速度单位
    err_t err = aQuantityParse("10 m/s", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 10 * m /s);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 10.0, 1e-9);
    
    // 加速度单位
    err = aQuantityParse("9.8 m/s^2", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 9.8 * m / s / s);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 9.8, 1e-9);
    
    // 面积单位
    err = aQuantityParse("100 m^2", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 100 * m*m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 100.0, 1e-9);
    
    // 体积单位
    err = aQuantityParse("1000 m^3", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 1000 * m * m * m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 1000.0, 1e-9);
}

// 测试带括号的单位
TEST(QuantityParser, BracketedUnits)
{
    Quantity quantity;
    
    // 带括号的基本单位
    err_t err = aQuantityParse("10 [m]", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 10 * m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 10.0, 1e-9);
    
    // 带括号的复合单位
    err = aQuantityParse("9.8 [m/s^2]", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 9.8 * m/s/s);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 9.8, 1e-9);
    
    // 带括号的复杂单位
    err = aQuantityParse("50 [kg*m/s^2]", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 50 * N);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 50.0, 1e-9);
}

// 测试错误情况
TEST(QuantityParser, ErrorCases)
{
    Quantity quantity;
    
    // 无效的数值格式
    err_t err = aQuantityParse("abc m", quantity);
    EXPECT_NE(err, eNoError);
    EXPECT_FALSE(quantity.isValid());
    
    // 无效的单位格式
    err = aQuantityParse("123 invalid_unit", quantity);
    EXPECT_NE(err, eNoError);
    
    // 不匹配的括号
    err = aQuantityParse("123 [m", quantity);
    EXPECT_NE(err, eNoError);
    
    // 数值缺失
    err = aQuantityParse(" m/s", quantity);
    EXPECT_NE(err, eNoError);
    EXPECT_FALSE(quantity.isValid());
}

// 测试不带空格的格式
TEST(QuantityParser, NoWhitespaceHandling)
{
    Quantity quantity;
    
    // 数值和单位之间没有空格
    err_t err = aQuantityParse("123m/s", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m / s);
    EXPECT_TRUE(quantity.isValid());

    err = aQuantityParse("123m/s^2", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m / s / s);

    // 带括号的单位，括号内外没有空格
    err = aQuantityParse("123[m/s]", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m / s);
    EXPECT_TRUE(quantity.isValid());

    err = aQuantityParse("123[m/s^2]", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m / s / s);
}

// 测试带空格的格式
TEST(QuantityParser, WhitespaceHandling)
{
    Quantity quantity;
    
    // 数值和单位之间有多个空格
    err_t err = aQuantityParse("123   m/s", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m/s);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 123.0, 1e-9);
    
    // 带括号的单位，括号内外有空格
    err = aQuantityParse("123  [  m/s  ]  ", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 123 * m / s);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 123.0, 1e-9);
}

// 测试科学计数法
TEST(QuantityParser, ScientificNotation)
{
    Quantity quantity;
    
    // 正指数
    err_t err = aQuantityParse("1.23e3 m", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 1.23e3 * m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 1230.0, 1e-9);
    
    // 负指数
    err = aQuantityParse("1.23e-3 m", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 1.23e-3 * m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 0.00123, 1e-12);
    
    // 大写 E
    err = aQuantityParse("1.23E3 m", quantity);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(quantity, 1.23E3 * m);
    EXPECT_TRUE(quantity.isValid());
    EXPECT_NEAR(quantity.value(), 1230.0, 1e-9);
}

GTEST_MAIN()
