/// @file      testJulianDate.cpp
/// @brief     测试儒略日类的功能
/// @details   测试 JulianDate 类及其相关转换函数
/// @author    axel
/// @date      04.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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


#include "AstCore/JulianDate.hpp"
#include "AstCore/Date.hpp"
#include "AstTest/AstTestMacro.h"
#include "AstUtil/Constants.h"
#include "AstCore/ModJulianDate.hpp"
#include <stdio.h>


TEST(JulianDate, Constructor)
{
    AST_USING_NAMESPACE
    
    // 测试默认构造
    JulianDate jd;
    jd.day() = 2451545.0;
    jd.second() = 0.0;
    EXPECT_DOUBLE_EQ(jd.day(), 2451545.0);
    EXPECT_DOUBLE_EQ(jd.second(), 0.0);
    
    // 测试静态构造方法 FromImpreciseDay
    JulianDate jd2 = JulianDate::FromImpreciseDay(2451545.5);
    EXPECT_DOUBLE_EQ(jd2.day(), 2451545.0);
    EXPECT_DOUBLE_EQ(jd2.second(), 43200.0); // 12小时 = 43200秒
    
    // 测试静态构造方法 FromDaySecond
    JulianDate jd3 = JulianDate::FromDaySecond(2451545.0, 86400.0);
    EXPECT_DOUBLE_EQ(jd3.day(), 2451545.0);
    EXPECT_DOUBLE_EQ(jd3.second(), 86400.0);
    
    // 测试静态构造方法 FromDayTwoPart
    // JulianDate jd4 = JulianDate::FromDayTwoPart(2451545.0, 0.75);
    // EXPECT_DOUBLE_EQ(jd4.day(), 2451545.0);
    // EXPECT_DOUBLE_EQ(jd4.second(), 64800.0); // 0.75天 = 18小时 = 64800秒
}

TEST(JulianDate, AccessorMethods)
{
    AST_USING_NAMESPACE
    
    JulianDate jd;
    jd.setDay(2451545.0);
    jd.setSecond(43200.0); // 12小时
    
    // 测试基本访问器
    EXPECT_DOUBLE_EQ(jd.day(), 2451545.0);
    EXPECT_DOUBLE_EQ(jd.second(), 43200.0);
    
    // 测试 getDaySecond 和 setDaySecond
    int day;double second;
    jd.getDaySecond(day, second);
    EXPECT_DOUBLE_EQ(day, 2451545.0);
    EXPECT_DOUBLE_EQ(second, 43200.0);
    
    jd.setDaySecond(2451546.0, 0.0);
    EXPECT_DOUBLE_EQ(jd.day(), 2451546.0);
    EXPECT_DOUBLE_EQ(jd.second(), 0.0);
    
    // 测试 impreciseDay 和 setImpreciseDay
    double imprecise = jd.impreciseDay();
    EXPECT_DOUBLE_EQ(imprecise, 2451546.0);
    
    jd.setImpreciseDay(2451546.25);
    EXPECT_DOUBLE_EQ(jd.day(), 2451546.0);
    EXPECT_DOUBLE_EQ(jd.second(), 21600.0); // 0.25天 = 6小时 = 21600秒
    EXPECT_DOUBLE_EQ(jd.impreciseDay(), 2451546.25);
}

TEST(JulianDate, TwoPartMethods)
{
    AST_USING_NAMESPACE
    
    JulianDate jd;
    jd.setDay(2451545.0);
    jd.setDayFractional(0.5); // 12小时
    
    EXPECT_DOUBLE_EQ(jd.day(), 2451545.0);
    EXPECT_DOUBLE_EQ(jd.dayFractional(), 0.5);
    
}

TEST(JulianDate, JDToMJDConversion)
{
    AST_USING_NAMESPACE
    
    // 测试 JD 到 MJD 的转换
    JulianDate jd;
    jd.setDay(2451545.0);
    jd.setSecond(0.0); // 2000-01-01 12:00:00
    
    ModJulianDate mjd;
    aJDToMJD(jd, mjd);
    
    // MJD = JD - 2400000.5
    double expectedMJD = 2451545.0 - kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(mjd.impreciseDay(), expectedMJD);
    
}

