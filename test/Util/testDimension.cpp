///
/// @file      testDimension.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-12
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

#include "AstUtil/Dimension.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

// 测试基本量纲
TEST(Dimension, BasicDimensions)
{
    // 测试默认构造函数
    Dimension unit;
    EXPECT_EQ(unit.value(), EDimension::eUnit);
    EXPECT_EQ(unit.getMass(), 0);
    EXPECT_EQ(unit.getLength(), 0);
    EXPECT_EQ(unit.getTime(), 0);
    EXPECT_EQ(unit.getAngle(), 0);
    EXPECT_EQ(unit.getCurrent(), 0);
    EXPECT_EQ(unit.getTemperature(), 0);
    EXPECT_EQ(unit.getAmount(), 0);
    EXPECT_EQ(unit.getLuminous(), 0);

    // 测试质量量纲
    Dimension mass(EDimension::eMass);
    EXPECT_EQ(mass.value(), EDimension::eMass);
    EXPECT_EQ(mass.getMass(), 1);
    EXPECT_EQ(mass.getLength(), 0);
    EXPECT_EQ(mass.getTime(), 0);

    // 测试长度量纲
    Dimension length(EDimension::eLength);
    EXPECT_EQ(length.value(), EDimension::eLength);
    EXPECT_EQ(length.getMass(), 0);
    EXPECT_EQ(length.getLength(), 1);
    EXPECT_EQ(length.getTime(), 0);

    // 测试时间量纲
    Dimension time(EDimension::eTime);
    EXPECT_EQ(time.value(), EDimension::eTime);
    EXPECT_EQ(time.getMass(), 0);
    EXPECT_EQ(time.getLength(), 0);
    EXPECT_EQ(time.getTime(), 1);

    // 测试电流、温度、物质量、发光强度量纲
    Dimension current(EDimension::eCurrent);
    EXPECT_EQ(current.getCurrent(), 1);

    Dimension temperature(EDimension::eTemperature);
    EXPECT_EQ(temperature.getTemperature(), 1);

    Dimension amount(EDimension::eAmount);
    EXPECT_EQ(amount.getAmount(), 1);

    Dimension luminous(EDimension::eLuminous);
    EXPECT_EQ(luminous.getLuminous(), 1);

    // 测试角度量纲
    Dimension angle(EDimension::eAngle);
    EXPECT_EQ(angle.getAngle(), 1);
}

// 测试静态工厂方法
TEST(Dimension, StaticFactoryMethods)
{
    EXPECT_EQ(Dimension::Unit().value(), EDimension::eUnit);
    EXPECT_EQ(Dimension::Mass().value(), EDimension::eMass);
    EXPECT_EQ(Dimension::Length().value(), EDimension::eLength);
    EXPECT_EQ(Dimension::Time().value(), EDimension::eTime);
    EXPECT_EQ(Dimension::Current().value(), EDimension::eCurrent);
    EXPECT_EQ(Dimension::Temperature().value(), EDimension::eTemperature);
    EXPECT_EQ(Dimension::Amount().value(), EDimension::eAmount);
    EXPECT_EQ(Dimension::Luminous().value(), EDimension::eLuminous);
    EXPECT_EQ(Dimension::Angle().value(), EDimension::eAngle);
}

// 测试导出量纲
TEST(Dimension, DerivedDimensions)
{
    // 测试面积
    Dimension area(EDimension::eArea);
    EXPECT_EQ(area.getLength(), 2);
    EXPECT_EQ(area.getMass(), 0);
    EXPECT_EQ(area.getTime(), 0);

    // 测试体积
    Dimension volume(EDimension::eVolume);
    EXPECT_EQ(volume.getLength(), 3);
    EXPECT_EQ(volume.getMass(), 0);
    EXPECT_EQ(volume.getTime(), 0);

    // 测试速度
    Dimension speed(EDimension::eSpeed);
    EXPECT_EQ(speed.getLength(), 1);
    EXPECT_EQ(speed.getTime(), -1);

    // 测试加速度
    Dimension acceleration(EDimension::eAcceleration);
    EXPECT_EQ(acceleration.getLength(), 1);
    EXPECT_EQ(acceleration.getTime(), -2);

    // 测试力
    Dimension force(EDimension::eForce);
    EXPECT_EQ(force.getMass(), 1);
    EXPECT_EQ(force.getLength(), 1);
    EXPECT_EQ(force.getTime(), -2);

    // 测试压力
    Dimension pressure(EDimension::ePressure);
    EXPECT_EQ(pressure.getMass(), 1);
    EXPECT_EQ(pressure.getLength(), -1);
    EXPECT_EQ(pressure.getTime(), -2);

    // 测试能量
    Dimension energy(EDimension::eEnergy);
    EXPECT_EQ(energy.getMass(), 1);
    EXPECT_EQ(energy.getLength(), 2);
    EXPECT_EQ(energy.getTime(), -2);

    // 测试功率
    Dimension power(EDimension::ePower);
    EXPECT_EQ(power.getMass(), 1);
    EXPECT_EQ(power.getLength(), 2);
    EXPECT_EQ(power.getTime(), -3);

    // 测试频率
    Dimension frequency(EDimension::eFrequency);
    EXPECT_EQ(frequency.getTime(), -1);

    // 测试角速度
    Dimension angVel(EDimension::eAngVel);
    EXPECT_EQ(angVel.getAngle(), 1);
    EXPECT_EQ(angVel.getTime(), -1);
}

