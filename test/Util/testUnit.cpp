///
/// @file      testUnit.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
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
        EXPECT_EQ(unit.name(), aText("°"));
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

// 测试高级四则运算，覆盖非国际单位制、非1幂次和常数单位
TEST(Unit, AdvancedArithmetic)
{
    // 1. 非国际单位制的四则运算测试
    {
        // 测试不同时间单位的四则运算
        auto min = Unit::Minute();  // 非国际单位制（国际单位制是秒）
        auto h = Unit::Hour();     // 非国际单位制（国际单位制是秒）
        auto day = Unit::Day();    // 非国际单位制（国际单位制是秒）
        
        // 乘法
        auto min_h = min * h;
        EXPECT_EQ(min_h.dimension(), Dimension::Time() * Dimension::Time());
        EXPECT_NEAR(min_h.getScale(), 60.0 * 3600.0, 1e-15);
        
        // 除法
        auto day_h = day / h;
        EXPECT_EQ(day_h.dimension(), Dimension::Unit());  // 无量纲
        EXPECT_NEAR(day_h.getScale(), 86400.0 / 3600.0, 1e-15);
        
        // 混合运算
        auto min2 = min.pow(2);
        auto complex = min2 / h;
        EXPECT_EQ(complex.dimension(), Dimension::Time());
        EXPECT_NEAR(complex.getScale(), (60.0 * 60.0) / 3600.0, 1e-15);
    }
    
    // 2. 非1幂次的四则运算测试
    {
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto kg = Unit::Kilogram();
        
        // 高次幂运算
        auto m3 = m.pow(3);  // 立方米
        auto s2 = s.pow(2);  // 平方秒
        auto kg_2 = kg.pow(-2);  // 千克的负二次方
        
        EXPECT_EQ(m3.dimension(), Dimension::Volume());
        EXPECT_EQ(s2.dimension(), Dimension::Time().pow(2));
        EXPECT_EQ(kg_2.dimension(), Dimension::Mass().pow(-2));
        
        // 混合高次幂运算
        auto m3_s2 = m3 / s2;
        EXPECT_EQ(m3_s2.dimension(), Dimension::Volume() / Dimension::Time().pow(2));
        
        // 幂次与乘法混合
        auto complex = m3 * s2 * kg_2;
        EXPECT_EQ(complex.dimension(), Dimension::Volume() * Dimension::Time().pow(2) / Dimension::Mass().pow(2));
        
        // 幂次与除法混合
        auto complex2 = m3 / (s2 * kg_2);
        EXPECT_EQ(complex2.dimension(), Dimension::Volume() * Dimension::Mass().pow(2) / Dimension::Time().pow(2));
    }
    
    // 3. 常数单位的四则运算测试
    {
        // 测试百分比单位（常数单位）
        auto percent = Unit::Percent();  // 0.01的缩放因子
        
        // 百分比与其他单位的乘法
        auto m_percent = Unit::Meter() * percent;
        EXPECT_EQ(m_percent.dimension(), Dimension::Length());
        EXPECT_NEAR(m_percent.getScale(), 1.0 * 0.01, 1e-15);
        
        // 百分比与其他单位的除法
        auto s_percent = Unit::Second() / percent;
        EXPECT_EQ(s_percent.dimension(), Dimension::Time());
        EXPECT_NEAR(s_percent.getScale(), 1.0 / 0.01, 1e-15);
        
        // 测试Scale单位（常数单位）
        auto scale2 = Unit::Scale(2.0);  // 2.0的缩放因子
        auto scale3 = Unit::Scale(3.0);  // 3.0的缩放因子
        
        // Scale单位之间的运算
        auto scale6 = scale2 * scale3;
        EXPECT_EQ(scale6.dimension(), Dimension::Unit());
        EXPECT_NEAR(scale6.getScale(), 2.0 * 3.0, 1e-15);
        
        auto scale_23 = scale2 / scale3;
        EXPECT_EQ(scale_23.dimension(), Dimension::Unit());
        EXPECT_NEAR(scale_23.getScale(), 2.0 / 3.0, 1e-15);
        
        // Scale单位与其他单位的混合运算
        auto kg_scale2 = Unit::Kilogram() * scale2;
        EXPECT_EQ(kg_scale2.dimension(), Dimension::Mass());
        EXPECT_NEAR(kg_scale2.getScale(), 1.0 * 2.0, 1e-15);
        
        auto m_scale3 = Unit::Meter() / scale3;
        EXPECT_EQ(m_scale3.dimension(), Dimension::Length());
        EXPECT_NEAR(m_scale3.getScale(), 1.0 / 3.0, 1e-15);
    }
    
    // 4. 综合测试：非国际单位制 + 非1幂次 + 常数单位
    {
        auto km = Unit::Kilometer();  // 非国际单位制（国际单位制是米）
        auto h = Unit::Hour();        // 非国际单位制（国际单位制是秒）
        auto percent = Unit::Percent();  // 常数单位
        
        // 速度单位：km/h
        auto kmh = km / h;
        EXPECT_EQ(kmh.dimension(), Dimension::Speed());
        EXPECT_NEAR(kmh.getScale(), 1000.0 / 3600.0, 1e-15);
        
        // 加速度单位：km/h²
        auto kmh2 = km / h / h;
        EXPECT_EQ(kmh2.dimension(), Dimension::Acceleration());
        EXPECT_NEAR(kmh2.getScale(), 1000.0 / (3600.0 * 3600.0), 1e-15);
        
        // 带百分比的复杂运算：(km/h²) * %
        auto complex = kmh2 * percent;
        EXPECT_EQ(complex.dimension(), Dimension::Acceleration());
        EXPECT_NEAR(complex.getScale(), (1000.0 / (3600.0 * 3600.0)) * 0.01, 1e-15);
        
        // 带幂次的复杂运算：(km/h²)² * %
        auto complex2 = kmh2.pow(2) * percent;
        EXPECT_EQ(complex2.dimension(), Dimension::Acceleration().pow(2));
        EXPECT_NEAR(complex2.getScale(), std::pow(1000.0 / (3600.0 * 3600.0), 2) * 0.01, 1e-15);
    }
}

