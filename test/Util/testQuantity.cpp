///
/// @file      testQuantity.cpp
/// @brief     ~
/// @details   ~
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

#include "AstUtil/Quantity.hpp"
#include "AstUtil/IO.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE
using namespace _AST units;

// 测试基本构造函数和静态方法
TEST(Quantity, Basic)
{
    // 测试静态方法
    auto nan = Quantity::NaN();
    EXPECT_FALSE(nan.isValid());
    
    auto zero = Quantity::Zero();
    EXPECT_TRUE(zero.isValid());
    EXPECT_EQ(zero.value(), 0.0);
    
    auto one = Quantity::One();
    EXPECT_TRUE(one.isValid());
    EXPECT_EQ(one.value(), 1.0);
    
    auto scalar = Quantity::Scalar(5.0);
    EXPECT_TRUE(scalar.isValid());
    EXPECT_EQ(scalar.value(), 5.0);
    EXPECT_TRUE(scalar.unit() == Unit::None());
    
    // 测试构造函数
    Quantity q(10.0, Unit::Meter());
    EXPECT_EQ(q.value(), 10.0);
    EXPECT_TRUE(q.unit() == Unit::Meter());
    
    // 测试setter和getter
    q.setValue(20.0);
    EXPECT_EQ(q.value(), 20.0);
    
    q.setUnit(Unit::Kilogram());
    EXPECT_TRUE(q.unit() == Unit::Kilogram());
    
    // 测试toString
    auto str = q.toString();
    ast_printf("Quantity: %s\n", str.c_str());
}

// 测试相等性比较
TEST(Quantity, Comparison)
{
    auto q1 = 1000 * m;
    auto q2 = 1 * km;
    auto q3 = 2 * km;
    
    EXPECT_TRUE(q1 == q2);
    EXPECT_FALSE(q1 != q2);
    EXPECT_FALSE(q1 == q3);
    EXPECT_TRUE(q1 != q3);
    
    // 测试不同单位但相同量纲的比较
    auto q4 = 1 * kg;
    auto q5 = 1000 * g;
    EXPECT_TRUE(q4 == q5);
}

// 测试加法运算
TEST(Quantity, Add)
{
    // 相同单位相加
    auto q1 = 10 * m;
    auto q2 = 20 * m;
    auto q3 = q1 + q2;
    EXPECT_EQ(q3.value(), 30.0);
    EXPECT_TRUE(q3.unit() == Unit::Meter());
    
    // 不同单位但相同量纲相加
    auto q4 = 1 * km;
    auto q5 = 500 * m;
    auto q6 = q4 + q5;
    auto str6 = q6.toString();
    EXPECT_EQ(q6, 1500 * m);
    ast_printf("1km + 500m = %s\n", str6.c_str());
    
    // 测试加法赋值
    auto q7 = 10 * m;
    q7 += 20 * m;
    EXPECT_EQ(q7.value(), 30.0);
}

// 测试减法运算
TEST(Quantity, Sub)
{
    // 相同单位相减
    auto q1 = 30 * m;
    auto q2 = 10 * m;
    auto q3 = q1 - q2;
    EXPECT_EQ(q3.value(), 20.0);
    EXPECT_TRUE(q3.unit() == Unit::Meter());
    
    // 不同单位但相同量纲相减
    auto q4 = 1 * km;
    auto q5 = 500 * m;
    auto q6 = q4 - q5;
    auto str6 = q6.toString();
    EXPECT_EQ(q6, 500 * m);
    ast_printf("1km - 500m = %s\n", str6.c_str());
    
    // 测试减法赋值
    auto q7 = 30 * m;
    q7 -= 10 * m;
    EXPECT_EQ(q7.value(), 20.0);
}

// 测试乘法运算
TEST(Quantity, Mul)
{
    // 数量乘以标量
    auto q1 = 10 * m;
    auto q2 = q1 * 2.0;
    EXPECT_EQ(q2.value(), 20.0);
    EXPECT_TRUE(q2.unit() == Unit::Meter());
    
    // 标量乘以数量
    auto q3 = 3.0 * q1;
    EXPECT_EQ(q3.value(), 30.0);
    EXPECT_EQ(q3, 30 * m);
    EXPECT_TRUE(q3.unit() == Unit::Meter());
    
    // 数量乘以单位
    auto q4 = 10 * m;
    auto q5 = q4 * Unit::Second();
    auto str5 = q5.toString();
    EXPECT_EQ(q5, 10 * m * s);
    ast_printf("10m * s = %s\n", str5.c_str());
    
    // 数量乘以数量
    auto q6 = 10 * m;
    auto q7 = 2 * m;
    auto q8 = q6 * q7;
    auto str8 = q8.toString();
    EXPECT_EQ(q8, 20 * m2);
    ast_printf("10m * 2m = %s\n", str8.c_str());
    
    // 测试乘法赋值
    auto q9 = 10 * m;
    q9 *= 2.0;
    EXPECT_EQ(q9.value(), 20.0);
    
    auto q10 = 10 * m;
    q10 *= Unit::Second();
    auto str10 = q10.toString();
    EXPECT_EQ(q10, 10 * m * s);
    ast_printf("10m *= s = %s\n", str10.c_str());
}

