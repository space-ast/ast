///
/// @file      testUnit.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-14
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
#include "AstUtil/Constants.h"
#include "AstUtil/Unit.hpp"

AST_USING_NAMESPACE

// 测试基本单位创建
TEST(Unit, BasicCreation)
{
    // 测试None单位
    {   
        auto unit = Unit::None();
        EXPECT_EQ(unit.name(), "");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Unit());
    }
    
    // 测试One单位
    {
        auto unit = Unit::One();
        EXPECT_EQ(unit.name(), aText("×1"));
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Unit());
    }
    
    // 测试Scale单位
    {
        auto unit = Unit::Scale(2.5);
        EXPECT_EQ(unit.getScale(), 2.5);
        EXPECT_EQ(unit.dimension(), Dimension::Unit());
    }
    
    // 测试Percent单位
    {
        auto unit = Unit::Percent();
        EXPECT_EQ(unit.name(), "%");
        EXPECT_EQ(unit.getScale(), 0.01);
        EXPECT_EQ(unit.dimension(), Dimension::Unit());
    }
}

// 测试预定义单位
TEST(Unit, PredefinedUnits)
{
    // 时间单位
    {
        auto unit = Unit::Second();
        EXPECT_EQ(unit.name(), "s");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Time());
        
        unit = Unit::Minute();
        EXPECT_EQ(unit.name(), "min");
        EXPECT_EQ(unit.getScale(), 60.0);
        EXPECT_EQ(unit.dimension(), Dimension::Time());
        
        unit = Unit::Hour();
        EXPECT_EQ(unit.name(), "h");
        EXPECT_EQ(unit.getScale(), 3600.0);
        EXPECT_EQ(unit.dimension(), Dimension::Time());
        
        unit = Unit::Day();
        EXPECT_EQ(unit.name(), "day");
        EXPECT_EQ(unit.getScale(), 86400.0);
        EXPECT_EQ(unit.dimension(), Dimension::Time());
    }
    
    // 长度单位
    {
        auto unit = Unit::Meter();
        EXPECT_EQ(unit.name(), "m");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Length());
        
        unit = Unit::Kilometer();
        EXPECT_EQ(unit.name(), "km");
        EXPECT_EQ(unit.getScale(), 1000.0);
        EXPECT_EQ(unit.dimension(), Dimension::Length());
    }
    
    // 质量单位
    {
        auto unit = Unit::Kilogram();
        EXPECT_EQ(unit.name(), "kg");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Mass());
        
        unit = Unit::Gram();
        EXPECT_EQ(unit.name(), "g");
        EXPECT_EQ(unit.getScale(), 0.001);
        EXPECT_EQ(unit.dimension(), Dimension::Mass());
    }
    
    // 角度单位
    {
        auto unit = Unit::Radian();
        EXPECT_EQ(unit.name(), "rad");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Angle());
        
        unit = Unit::Degree();
        EXPECT_EQ(unit.name(), "°");
        EXPECT_NEAR(unit.getScale(), 0.017453292519943295, 1e-15);
        EXPECT_EQ(unit.dimension(), Dimension::Angle());
    }
    
    // 力单位
    {
        auto unit = Unit::Newton();
        EXPECT_EQ(unit.name(), "N");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Force());
    }
    
    // 其他基本单位
    {
        auto unit = Unit::Ampere();
        EXPECT_EQ(unit.name(), "A");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Current());
        
        unit = Unit::Kelvin();
        EXPECT_EQ(unit.name(), "K");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Temperature());
        
        unit = Unit::Mole();
        EXPECT_EQ(unit.name(), "mol");
        EXPECT_EQ(unit.getScale(), 1.0);
        EXPECT_EQ(unit.dimension(), Dimension::Amount());
    }
}