// 测试单位的合并和约简，覆盖非国际单位制、非1幂次和常数单位
TEST(Unit, ReductionAndSimplification)
{
    // 1. 非国际单位制的合并约简
    {
        // 测试千米(km)和米(m)的合并约简
        auto km = Unit::Kilometer();
        auto m = Unit::Meter();
        
        // km * m 应该合并为 km·m，并且保持正确的缩放因子
        auto km_m = km * m;
        EXPECT_EQ(km_m.dimension(), Dimension::Length().pow(2));
        EXPECT_NEAR(km_m.getScale(), 1000.0 * 1.0, 1e-15);
        
        // km / m 应该合并为无量纲，并且缩放因子为1000
        auto km_div_m = km / m;
        EXPECT_EQ(km_div_m.dimension(), Dimension::Unit());
        EXPECT_NEAR(km_div_m.getScale(), 1000.0 / 1.0, 1e-15);
        
        // 小时(h)和分钟(min)的合并约简
        auto h = Unit::Hour();
        auto min = Unit::Minute();
        
        // h / min 应该合并为无量纲，并且缩放因子为60
        auto h_div_min = h / min;
        EXPECT_EQ(h_div_min.dimension(), Dimension::Unit());
        EXPECT_NEAR(h_div_min.getScale(), 3600.0 / 60.0, 1e-15);
        
        // 克(g)和千克(kg)的合并约简
        auto g = Unit::Gram();
        auto kg = Unit::Kilogram();
        
        // g / kg 应该合并为无量纲，并且缩放因子为0.001
        auto g_div_kg = g / kg;
        EXPECT_EQ(g_div_kg.dimension(), Dimension::Unit());
        EXPECT_NEAR(g_div_kg.getScale(), 0.001 / 1.0, 1e-15);
    }
    
    // 2. 非1幂次的合并约简
    {
        // 测试相同单位不同幂次的合并约简
        auto m = Unit::Meter();
        
        // m^2 * m^3 应该合并为 m^5
        auto m2 = m.pow(2);
        {
            auto m3 = m.pow(3);
            auto m5 = m2 * m3;
            EXPECT_EQ(m5.dimension(), Dimension::Length().pow(5));
            EXPECT_NEAR(m5.getScale(), 1.0, 1e-15);
        }
        {
        // m^4 / m^2 应该合并为 m^2
        auto m4 = m.pow(4);
        auto m2_again = m4 / m2;
        EXPECT_EQ(m2_again.dimension(), Dimension::Length().pow(2));
        EXPECT_NEAR(m2_again.getScale(), 1.0, 1e-15);
        }

        // 测试不同单位不同幂次的合并约简
        auto kg = Unit::Kilogram();
        auto s = Unit::Second();
        
        // kg^3 * m^2 / (kg^2 * m^4) 应该合并为 kg / m^2
        {auto kg3 = kg.pow(3);
        auto m2 = m.pow(2);
        auto kg2 = kg.pow(2);
        auto m4 = m.pow(4);
        
        auto numerator = kg3 * m2;
        auto denominator = kg2 * m4;
        auto result = numerator / denominator;
        
        EXPECT_EQ(result.dimension(), Dimension::Mass() / Dimension::Length().pow(2));
        EXPECT_NEAR(result.getScale(), 1.0, 1e-15);
        }
        
        // 测试带负数幂次的合并约简
        auto m_1 = m.pow(-1);
        auto m_2 = m.pow(-2);
        
        // m^3 * m^-1 应该合并为 m^2
        auto m2_from_neg = m.pow(3) * m_1;
        EXPECT_EQ(m2_from_neg.dimension(), Dimension::Length().pow(2));
        EXPECT_NEAR(m2_from_neg.getScale(), 1.0, 1e-15);
        
        // m^-1 * m^-2 应该合并为 m^-3
        auto m_3 = m_1 * m_2;
        EXPECT_EQ(m_3.dimension(), Dimension::Length().pow(-3));
        EXPECT_NEAR(m_3.getScale(), 1.0, 1e-15);
    }
    
    // 3. 常数单位的合并约简
    {
        // 测试百分比单位的合并约简
        auto percent = Unit::Percent();
        
        // % * % 应该合并为 0.0001 的缩放因子
        auto percent_squared = percent * percent;
        EXPECT_EQ(percent_squared.dimension(), Dimension::Unit());
        EXPECT_NEAR(percent_squared.getScale(), 0.01 * 0.01, 1e-15);
        
        // % / % 应该合并为 1 的缩放因子
        auto percent_div_percent = percent / percent;
        EXPECT_EQ(percent_div_percent.dimension(), Dimension::Unit());
        EXPECT_NEAR(percent_div_percent.getScale(), 1.0, 1e-15);
        
        // 测试Scale单位的合并约简
        auto scale2 = Unit::Scale(2.0);
        auto scale3 = Unit::Scale(3.0);
        
        // Scale(2) * Scale(3) 应该合并为 Scale(6)
        auto scale6 = scale2 * scale3;
        EXPECT_EQ(scale6.dimension(), Dimension::Unit());
        EXPECT_NEAR(scale6.getScale(), 2.0 * 3.0, 1e-15);
        
        // Scale(6) / Scale(2) 应该合并为 Scale(3)
        auto scale3_from_div = scale6 / scale2;
        EXPECT_EQ(scale3_from_div.dimension(), Dimension::Unit());
        EXPECT_NEAR(scale3_from_div.getScale(), 6.0 / 2.0, 1e-15);
    }
    
    // 4. 综合测试：非国际单位制 + 非1幂次 + 常数单位的合并约简
    {
        // 测试 (km/h^2) * (m/s^2) * % 的合并约简
        auto km = Unit::Kilometer();
        auto h = Unit::Hour();
        auto m = Unit::Meter();
        auto s = Unit::Second();
        auto percent = Unit::Percent();
        
        auto km_h2 = km / h.pow(2);
        auto m_s2 = m / s.pow(2);
        
        auto result = km_h2 * m_s2 * percent;
        
        // 量纲应该是 (m^2) / (s^4)
        EXPECT_EQ(result.dimension(), Dimension::Length().pow(2) / Dimension::Time().pow(4));
        
        // 缩放因子计算：(1000 m / (3600^2 s^2)) * (1 m / 1 s^2) * 0.01
        double expected_scale = (1000.0 / (3600.0 * 3600.0)) * (1.0 / 1.0) * 0.01;
        EXPECT_NEAR(result.getScale(), expected_scale, 1e-15);
        
        // 测试更复杂的合并约简：(kg^2 * g^-1) * (m^3 * km^-2)
        auto kg = Unit::Kilogram();
        auto g = Unit::Gram();
        
        auto kg2_g_1 = kg.pow(2) / g;
        auto m3_km_2 = m.pow(3) / Unit::Kilometer().pow(2);
        
        auto complex_result = kg2_g_1 * m3_km_2;
        
        // 量纲应该是 kg^2 * g^-1 * m^3 * km^-2
        // 由于 g 和 kg 都是质量单位，km 和 m 都是长度单位，应该合并为 kg * m
        EXPECT_EQ(complex_result.dimension(), Dimension::Mass() * Dimension::Length());
        
        // 缩放因子计算：(1.0^2 kg^2) * (0.001 kg^-1) * (1.0^3 m^3) * (1000.0^-2 km^-2)
        // 转换为国际单位：1.0^2 * (0.001)^-1 * 1.0^3 * (1000)^-2
        double complex_scale = (1.0 * 1.0) * (1.0 / 0.001) * (1.0 * 1.0 * 1.0) * (1.0 / (1000.0 * 1000.0));
        EXPECT_NEAR(complex_result.getScale(), complex_scale, 1e-15);
    }
     // 5. 两步合并约简测试：非1/负数幂次非国际单位的合并与约简
    {
        // 5.1 时间单位测试
        {
            // 第一步：小时的负二次幂（非国际单位制，非1/负数幂次）乘以分钟（非国际单位制）
            auto h = Unit::Hour();
            auto min = Unit::Minute();
            auto h_neg2 = h.pow(-2);  // h^-2
            
            // h^-2 * min 应该合并为时间单位，量纲为 T^-2 * T = T^-1
            auto step1 = h_neg2 * min;
            EXPECT_EQ(step1.dimension(), Dimension::Time().pow(-1));
            
            // 缩放因子计算：h^-2 * min = (3600 s)^-2 * (60 s) = 60 / (3600^2) s^-1
            double expected_scale_step1 = 60.0 / (3600.0 * 3600.0);
            EXPECT_NEAR(step1.getScale(), expected_scale_step1, 1e-15);
            
            // 第二步：然后除以天（非国际单位制，量纲相同但缩放因子不同）
            auto day = Unit::Day();
            
            // (h^-2 * min) * day 应该约简为无量纲
            auto step2 = step1 * day;
            EXPECT_EQ(step2.dimension(), Dimension::Unit());
            
            // 缩放因子计算：[60 / (3600^2)] * 86400 = 60 / (3600^2 * 86400)
            double expected_scale_step2 = expected_scale_step1 * 86400.0;
            EXPECT_NEAR(step2.getScale(), expected_scale_step2, 1e-15);
        }
        
        // 5.2 长度单位测试
        {
            // 第一步：千米的二次幂（非国际单位制，非1幂次）除以厘米（非国际单位制）
            auto km = Unit::Kilometer();
            auto cm = Unit::Centimeter();
            auto km_sq = km.pow(2);  // km^2
            
            // km^2 / cm 应该合并为长度单位，量纲为 L^2 / L = L
            auto step1 = km_sq / cm;
            EXPECT_EQ(step1.dimension(), Dimension::Length());
            
            // 缩放因子计算：km^2 / cm = (1000 m)^2 / (0.01 m) = (1,000,000 m^2) / (0.01 m) = 100,000,000 m
            double expected_scale_step1 = (1000.0 * 1000.0) / 0.01;
            EXPECT_NEAR(step1.getScale(), expected_scale_step1, 1e-15);
            
            // 第二步：然后乘以毫米（非国际单位制，量纲相同但缩放因子不同）
            auto mm = Unit::Millimeter();
            
            // (km^2 / cm) * mm 应该合并为长度单位，量纲为 L * L = L^2
            auto step2 = step1 * mm;
            EXPECT_EQ(step2.dimension(), Dimension::Length().pow(2));
            
            // 缩放因子计算：100,000,000 m * 0.001 m = 100,000 m^2
            double expected_scale_step2 = expected_scale_step1 * 0.001;
            EXPECT_NEAR(step2.getScale(), expected_scale_step2, 1e-15);
        }
        
        // 5.3 质量单位测试
        {
            // 第一步：毫克的三次幂（非国际单位制，非1幂次）乘以克（非国际单位制）
            auto mg = Unit::Milligram();
            auto g = Unit::Gram();
            auto mg_cube = mg.pow(3);  // mg^3
            
            // mg^3 * g 应该合并为质量单位，量纲为 M^3 * M = M^4
            auto step1 = mg_cube * g;
            EXPECT_EQ(step1.dimension(), Dimension::Mass().pow(4));
            
            // 缩放因子计算：mg^3 * g = (0.000001 kg)^3 * (0.001 kg) = (1e-18 kg^3) * (1e-3 kg) = 1e-21 kg^4
            double expected_scale_step1 = std::pow(0.000001, 3) * 0.001;
            EXPECT_NEAR(step1.getScale(), expected_scale_step1, 1e-15);
            
            // 第二步：然后除以千克（非国际单位制，量纲相同但缩放因子不同）
            auto kg = Unit::Kilogram();
            
            // (mg^3 * g) / kg 应该约简为质量单位，量纲为 M^4 / M = M^3
            auto step2 = step1 / kg;
            EXPECT_EQ(step2.dimension(), Dimension::Mass().pow(3));
            
            // 缩放因子计算：1e-21 kg^4 / 1 kg = 1e-21 kg^3
            double expected_scale_step2 = expected_scale_step1 / 1.0;
            EXPECT_NEAR(step2.getScale(), expected_scale_step2, 1e-15);
        }
        
        // 5.4 面积单位测试
        {
            // 第一步：平方千米的负一次幂（非国际单位制，非1/负数幂次）乘以平方英尺（非国际单位制）
            auto km2 = Unit::SquareKilometer();
            auto ft2 = Unit::SquareFoot();
            auto km2_neg1 = km2.pow(-1);  // km^-2
            
            // km^-2 * ft^2 应该合并为无量纲，因为量纲为 L^-2 * L^2 = L^0
            auto step1 = km2_neg1 * ft2;
            EXPECT_EQ(step1.dimension(), Dimension::Unit());
            
            // 缩放因子计算：km^-2 * ft^2 = (1e6 m^2)^-1 * (0.092903 m^2) = 0.092903 / 1e6
            const double sf = 0.3048 * 0.3048;
            double expected_scale_step1 = 0.09290304 / 1000000.0;
            EXPECT_NEAR(step1.getScale(), expected_scale_step1, 1e-15);
            
            // 第二步：然后乘以平方码（非国际单位制，量纲相同但缩放因子不同）
            auto yd2 = Unit::SquareYard();
            
            // (km^-2 * ft^2) * yd^2 应该合并为面积单位，量纲为 L^0 * L^2 = L^2
            auto step2 = step1 * yd2;
            EXPECT_EQ(step2.dimension(), Dimension::Area());
            
            // 缩放因子计算：(0.092903 / 1e6) * 0.836127 m^2
            const double sy = 0.9144 * 0.9144;
            double expected_scale_step2 = expected_scale_step1 * 0.83612736;
            EXPECT_NEAR(step2.getScale(), expected_scale_step2, 1e-15);
        }
        
        // 5.5 速度单位测试
        {
            // 第一步：千米每小时的二次幂（非国际单位制，非1幂次）除以米每分钟（非国际单位制）
            auto kmh = Unit::KilometerPerHour();
            auto mpm = Unit::MeterPerMinute();
            auto kmh_sq = kmh.pow(2);  // (km/h)^2
            
            // (km/h)^2 / (m/min) 应该合并为速度单位，量纲为 (L/T)^2 / (L/T) = L/T
            auto step1 = kmh_sq / mpm;
            EXPECT_EQ(step1.dimension(), Dimension::Speed());
            
            // 缩放因子计算：(km/h)^2 / (m/min) = (1000 m / 3600 s)^2 / (1 m / 60 s)
            double kmh_in_mps = 1000.0 / 3600.0;  // m/s
            double mpm_in_mps = 1.0 / 60.0;  // m/s
            double expected_scale_step1 = (kmh_in_mps * kmh_in_mps) / mpm_in_mps;
            EXPECT_NEAR(step1.getScale(), expected_scale_step1, 1e-15);
            
            // 第二步：然后乘以厘米每秒（非国际单位制，量纲相同但缩放因子不同）
            auto cmps = Unit::CentimeterPerSecond();
            
            // [(km/h)^2 / (m/min)] * (cm/s) 应该合并为加速度单位，量纲为 (L/T) * (L/T) = L^2/T^2
            auto step2 = step1 * cmps;
            EXPECT_EQ(step2.dimension(), Dimension::Acceleration() * Dimension::Length());
            
            // 缩放因子计算：[ (1000/3600)^2 / (1/60) ] * 0.01
            double expected_scale_step2 = expected_scale_step1 * 0.01;
            EXPECT_NEAR(step2.getScale(), expected_scale_step2, 1e-15);
        }
        {
            auto km2_h_min = Unit::Kilometer() * Unit::Kilometer() / Unit::Hour() / Unit::Minute();
            auto unit = km2_h_min * Unit::Day() * Unit::Second() / Unit::Centimeter();
            auto expect_scale = 1e3 * 1e3 / 3600 / 60 * 86400 * 1 / 0.01;
            EXPECT_DOUBLE_EQ(expect_scale, unit.getScale());
            EXPECT_EQ(unit.dimension(), Dimension::Length());
        }
    }
}