// 测试除法运算
TEST(Quantity, Div)
{
    // 数量除以标量
    auto q1 = 20 * m;
    auto q2 = q1 / 2.0;
    EXPECT_EQ(q2.value(), 10.0);
    EXPECT_EQ(q2, 10.0 * m);
    EXPECT_TRUE(q2.unit() == Unit::Meter());
    
    // 数量除以单位
    auto q3 = 10 * m;
    auto q4 = q3 / Unit::Second();
    auto str4 = q4.toString();
    EXPECT_EQ(q4, 10.0 * m / s);
    ast_printf("10m / s = %s\n", str4.c_str());
    
    // 数量除以数量
    auto q5 = 20 * m;
    auto q6 = 10 * m;
    auto q7 = q5 / q6;
    auto str7 = q7.toString();
    EXPECT_EQ(q7, 2.0);
    ast_printf("20m / 10m = %s\n", str7.c_str());
    
    // 测试除法赋值
    auto q8 = 20 * m;
    q8 /= 2.0;
    EXPECT_EQ(q8, 10 * m);
    EXPECT_EQ(q8.value(), 10.0);
    
    auto q9 = 10 * m;
    q9 /= Unit::Second();
    auto str9 = q9.toString();
    EXPECT_EQ(q9, 10 * m / s);
    ast_printf("10m /= s = %s\n", str9.c_str());
}

// 测试单位转换
TEST(Quantity, UnitConversion)
{
    // 测试长度单位转换
    EXPECT_TRUE(1 * km == 1000 * m);
    EXPECT_TRUE(1 * m == 100 * cm);
    EXPECT_TRUE(1 * cm == 10 * mm);
    
    // 测试质量单位转换
    EXPECT_TRUE(1 * kg == 1000 * g);
    EXPECT_TRUE(1 * g == 1000 * mg);
    
    // 测试时间单位转换
    EXPECT_TRUE(1 * h == 60 * min);
    EXPECT_TRUE(1 * min == 60 * s);
    EXPECT_TRUE(1 * h == 3600 * s);
    
    // 测试面积单位转换
    auto area1 = 1 * km * 1 * km;
    auto area2 = 1000000 * m * 1 * m;
    EXPECT_TRUE(area1 == area2);
}

// 测试复杂运算
TEST(Quantity, ComplexOperations)
{
    // 测试速度计算
    auto distance = 100 * m;
    auto time = 10 * s;
    auto speed = distance / time;
    auto str_speed = speed.toString();
    ast_printf("Speed: %s\n", str_speed.c_str());
    
    // 测试加速度计算
    auto initial_velocity = 0 * m / s;
    auto final_velocity = 20 * m / s;
    auto acceleration = (final_velocity - initial_velocity) / time;
    auto str_acceleration = acceleration.toString();
    ast_printf("Acceleration: %s\n", str_acceleration.c_str());
    
    // 测试力的计算 (F = ma)
    auto mass = 5 * kg;
    auto force = mass * acceleration;
    auto str_force = force.toString();
    ast_printf("Force: %s\n", str_force.c_str());
}

// 测试数量值倒数运算
TEST(Quantity, Invert)
{
    // 测试标量的倒数
    {
        auto q = Quantity::Scalar(2.0);
        auto inverted = q.invert();
        EXPECT_EQ(inverted.value(), 0.5);
        EXPECT_TRUE(inverted.unit() == Unit::None());
    }

    // 测试基本单位数量值的倒数
    {
        auto q = 10.0 * Unit::Meter(); // 10 m
        auto inverted = q.invert(); // 0.1 m⁻¹
        EXPECT_EQ(inverted.value(), 0.1);
        EXPECT_EQ(inverted.dimension(), Dimension::Length().invert());
    }

    // 测试复合单位数量值的倒数
    {
        auto q = 5.0 * (Unit::Meter() / Unit::Second()); // 5 m/s
        auto inverted = q.invert(); // 0.2 s/m
        EXPECT_EQ(inverted.value(), 0.2);
        EXPECT_EQ(inverted.dimension(), (Dimension::Length() / Dimension::Time()).invert());
    }

    // 测试倒数的倒数是否等于原数量值
    {
        auto q = 3.0 * Unit::Kilogram(); // 3 kg
        auto inverted1 = q.invert();
        auto inverted2 = inverted1.invert();
        EXPECT_TRUE(inverted2 == q);
    }

    // 测试与其他运算的结合
    {
        auto speed = 100 * km / h; // 100 km/h
        auto time = 2 * h;
        auto distance = speed * time; // 200 km
        auto inverted_speed = speed.invert(); // h/km
        auto distance2 = time / inverted_speed; // 200 km
        EXPECT_TRUE(distance == distance2);
    }
}

GTEST_MAIN()