// 测试量纲乘法运算
TEST(Dimension, Multiplication)
{
    // 基本量纲相乘
    Dimension massLength = Dimension::Mass() * Dimension::Length();
    EXPECT_EQ(massLength.getMass(), 1);
    EXPECT_EQ(massLength.getLength(), 1);

    // 导出量纲相乘
    Dimension speedArea = Dimension::Speed() * Dimension::Area();
    EXPECT_EQ(speedArea.getLength(), 3);
    EXPECT_EQ(speedArea.getTime(), -1);

    // 乘法赋值运算符
    Dimension velocity = Dimension::Speed();
    velocity *= Dimension::Time();
    EXPECT_EQ(velocity.getLength(), 1);
    EXPECT_EQ(velocity.getTime(), 0);

    // 多个量纲相乘
    Dimension complex = Dimension::Mass() * Dimension::Length() * Dimension::Length() * Dimension::Time() * Dimension::Time() * Dimension::Time();
    EXPECT_EQ(complex.getMass(), 1);
    EXPECT_EQ(complex.getLength(), 2);
    EXPECT_EQ(complex.getTime(), 3);
}

// 测试量纲除法运算
TEST(Dimension, Division)
{
    // 基本量纲相除
    Dimension massOverLength = Dimension::Mass() / Dimension::Length();
    EXPECT_EQ(massOverLength.getMass(), 1);
    EXPECT_EQ(massOverLength.getLength(), -1);

    // 导出量纲相除
    Dimension speedOverTime = Dimension::Speed() / Dimension::Time();
    EXPECT_EQ(speedOverTime.getLength(), 1);
    EXPECT_EQ(speedOverTime.getTime(), -2);

    // 除法赋值运算符
    Dimension area = Dimension::Area();
    area /= Dimension::Length();
    EXPECT_EQ(area.getLength(), 1);

    // 多个量纲相除
    Dimension complex = Dimension::Mass() / Dimension::Length() / Dimension::Time() / Dimension::Time();
    EXPECT_EQ(complex.getMass(), 1);
    EXPECT_EQ(complex.getLength(), -1);
    EXPECT_EQ(complex.getTime(), -2);
}

// 测试量纲幂运算
TEST(Dimension, Power)
{
    // 基本量纲的幂
    Dimension lengthSquared = Dimension::Length().pow(2);
    EXPECT_EQ(lengthSquared.getLength(), 2);

    Dimension timeCubed = Dimension::Time().pow(3);
    EXPECT_EQ(timeCubed.getTime(), 3);

    // 导出量纲的幂
    Dimension areaSquared = Dimension::Area().pow(2);
    EXPECT_EQ(areaSquared.getLength(), 4);

    // 负幂运算
    Dimension lengthInverse = Dimension::Length().pow(-1);
    EXPECT_EQ(lengthInverse.getLength(), -1);

    // 零次幂
    Dimension unitPower = Dimension::Mass().pow(0);
    EXPECT_EQ(unitPower.getMass(), 0);
    EXPECT_EQ(unitPower.getLength(), 0);
    EXPECT_EQ(unitPower.getTime(), 0);
    EXPECT_EQ(unitPower.value(), EDimension::eUnit);
}

// 测试复合运算
TEST(Dimension, ComplexOperations)
{
    // 测试速度 = 长度 / 时间
    Dimension speed1 = Dimension::Length() / Dimension::Time();
    Dimension speed2 = Dimension::Speed();
    EXPECT_EQ(speed1.getLength(), speed2.getLength());
    EXPECT_EQ(speed1.getTime(), speed2.getTime());

    // 测试加速度 = 速度 / 时间
    Dimension acceleration1 = Dimension::Speed() / Dimension::Time();
    Dimension acceleration2 = Dimension::Acceleration();
    EXPECT_EQ(acceleration1.getLength(), acceleration2.getLength());
    EXPECT_EQ(acceleration1.getTime(), acceleration2.getTime());

    // 测试力 = 质量 * 加速度
    Dimension force1 = Dimension::Mass() * Dimension::Acceleration();
    Dimension force2 = Dimension::Force();
    EXPECT_EQ(force1.getMass(), force2.getMass());
    EXPECT_EQ(force1.getLength(), force2.getLength());
    EXPECT_EQ(force1.getTime(), force2.getTime());

    // 测试能量 = 力 * 长度
    Dimension energy1 = Dimension::Force() * Dimension::Length();
    Dimension energy2 = Dimension::Energy();
    EXPECT_EQ(energy1.getMass(), energy2.getMass());
    EXPECT_EQ(energy1.getLength(), energy2.getLength());
    EXPECT_EQ(energy1.getTime(), energy2.getTime());

    // 测试功率 = 能量 / 时间
    Dimension power1 = Dimension::Energy() / Dimension::Time();
    Dimension power2 = Dimension::Power();
    EXPECT_EQ(power1.getMass(), power2.getMass());
    EXPECT_EQ(power1.getLength(), power2.getLength());
    EXPECT_EQ(power1.getTime(), power2.getTime());
}