// 测试单位转换
TEST(Unit, Conversion)
{
    // 测试时间单位转换
    {
        auto s = Unit::Second();
        auto min = Unit::Minute();
        auto h = Unit::Hour();
        
        // 1分钟 = 60秒
        EXPECT_EQ(s.toInternal(60), min.toInternal(1));
        EXPECT_EQ(s.fromInternal(1), min.fromInternal(60));
        
        // 1小时 = 3600秒
        EXPECT_EQ(s.toInternal(3600), h.toInternal(1));
        EXPECT_EQ(s.fromInternal(1), h.fromInternal(3600));
        
        // 1小时 = 60分钟
        EXPECT_EQ(min.toInternal(60), h.toInternal(1));
        EXPECT_EQ(min.fromInternal(1), h.fromInternal(60));
    }
    
    // 测试长度单位转换
    {
        auto m = Unit::Meter();
        auto km = Unit::Kilometer();
        
        // 1千米 = 1000米
        EXPECT_EQ(m.toInternal(1000), km.toInternal(1));
        EXPECT_EQ(m.fromInternal(1), km.fromInternal(1000));
    }
    
    // 测试质量单位转换
    {
        auto g = Unit::Gram();
        auto kg = Unit::Kilogram();
        
        // 1千克 = 1000克
        EXPECT_EQ(g.toInternal(1000), kg.toInternal(1));
        EXPECT_EQ(g.fromInternal(1), kg.fromInternal(1000));
    }
    
    // 测试角度单位转换
    {
        auto rad = Unit::Radian();
        auto deg = Unit::Degree();
        
        // 180度 = π弧度
        EXPECT_NEAR(deg.toInternal(180), rad.toInternal(kPI), 1e-15);
        EXPECT_NEAR(deg.fromInternal(kPI), rad.fromInternal(180), 1e-15);
    }
}

// 测试单位乘法
TEST(Unit, Multiply)
{
    // 基本乘法
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = kg * m;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() * Dimension::Length());
    }
    
    // 多次乘法
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto unit = kg * m * s;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() * Dimension::Length() * Dimension::Time());
    }
    
    // 乘法赋值运算符
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = kg;
        unit *= m;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() * Dimension::Length());
    }
    
    // 使用aUnitMultiply函数
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = aUnitMultiply(kg, m);
        EXPECT_EQ(unit.dimension(), Dimension::Mass() * Dimension::Length());
        
        // 带名称的乘法
        auto namedUnit = aUnitMultiply(kg, m, "kg·m");
        EXPECT_EQ(namedUnit.dimension(), Dimension::Mass() * Dimension::Length());
    }
}

// 测试单位除法
TEST(Unit, Divide)
{
    // 基本除法
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = kg / m;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() / Dimension::Length());
    }
    
    // 链式除法
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto unit = kg / m / s;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() / (Dimension::Length() * Dimension::Time()));
    }
    
    // 除法赋值运算符
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = kg;
        unit /= m;
        EXPECT_EQ(unit.dimension(), Dimension::Mass() / Dimension::Length());
    }
    
    // 使用aUnitDivide函数
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = aUnitDivide(kg, m);
        EXPECT_EQ(unit.dimension(), Dimension::Mass() / Dimension::Length());
        
        // 带名称的除法
        auto namedUnit = aUnitDivide(kg, m, "kg/m");
        EXPECT_EQ(namedUnit.dimension(), Dimension::Mass() / Dimension::Length());
    }
    
    // 测试牛顿单位的组合
    {
        auto N = Unit::Newton();
        auto expectedN = Unit::Kilogram() * Unit::Meter() / (Unit::Second() * Unit::Second());
        EXPECT_EQ(N.dimension(), expectedN.dimension());
    }
}

