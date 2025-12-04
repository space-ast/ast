/// @file      testDate.cpp
/// @brief     测试日期类的功能
/// @details   ~
/// @author    jinke18
/// @date      21.11.2025
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


#include "AstCore/Date.hpp"
#include "AstTest/AstTestMacro.h"
#include <stdio.h>


TEST(Date, BasicConstructor) 
{
    AST_USING_NAMESPACE
    
    // 测试默认构造
    Date date;
    date.year() = 2023;
    date.month() = 12;
    date.day() = 25;
    EXPECT_EQ(date.year(), 2023);
    EXPECT_EQ(date.month(), 12);
    EXPECT_EQ(date.day(), 25);
    
    // 测试静态构造方法 FromYMD
    Date date2 = Date::FromYMD(2024, 2, 29);
    EXPECT_EQ(date2.year(), 2024);
    EXPECT_EQ(date2.month(), 2);
    EXPECT_EQ(date2.day(), 29);
}

TEST(Date, LeapYear) 
{
    AST_USING_NAMESPACE
    
    // 测试闰年判断
    EXPECT_TRUE(aIsLeapYear(2000));   // 能被400整除的年份是闰年
    EXPECT_TRUE(aIsLeapYear(2024));   // 能被4整除但不能被100整除的年份是闰年
    EXPECT_FALSE(aIsLeapYear(1900));  // 能被100整除但不能被400整除的年份不是闰年
    EXPECT_FALSE(aIsLeapYear(2023));  // 不能被4整除的年份不是闰年
    
    // 测试Date类中的isLeapYear方法
    Date date = Date::FromYMD(2024, 3, 15);
    EXPECT_TRUE(date.isLeapYear());
    
    date.year() = 2023;
    EXPECT_FALSE(date.isLeapYear());
}

