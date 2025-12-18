///
/// @file      testUnitParser.cpp
/// @brief     UnitParser 测试文件
/// @details   测试单位解析功能的正确性
/// @author    jinke18
/// @date      2025-12-15
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
#include "AstUtil/UnitParser.hpp"
#include "AstUtil/Unit.hpp"

AST_USING_NAMESPACE

using namespace _AST units;

// 测试基本单位解析
TEST(UnitParserTest, BasicUnitParse) {
    Unit unit;
    err_t err;
    
    // 测试基本长度单位
    err = aUnitParse("m", unit);
    EXPECT_EQ(unit, m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试基本时间单位
    err = aUnitParse("s", unit);
    EXPECT_EQ(unit, s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试基本质量单位
    err = aUnitParse("kg", unit);
    EXPECT_EQ(unit, kg);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试其他基本单位
    err = aUnitParse("N", unit); // Newton
    EXPECT_EQ(unit, N);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    err = aUnitParse("rad", unit); // Radian
    EXPECT_EQ(unit, rad);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试复合单位解析（乘法）
TEST(UnitParserTest, CompoundUnitMultiply) {
    Unit unit;
    err_t err;
    
    // 测试使用 * 分隔的复合单位
    err = aUnitParse("m*s", unit);
    EXPECT_EQ(unit, m*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试使用 · 分隔的复合单位
    err = aUnitParse(aText("m·s"), unit);
    EXPECT_EQ(unit, m*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试多个单位相乘
    err = aUnitParse(aText("kg·m·s"), unit);
    EXPECT_EQ(unit, kg*m*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试复合单位解析（除法）
TEST(UnitParserTest, CompoundUnitDivide) {
    Unit unit;
    err_t err;
    
    // 测试简单除法
    err = aUnitParse("m/s", unit);
    EXPECT_EQ(unit, m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试多个除法
    err = aUnitParse(aText("m/s/s"), unit);
    EXPECT_EQ(unit, m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试乘除混合
    err = aUnitParse(aText("kg·m/s"), unit);
    EXPECT_EQ(unit, kg*m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    err = aUnitParse(aText("kg·m/s²"), unit);
    EXPECT_EQ(unit, kg*m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试带指数的单位解析
TEST(UnitParserTest, UnitWithExponent) {
    Unit unit;
    err_t err;
    
    // 测试正指数
    err = aUnitParse("m^2", unit);
    EXPECT_EQ(unit, m*m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    err = aUnitParse("s^3", unit);
    EXPECT_EQ(unit, s*s*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试负指数
    err = aUnitParse("m^-1", unit);
    EXPECT_EQ(unit, m.pow(-1));
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试复合单位带指数
    err = aUnitParse(aText("kg·m^2/s^2"), unit);
    EXPECT_EQ(unit, kg*m*m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试Newton单位解析（kg·m/s²）
    err = aUnitParse("N", unit);
    EXPECT_EQ(unit, kg*m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试单返回值版本的单位解析函数
TEST(UnitParserTest, UnitParseSingleReturn) {
    Unit unit;
    
    // 测试基本单位
    unit = aUnitParse("m");
    EXPECT_EQ(unit, m);
    EXPECT_TRUE(unit.isValid());
    
    // 测试复合单位
    unit = aUnitParse("m/s");
    EXPECT_EQ(unit, m/s);
    EXPECT_TRUE(unit.isValid());
    
    // 测试带指数的单位
    unit = aUnitParse("m^2");
    EXPECT_EQ(unit, m * m);
    EXPECT_TRUE(unit.isValid());
    
    // 测试错误情况
    unit = aUnitParse("unknown_unit");
    EXPECT_FALSE(unit.isValid());
}

// 测试括号表达式处理
TEST(UnitParserTest, ParenthesesHandling) {
    Unit unit;
    err_t err;
    
    // 测试简单括号
    err = aUnitParse("(m/s)", unit);
    EXPECT_EQ(unit, m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试嵌套括号
    err = aUnitParse("((m/s)^2)", unit);
    EXPECT_EQ(unit, (m/s).pow(2));
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试括号与乘除混合
    err = aUnitParse(aText("(kg·m)/s"), unit);
    EXPECT_EQ(unit, kg*m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试复合括号表达式
    err = aUnitParse(aText("(kg·(m/s^2))"), unit);
    EXPECT_EQ(unit, kg*m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试复合括号表达式
    err = aUnitParse(aText("(kg/(m/s^2))"), unit);
    EXPECT_EQ(unit, kg/(m/s/s));
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试错误情况：缺少右括号
    err = aUnitParse("(m/s", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试错误情况：缺少左括号
    err = aUnitParse("m/s)", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
}

// 测试空格处理
TEST(UnitParserTest, WhitespaceHandling) {
    Unit unit;
    err_t err;
    
    // 测试前导空格
    err = aUnitParse("  m", unit);
    EXPECT_EQ(unit, m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试尾随空格
    err = aUnitParse("m  ", unit);
    EXPECT_EQ(unit, m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试中间空格（隐式乘法）
    err = aUnitParse("m s", unit);
    EXPECT_EQ(unit, m*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试多个空格
    err = aUnitParse("kg   m   s", unit);
    EXPECT_EQ(unit, kg*m*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试空格与运算符混合
    err = aUnitParse(aText("kg · m / s"), unit);
    EXPECT_EQ(unit, kg*m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试空格与指数混合
    err = aUnitParse("m ^ 2", unit);
    EXPECT_EQ(unit, m*m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试空格与括号混合
    err = aUnitParse("( m / s )", unit);
    EXPECT_EQ(unit, m/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试上标符号处理
TEST(UnitParserTest, SuperscriptHandling) {
    Unit unit;
    err_t err;
    
    // 测试简单上标
    err = aUnitParse(aText("m²"), unit);
    EXPECT_EQ(unit, m*m);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    err = aUnitParse(aText("s³"), unit);
    EXPECT_EQ(unit, s*s*s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试上标与其他运算符混合
    err = aUnitParse(aText("kg·m²/s²"), unit);
    EXPECT_EQ(unit, kg*m*m/s/s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试负数上标
    err = aUnitParse(aText("kg m²s⁻²"), unit);
    EXPECT_EQ(unit, kg * m * m / s / s);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

}

// 测试复杂嵌套表达式
TEST(UnitParserTest, ComplexNestedExpressions) {
    Unit unit;
    err_t err;
    
    // 测试复杂混合表达式
    err = aUnitParse(aText("(kg·m^2)/(s^2·A)"), unit);
    EXPECT_EQ(unit, kg*m*m/(s*s*A));
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试多层嵌套
    err = aUnitParse(aText("((kg·m)/s^2)^3"), unit);
    EXPECT_EQ(unit, (kg*m/s/s).pow(3));
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
    
    // 测试右结合指数
    err = aUnitParse("m^2^3", unit);
    EXPECT_EQ(unit, m.pow(8)); // m^(2^3) = m^8
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试多层指数
    err = aUnitParse("m^2^3^2", unit);
    EXPECT_EQ(unit, m.pow(512)); // m^(2^(3^2)) = m^512
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试多层指数带括号
    err = aUnitParse("(m^2)^3^2", unit);
    EXPECT_EQ(unit, m.pow(18)); // (m^2)^(3^2) = m^18
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试多层指数带括号
    err = aUnitParse("m^(2^3)^2", unit);
    EXPECT_EQ(unit, m.pow(64)); // m^(2^3)^2) = m^64
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());

    // 测试多层指数带括号
    err = aUnitParse("m^(64)", unit);
    EXPECT_EQ(unit, m.pow(64)); 
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(unit.isValid());
}

// 测试错误情况
TEST(UnitParserTest, ErrorCases) {
    Unit unit;
    err_t err;
    
    // 测试空字符串
    err = aUnitParse("", unit);
    EXPECT_TRUE(err);
    EXPECT_FALSE(unit.isValid());
    
    // 测试未知单位
    err = aUnitParse("unknown_unit", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试语法错误的单位字符串（连续的分隔符）
    err = aUnitParse("m**s", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试语法错误的单位字符串（开头的分隔符）
    err = aUnitParse("*m/s", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试语法错误的单位字符串（结尾的分隔符）
    err = aUnitParse("m/s*", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试无效字符
    err = aUnitParse("m@/s", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试不完整的指数
    err = aUnitParse("m^", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试无效的指数
    err = aUnitParse("m^abc", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试不匹配的括号
    err = aUnitParse("((m/s)", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
    
    // 测试多余的括号
    err = aUnitParse("(m/s))", unit);
    EXPECT_EQ(err, eErrorParse);
    EXPECT_FALSE(unit.isValid());
}

GTEST_MAIN()