// 测试单位幂运算
TEST(Unit, Power)
{
    // 使用pow方法
    {
        auto m = Unit::Meter();
        auto m2 = m.pow(2);
        EXPECT_EQ(m2.dimension(), Dimension::Length().pow(2));
        
        auto m3 = m.pow(3);
        EXPECT_EQ(m3.dimension(), Dimension::Length().pow(3));
        
        // 带名称的幂运算
        auto m2Named = m.pow("m²", 2);
        EXPECT_EQ(m2Named.dimension(), Dimension::Length().pow(2));
    }
    
    // 使用aUnitPower函数
    {
        auto m = Unit::Meter();
        auto m2 = aUnitPower(m, 2);
        EXPECT_EQ(m2.dimension(), Dimension::Length().pow(2));
        
        // 带名称的幂运算
        auto m2Named = aUnitPower(m, 2, "m²");
        EXPECT_EQ(m2Named.dimension(), Dimension::Length().pow(2));
    }
    
    // 测试负指数
    {
        auto m = Unit::Meter();
        auto m_1 = m.pow(-1);
        EXPECT_EQ(m_1.dimension(), Dimension::Length().pow(-1));
    }
}

// 测试单位比较
TEST(Unit, Comparison)
{
    // 相同单位应该相等
    {
        auto kg1 = Unit::Kilogram();
        auto kg2 = Unit::Kilogram();
        EXPECT_EQ(kg1, kg2);
    }
    
    // 不同单位应该不相等
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        EXPECT_NE(kg, m);
    }
    
    // 相同量纲但不同缩放因子的单位应该不相等
    {
        auto g = Unit::Gram();
        auto kg = Unit::Kilogram();
        EXPECT_NE(g, kg);
    }
}

// 测试复杂单位组合
TEST(Unit, ComplexCombinations)
{
    // 测试速度单位 (m/s)
    {
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto mps = m / s;
        EXPECT_EQ(mps.dimension(), Dimension::Speed());
    }
    
    // 测试加速度单位 (m/s²)
    {
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto mps2 = m / s / s;
        EXPECT_EQ(mps2.dimension(), Dimension::Acceleration());
    }
    
    // 测试能量单位 (J = N·m = kg·m²/s²)
    {
        auto N = Unit::Newton();
        auto m = Unit::Meter();
        auto J = N * m;
        
        auto kg = Unit::Kilogram();
        auto m2 = m.pow(2);
        auto s2 = Unit::Second().pow(2);
        auto expectedJ = kg * m2 / s2;
        
        EXPECT_EQ(J.dimension(), expectedJ.dimension());
        EXPECT_EQ(J.dimension(), Dimension::Energy());
    }
    
    // 测试功率单位 (W = J/s = kg·m²/s³)
    {
        auto J = Unit::Newton() * Unit::Meter();
        auto s = Unit::Second();
        auto W = J / s;
        
        EXPECT_EQ(W.dimension(), Dimension::Power());
    }
}



TEST(Unit, multiply)
{
    auto kg = Unit::Kilogram();
    auto m = Unit::Meter();
    auto unit = kg * m;
    nothing();
    auto h = Unit::Hour();

    unit = unit * h;
    
    unit = unit * Unit::Minute();

    nothing();
}

TEST(Unit, divide)
{
    {
        auto kg = Unit::Kilogram();
        auto m = Unit::Meter();
        auto unit = kg * m;
        unit = unit / m;
        unit = unit / m;
        nothing();
    }
    {
        auto g = Unit::Gram();
        auto m = Unit::Meter();
        auto unit = g * m;
        unit = unit / m;
        unit = unit / m;
        nothing();
    }
    {
        auto kg = Unit::Kilogram();
        auto km = Unit::Kilometer();
        auto unit = kg * km;
        unit = unit / km;
        unit = unit / km;
        nothing();
    }
    {
        auto N = Unit::Newton();
        auto N2 = Unit::Kilogram() * Unit::Meter() / Unit::Second() / Unit::Second();
        auto unit = N / N2;
        nothing();
    }
    {
        auto N = Unit::Newton();
        auto N2 = Unit::Gram() * Unit::Meter() / Unit::Hour() / Unit::Second();
        auto unit = N / N2;
        nothing();
    }
    nothing();
}


TEST(Unit, Percent)
{
    auto unit = Unit::Percent();
    nothing();
}

GTEST_MAIN()