// 测试单位倒数运算
TEST(Unit, Invert)
{
    // 测试基本单位的倒数
    {
        Unit meter = Unit::Meter();
        Unit inverted = meter.invert();
        EXPECT_EQ(inverted.dimension(), meter.dimension().invert());
        EXPECT_DOUBLE_EQ(inverted.getScale(), 1.0 / meter.getScale());
    }

    // 测试复合单位的倒数
    {
        Unit speed = Unit::Meter() / Unit::Second(); // m/s
        Unit inverted = speed.invert(); // s/m
        EXPECT_EQ(inverted.dimension(), Dimension::Time() / Dimension::Length());
        EXPECT_DOUBLE_EQ(inverted.getScale(), 1.0 / speed.getScale());
    }

    // 测试倒数的倒数是否等于原单位
    {
        Unit force = Unit::Newton(); // N
        Unit inverted1 = force.invert();
        Unit inverted2 = inverted1.invert();
        EXPECT_TRUE(inverted2 == force);
    }

    // 测试带名称的单位的倒数
    {
        Unit kg = Unit::Kilogram();
        Unit inverted = kg.invert();
        // 倒数单位可能没有名称，所以只测试维度和缩放因子
        EXPECT_EQ(inverted.dimension(), Dimension::Mass().invert());
        EXPECT_DOUBLE_EQ(inverted.getScale(), 1.0);
    }

    // 测试特殊单位的倒数
    {
        Unit none = Unit::None();
        Unit inverted = none.invert();
        EXPECT_TRUE(inverted == none);
    }
}

GTEST_MAIN()