TEST(JulianDate, MJDToJDConversion)
{
    AST_USING_NAMESPACE
    
    // 测试 MJD 到 JD 的转换
    ModJulianDate mjd;
    mjd.setDay(51544.0); // 对应 JD 2451544.5 (2000-01-01 00:00:00)
    mjd.setSecond(0.0);
    
    JulianDate jd;
    aMJDToJD(mjd, jd);
    
    // JD = MJD + 2400000.5
    double expectedJD = 51544.0 + kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(jd.impreciseDay(), expectedJD);
    
}

TEST(JulianDate, ImpreciseConversion)
{
    AST_USING_NAMESPACE
    
    // 测试 aJDToMJD_Imprecise (JulianDate 版本)
    JulianDate jd;
    jd.setDay(2451545.0);
    jd.setSecond(43200.0); // 12小时
    
    double mjdValue = aJDToMJD_Imprecise(jd);
    double expectedMJD = 2451545.0 + (43200.0 / 86400.0) - kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(mjdValue, expectedMJD);
    
    // 测试 aMJDToJD_Imprecise (ModJulianDate 版本)
    ModJulianDate mjd;
    mjd.setDay(51544.0);
    mjd.setSecond(43200.0); // 12小时
    
    ImpreciseJD jdValue = aMJDToJD_Imprecise(mjd);
    ImpreciseJD expectedJD = 51544.0 + (43200.0 / 86400.0) + kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(jdValue, expectedJD);
    
    // 测试 aJDToMJD_Imprecise (ImpreciseJD 版本)
    ImpreciseJD impreciseJD = 2451545.5; // 2000-01-01 18:00:00
    ImpreciseMJD impreciseMJD = aJDToMJD_Imprecise(impreciseJD);
    ImpreciseMJD expectedImpreciseMJD = impreciseJD - kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(impreciseMJD, expectedImpreciseMJD);
    
    // 测试 aMJDToJD_Imprecise (ImpreciseMJD 版本)
    impreciseMJD = 51544.5; // 对应 JD 2451545.0 (2000-01-01 12:00:00)
    impreciseJD = aMJDToJD_Imprecise(impreciseMJD);
    auto expectedImpreciseJD = impreciseMJD + kMJDRefEpoch;
    EXPECT_DOUBLE_EQ(impreciseJD, expectedImpreciseJD);
}

TEST(JulianDate, RoundTripConversion)
{
    AST_USING_NAMESPACE
    
    // 测试 JD <-> MJD 的往返转换
    JulianDate jd1;
    jd1.setDay(2451545.0);
    jd1.setSecond(43200.0); // 12小时
    
    ModJulianDate mjd;
    aJDToMJD(jd1, mjd);
    
    JulianDate jd2;
    aMJDToJD(mjd, jd2);
    
    EXPECT_DOUBLE_EQ(jd1.day(), jd2.day());
    EXPECT_DOUBLE_EQ(jd1.second(), jd2.second());
    
    // 测试不精确版本的往返转换
    ImpreciseJD impreciseJD1 = 2451545.5;
    ImpreciseMJD impreciseMJD = aJDToMJD_Imprecise(impreciseJD1);
    ImpreciseJD impreciseJD2 = aMJDToJD_Imprecise(impreciseMJD);
    
    EXPECT_DOUBLE_EQ(impreciseJD1, impreciseJD2);
}

TEST(JulianDate, EdgeCases)
{
    AST_USING_NAMESPACE
    
    // 测试零值
    JulianDate jd;
    jd.setDay(0.0);
    jd.setSecond(0.0);
    
    EXPECT_DOUBLE_EQ(jd.impreciseDay(), 0.0);
    EXPECT_DOUBLE_EQ(jd.day(), 0.0);
    EXPECT_DOUBLE_EQ(jd.dayFractional(), 0.0);
    
    // 测试负值
    jd.setDay(-1000.0);
    jd.setSecond(-43200.0); // -12小时
    
    EXPECT_DOUBLE_EQ(jd.impreciseDay(), -1000.5);
    EXPECT_DOUBLE_EQ(jd.day(), -1000.0);
    EXPECT_DOUBLE_EQ(jd.dayFractional(), -0.5);
    
    // 测试大数值
    JulianDate jdLarge;
    jdLarge.setDay(1e9);
    jdLarge.setSecond(86399.0); // 接近一天结束
    
    EXPECT_DOUBLE_EQ(jdLarge.impreciseDay(), 1e9 + 86399.0 / 86400.0);
    EXPECT_DOUBLE_EQ(jdLarge.day(), 1e9);
    EXPECT_DOUBLE_EQ(jdLarge.dayFractional(), 86399.0 / 86400.0);
}

GTEST_MAIN()