// 测试常量表达式
TEST(Dimension, Constexpr)
{
    // 测试常量构造
    constexpr Dimension constUnit = Dimension::Unit();
    constexpr Dimension constMass = Dimension::Mass();
    constexpr Dimension constLength = Dimension::Length();
    constexpr Dimension constTime = Dimension::Time();

    // 测试常量运算
    constexpr Dimension constArea = constLength * constLength;
    static_assert(constArea.getLength() == 2, "Area should have length exponent 2");

    constexpr Dimension constSpeed = constLength / constTime;
    static_assert(constSpeed.getLength() == 1, "Speed should have length exponent 1");
    static_assert(constSpeed.getTime() == -1, "Speed should have time exponent -1");

    constexpr Dimension constAcceleration = constSpeed / constTime;
    static_assert(constAcceleration.getLength() == 1, "Acceleration should have length exponent 1");
    static_assert(constAcceleration.getTime() == -2, "Acceleration should have time exponent -2");
}




// 测试量纲set接口
TEST(Dimension, SetInterface)
{
    // 测试基本的set方法
    {
        Dimension dim;
        
        // 测试setMass
        dim.setMass(2);
        EXPECT_EQ(dim.getMass(), 2);
        
        // 测试setLength
        dim.setLength(3);
        EXPECT_EQ(dim.getLength(), 3);
        
        // 测试setTime
        dim.setTime(-1);
        EXPECT_EQ(dim.getTime(), -1);
        
        // 测试setCurrent
        dim.setCurrent(1);
        EXPECT_EQ(dim.getCurrent(), 1);
        
        // 测试setAngle
        dim.setAngle(-2);
        EXPECT_EQ(dim.getAngle(), -2);
        
        // 测试setTemperature
        dim.setTemperature(0);
        EXPECT_EQ(dim.getTemperature(), 0);
        
        // 测试setAmount
        dim.setAmount(1);
        EXPECT_EQ(dim.getAmount(), 1);
        
        // 测试setLuminous
        dim.setLuminous(-1);
        EXPECT_EQ(dim.getLuminous(), -1);
    }
    
    // 测试链式调用
    {
        Dimension dim;
        dim.setMass(1)
           .setLength(2)
           .setTime(-3)
           .setCurrent(4)
           .setAngle(-5)
           .setTemperature(6)
           .setAmount(-7)
           .setLuminous(0);
        
        EXPECT_EQ(dim.getMass(), 1);
        EXPECT_EQ(dim.getLength(), 2);
        EXPECT_EQ(dim.getTime(), -3);
        EXPECT_EQ(dim.getCurrent(), 4);
        EXPECT_EQ(dim.getAngle(), -5);
        EXPECT_EQ(dim.getTemperature(), 6);
        EXPECT_EQ(dim.getAmount(), -7);
        EXPECT_EQ(dim.getLuminous(), 0);
    }
    
    // 测试覆盖已有值
    {
        Dimension dim;
        dim.setMass(5);
        EXPECT_EQ(dim.getMass(), 5);
        
        // 覆盖现有值
        dim.setMass(-3);
        EXPECT_EQ(dim.getMass(), -3);
    }
    
    // 测试从非单位量纲开始设置
    {
        Dimension dim(EDimension::eForce); // Force = M*L*T^-2
        
        // 修改一个维度
        dim.setMass(2);
        EXPECT_EQ(dim.getMass(), 2);
        EXPECT_EQ(dim.getLength(), 1);
        EXPECT_EQ(dim.getTime(), -2);
        
        // 修改另一个维度
        dim.setTime(-1);
        EXPECT_EQ(dim.getMass(), 2);
        EXPECT_EQ(dim.getLength(), 1);
        EXPECT_EQ(dim.getTime(), -1);
    }
    
    // 测试设置为零值
    {
        Dimension dim(EDimension::eArea); // Area = L^2
        
        dim.setLength(0);
        EXPECT_EQ(dim.getLength(), 0);
        EXPECT_EQ(dim.getMass(), 0);
        EXPECT_EQ(dim.getTime(), 0);
        // 验证是否为单位量纲
        EXPECT_EQ(dim.value(), EDimension::eUnit);
    }
    
    // 测试与get方法配合使用
    {
        Dimension dim;
        int massExponent = 3;
        int lengthExponent = -2;
        int timeExponent = 1;
        
        dim.setMass(massExponent);
        dim.setLength(lengthExponent);
        dim.setTime(timeExponent);
        
        EXPECT_EQ(dim.getMass(), massExponent);
        EXPECT_EQ(dim.getLength(), lengthExponent);
        EXPECT_EQ(dim.getTime(), timeExponent);
    }
}



GTEST_MAIN()