TEST(Date, DayInMonth) 
{
    AST_USING_NAMESPACE
    
    // 测试非闰年各月天数
    EXPECT_EQ(aDaysInMonthByLeap(1, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(2, false), 28);
    EXPECT_EQ(aDaysInMonthByLeap(3, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(4, false), 30);
    EXPECT_EQ(aDaysInMonthByLeap(5, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(6, false), 30);
    EXPECT_EQ(aDaysInMonthByLeap(7, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(8, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(9, false), 30);
    EXPECT_EQ(aDaysInMonthByLeap(10, false), 31);
    EXPECT_EQ(aDaysInMonthByLeap(11, false), 30);
    EXPECT_EQ(aDaysInMonthByLeap(12, false), 31);
    
    // 测试闰年2月天数
    EXPECT_EQ(aDaysInMonthByLeap(2, true), 29);
    
    // 测试带年份的方法
    EXPECT_EQ(aDaysInMonthByYear(2, 2024), 29);  // 闰年
    EXPECT_EQ(aDaysInMonthByYear(2, 2023), 28);  // 非闰年
    
    // 测试Date类中的dayInMonth方法
    Date date = Date::FromYMD(2024, 2, 1);
    EXPECT_EQ(date.dayInMonth(), 29);
    
    date.year() = 2023;
    EXPECT_EQ(date.dayInMonth(), 28);
}

TEST(Date, DayOfYear) 
{
    AST_USING_NAMESPACE
    
    // 测试非闰年的年中日计算
    Date date1 = Date::FromYMD(2023, 1, 1);
    EXPECT_EQ(date1.dayOfYear(), 1);
    
    Date date2 = Date::FromYMD(2023, 3, 1);
    EXPECT_EQ(date2.dayOfYear(), 60);  // 1月31天 + 2月28天 = 59，+1得60
    
    Date date3 = Date::FromYMD(2023, 12, 31);
    EXPECT_EQ(date3.dayOfYear(), 365);
    
    // 测试闰年的年中日计算
    Date date4 = Date::FromYMD(2024, 3, 1);
    EXPECT_EQ(date4.dayOfYear(), 61);  // 1月31天 + 2月29天 = 60，+1得61
    
    Date date5 = Date::FromYMD(2024, 12, 31);
    EXPECT_EQ(date5.dayOfYear(), 366);
    
    // 测试独立函数
    EXPECT_EQ(aDayOfYear(date1), 1);
    EXPECT_EQ(aDayOfYear(date5), 366);
}

TEST(Date, JulianDateConversion) 
{
    AST_USING_NAMESPACE
    
    // 测试已知的儒略日转换（2000-01-01的儒略日是2451545）
    Date date1 = Date::FromYMD(2000, 1, 1);
    int jd1 = date1.toJD();
    EXPECT_EQ(jd1, 2451545);
    
    // 测试儒略日转日期
    Date date2;
    date2.fromJD(jd1);
    EXPECT_EQ(date2.year(), 2000);
    EXPECT_EQ(date2.month(), 1);
    EXPECT_EQ(date2.day(), 1);
    
    // 测试静态方法FromJD
    Date date3 = Date::FromJD(jd1);
    EXPECT_EQ(date3.year(), 2000);
    EXPECT_EQ(date3.month(), 1);
    EXPECT_EQ(date3.day(), 1);
    
    // 测试其他日期
    Date date4 = Date::FromYMD(2023, 12, 25);
    int jd4 = date4.toJD();
    Date date5 = Date::FromJD(jd4);
    EXPECT_EQ(date5.year(), 2023);
    EXPECT_EQ(date5.month(), 12);
    EXPECT_EQ(date5.day(), 25);
}

TEST(Date, YearDayConversion) 
{
    AST_USING_NAMESPACE
    
    // 测试日期转年中日
    Date date1 = Date::FromYMD(2023, 3, 1);
    int year, days;
    date1.toYD(year, days);
    EXPECT_EQ(year, 2023);
    EXPECT_EQ(days, 60);
    
    // 测试年中日转日期（非闰年）
    Date date2;
    date2.fromYD(2023, 60);
    EXPECT_EQ(date2.year(), 2023);
    EXPECT_EQ(date2.month(), 3);
    EXPECT_EQ(date2.day(), 1);
    
    // 测试年中日转日期（闰年）
    Date date3;
    date3.fromYD(2024, 61);  // 2024年第61天应该是3月1日
    EXPECT_EQ(date3.year(), 2024);
    EXPECT_EQ(date3.month(), 3);
    EXPECT_EQ(date3.day(), 1);
    
    // 测试静态方法FromYD
    Date date4 = Date::FromYD(2023, 60);
    EXPECT_EQ(date4.year(), 2023);
    EXPECT_EQ(date4.month(), 3);
    EXPECT_EQ(date4.day(), 1);
    
    // 测试边界情况
    Date date5 = Date::FromYD(2023, 365);  // 年末
    EXPECT_EQ(date5.year(), 2023);
    EXPECT_EQ(date5.month(), 12);
    EXPECT_EQ(date5.day(), 31);
}

TEST(Date, MonthWeekdayNames) 
{
    AST_USING_NAMESPACE
    
    // 测试月份完整名称
    EXPECT_STREQ(aMoonFullName(1), "January");
    EXPECT_STREQ(aMoonFullName(2), "February");
    EXPECT_STREQ(aMoonFullName(12), "December");
    EXPECT_STREQ(aMoonFullName(13), "?");  // 越界情况
    
    // 测试月份简写名称
    EXPECT_STREQ(aMoonShortName(1), "Jan");
    EXPECT_STREQ(aMoonShortName(2), "Feb");
    EXPECT_STREQ(aMoonShortName(12), "Dec");
    EXPECT_STREQ(aMoonShortName(-1), "???");  // 越界情况
    
    // 测试星期几完整名称
    EXPECT_STREQ(aWeekDayFullName(0), "Sunday");
    EXPECT_STREQ(aWeekDayFullName(1), "Monday");
    EXPECT_STREQ(aWeekDayFullName(6), "Saturday");
    EXPECT_STREQ(aWeekDayFullName(7), "?");  // 越界情况
    
    // 测试星期几简写名称
    EXPECT_STREQ(aWeekDayShortName(0), "Sun");
    EXPECT_STREQ(aWeekDayShortName(1), "Mon");
    EXPECT_STREQ(aWeekDayShortName(6), "Sat");
    EXPECT_STREQ(aWeekDayShortName(-1), "???");  // 越界情况
}

TEST(Date, DayInWeek)
{
    AST_USING_NAMESPACE

    {
        // 测试日期转星期几
        Date date = Date::FromYMD(2025, 11, 29);  // 2025年11月29日是星期三
        int weekday = date.dayOfWeek();
        EXPECT_EQ(weekday, 6);  // 0代表星期天，1代表星期一，以此类推
    }

    {
        // 测试日期转星期几
        Date date = Date::FromYMD(2023, 3, 1);  // 2023年3月1日是星期三
        int weekday = date.dayOfWeek();
        EXPECT_EQ(weekday, 3);  // 0代表星期天，1代表星期一，以此类推
        // 测试静态函数
        EXPECT_EQ(aDayOfWeek(date), 3);  // 0代表星期天，1代表星期一，以此类推
    }
    {
        // 测试跨月份边界情况
        Date date = Date::FromYMD(2023, 3, 31);  // 2023年3月31日是星期五
        int weekday = date.dayOfWeek();
        EXPECT_EQ(weekday, 5);  // 0代表星期天，1代表星期一，以此类推
        EXPECT_EQ(aDayOfWeek(date), 5);  // 0代表星期天，1代表星期一，以此类推
    }
    {
        // 测试跨年度边界情况
        Date date3 = Date::FromYMD(2023, 12, 31);  // 2023年12月31日是星期日
        EXPECT_EQ(date3.dayOfWeek(), 0);  // 0代表星期天，1代表星期一，以此类推
        EXPECT_EQ(aDayOfWeek(date3), 0);  // 0代表星期天，1代表星期一，以此类推
    }

    {
        // 测试跨年度边界情况
        Date date5 = Date::FromYMD(2023, 1, 0);  // 2023年1月1日是星期日
        EXPECT_EQ(date5.dayOfWeek(), 6);  // 0代表星期天，1代表星期日，以此类推
        EXPECT_EQ(aDayOfWeek(date5), 6);  // 0代表星期天，1代表星期日，以此类推
    }
}

TEST(Date, EdgeCases) 
{
    AST_USING_NAMESPACE
    
    // 测试跨年度边界情况
    Date date1;
    date1.fromYD(2023, 0);  // 2023年第0天应该是2022年最后一天
    EXPECT_EQ(date1.year(), 2022);
    EXPECT_EQ(date1.month(), 12);
    EXPECT_EQ(date1.day(), 31);
    
    // 测试跨年度边界情况
    Date date2;
    date2.fromYD(2023, 366);  // 2023年第366天应该是2024年第一天
    EXPECT_EQ(date2.year(), 2024);
    EXPECT_EQ(date2.month(), 1);
    EXPECT_EQ(date2.day(), 1);
    
    // 测试负年份（如果支持的话）
    Date date3 = Date::FromYMD(-45, 1, 1);  // 凯撒大帝出生年份
    int jd = date3.toJD();
    Date date4 = Date::FromJD(jd);
    EXPECT_EQ(date4.year(), -45);
    EXPECT_EQ(date4.month(), 1);
    EXPECT_EQ(date4.day(), 1);
}

GTEST_MAIN()