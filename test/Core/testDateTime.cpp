/// @file      testDateTime.cpp
/// @brief     测试日期时间类的功能
/// @details   ~
/// @author    jinke18
/// @date      03.12.2025
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


#include "AstCore/DateTime.hpp"
#include "AstCore/RunTime.hpp"
#include "AstTest/AstTestMacro.h"
#include <stdio.h>
#include <time.h>
#include <cstring>

TEST(DateTime, BasicConstructor) 
{
    AST_USING_NAMESPACE
    
    aInitialize();
    // 测试默认构造
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45.5;
    
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45.5);
    
    // 测试直接访问date_和time_成员
    EXPECT_EQ(dt.date_.year(), 2023);
    EXPECT_EQ(dt.time_.hour(), 10);
}

TEST(DateTime, Accessors) 
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.setYear(2024);
    dt.setMonth(2);
    dt.setDay(29);  // 闰年
    dt.setHour(23);
    dt.setMinute(59);
    dt.setSecond(59.999);
    
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 29);
    EXPECT_EQ(dt.hour(), 23);
    EXPECT_EQ(dt.minute(), 59);
    EXPECT_DOUBLE_EQ(dt.second(), 59.999);
    
    // 测试getter返回引用
    dt.year() = 2025;
    dt.month() = 11;
    dt.day() = 21;
    
    EXPECT_EQ(dt.year(), 2025);
    EXPECT_EQ(dt.month(), 11);
    EXPECT_EQ(dt.day(), 21);
    
    // 测试date()和time()方法
    Date& date = dt.date();
    Time& time = dt.time();
    
    date.year() = 2026;
    time.hour() = 12;
    
    EXPECT_EQ(dt.year(), 2026);
    EXPECT_EQ(dt.hour(), 12);
}

TEST(DateTime, Normalize) 
{
    AST_USING_NAMESPACE
    
    // 测试基本规范化
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 13;  // 无效月份
    dt.day() = 32;    // 无效日期
    dt.hour() = 25;   // 无效小时
    dt.minute() = 61; // 无效分钟
    dt.second() = 62; // 无效秒
    
    dt.normalize();
    
    // 预期结果: 2024-02-02 02:02:02
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 2);
    EXPECT_EQ(dt.hour(), 2);
    EXPECT_EQ(dt.minute(), 2);
    EXPECT_DOUBLE_EQ(dt.second(), 2);
    
    // 测试normalized()返回新对象
    DateTime dt2;
    dt2.year() = 2023;
    dt2.month() = 1;
    dt2.day() = 1;
    dt2.hour() = 0;
    dt2.minute() = 0;
    dt2.second() = -1; // 负秒数
    
    DateTime dt3 = dt2.normalized();
    
    // dt2不应改变
    EXPECT_EQ(dt2.year(), 2023);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.minute(), 0);
    EXPECT_DOUBLE_EQ(dt2.second(), -1);
    
    // dt3应为规范化后的结果: 2022-12-31 23:59:59
    EXPECT_EQ(dt3.year(), 2022);
    EXPECT_EQ(dt3.month(), 12);
    EXPECT_EQ(dt3.day(), 31);
    EXPECT_EQ(dt3.hour(), 23);
    EXPECT_EQ(dt3.minute(), 59);
    EXPECT_DOUBLE_EQ(dt3.second(), 59);
}

TEST(DateTime, FromString) 
{
    AST_USING_NAMESPACE
    
    // 测试默认格式的字符串解析
    DateTime dt = DateTime::FromString("2023-12-25 10:30:45");
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45);

    // 测试指定格式的字符串解析
    dt = DateTime::FromString("2025-11-21 12:34:56", "%Y-%m-%d %H:%M:%S");
    EXPECT_EQ(dt.year(), 2025);
    EXPECT_EQ(dt.month(), 11);
    EXPECT_EQ(dt.day(), 21);
    EXPECT_EQ(dt.hour(), 12);
    EXPECT_EQ(dt.minute(), 34);
    EXPECT_DOUBLE_EQ(dt.second(), 56);

    // 测试指定格式的字符串解析
    dt = DateTime::FromString("25/12/2023 10:30:45", "%d/%m/%Y %H:%M:%S");
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45);
    
    // 测试FromGregorian方法
    dt = DateTime::FromGregorian("2024-02-29 23:59:59");
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 29);
    EXPECT_EQ(dt.hour(), 23);
    EXPECT_EQ(dt.minute(), 59);
    EXPECT_DOUBLE_EQ(dt.second(), 59);
}

TEST(DateTime, CurrentDateTime) 
{
    AST_USING_NAMESPACE
    
    // 测试获取当前本地时间
    DateTime dtLocal = aCurrentDateTimeLocal();
    
    // 验证年份合理（不应是很久以前或未来）
    EXPECT_GT(dtLocal.year(), 2000);
    EXPECT_LT(dtLocal.year(), 2100);
    
    // 测试获取当前UTC时间
    DateTime dtUTC = aCurrentDateTimeUTC();
    
    // 验证年份合理
    EXPECT_GT(dtUTC.year(), 2000);
    EXPECT_LT(dtUTC.year(), 2100);
    
    // 测试引用版本的函数
    DateTime dtLocal2;
    aCurrentDateTimeLocal(dtLocal2);
    EXPECT_EQ(dtLocal.year(), dtLocal2.year());
    EXPECT_EQ(dtLocal.month(), dtLocal2.month());
    EXPECT_EQ(dtLocal.day(), dtLocal2.day());
}

TEST(DateTime, Formatting) 
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45.5;
    
    std::string str;
    
    // 测试格里高利历格式
    err_t err = aDateTimeFormatGregorian(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25") != std::string::npos);
    EXPECT_TRUE(str.find("10:30:45") != std::string::npos);
    
    // 测试格里高利历格式（英文）
    err = aDateTimeFormatGregorianEn(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("25 Dec 2023") != std::string::npos);
    
    // 测试GMT格式
    err = aDateTimeFormatGMT(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("GMT") != std::string::npos);
    
    // 测试ISO 8601格式
    err = aDateTimeFormatISO8601(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T") != std::string::npos);
    
    // 测试ISO别名
    err = aDateTimeFormatISO(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T") != std::string::npos);
    
    // 测试RFC 3339格式
    err = aDateTimeFormatRFC3339(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25") != std::string::npos);
}

TEST(DateTime, Parsing) 
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    
    // 测试解析ISO 8601格式
    err_t err = aDateTimeParseISO8601("2023-12-25T10:30:45Z", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45);
    
    // 测试解析ISO别名
    err = aDateTimeParseISO("2023-12-25T10:30:45Z", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    
    // 测试解析RFC 3339格式
    err = aDateTimeParseRFC3339("2023-12-25T10:30:45+08:00", dt);
    EXPECT_EQ(err, eNoError);
    
    // 测试解析格里高利历格式
    err = aDateTimeParseGregorian("2023-12-25 10:30:45", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    
    // 测试解析格里高利历格式（英文）
    err = aDateTimeParseGregorianEn("25 Dec 2023 10:30:45", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    
    // 测试解析GMT格式
    err = aDateTimeParseGMT("Mon, 25 Dec 2023 10:30:45 GMT", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
}

TEST(DateTime, AddFunctions) 
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    // 测试增加年
    DateTime dt2 = dt;
    aDateTimeAddYears(dt2, 1);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 12);
    EXPECT_EQ(dt2.day(), 25);
    
    // 测试增加月
    dt2 = dt;
    aDateTimeAddMonths(dt2, 2);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 2);
    EXPECT_EQ(dt2.day(), 25);
    
    // 测试增加日
    dt2 = dt;
    aDateTimeAddDays(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加小时
    dt2 = dt;
    aDateTimeAddHours(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分钟
    dt2 = dt;
    aDateTimeAddMinutes(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒
    dt2 = dt;
    aDateTimeAddSeconds(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
    EXPECT_EQ(dt2.minute(), 31);
}

TEST(DateTime, EdgeCases) 
{
    AST_USING_NAMESPACE
    
    // 测试闰年边界
    DateTime dt;
    dt.year() = 2024;
    dt.month() = 2;
    dt.day() = 29;  // 闰年2月29日
    dt.hour() = 23;
    dt.minute() = 59;
    dt.second() = 59;
    
    aDateTimeAddDays(dt, 1);
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 3);
    EXPECT_EQ(dt.day(), 1);
    
    // 测试时区枚举
    EXPECT_EQ(DateTime::eUTC, 0);
    EXPECT_EQ(DateTime::eBJT, 8);
    
    // 测试负数年份
    dt.year() = -45;
    dt.month() = 1;
    dt.day() = 1;
    
    std::string str;
    err_t err = aDateTimeFormatGregorian(dt, str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时间组件的边界值
    dt.hour() = 23;
    dt.minute() = 59;
    dt.second() = 59.999;
    dt.normalize();
    EXPECT_EQ(dt.hour(), 23);
    EXPECT_EQ(dt.minute(), 59);
    EXPECT_DOUBLE_EQ(dt.second(), 59.999);
}

TEST(DateTime, TimeConversion) 
{
    AST_USING_NAMESPACE
    
    // 测试time_t转换（这部分测试可能会因为系统时间不同而有不同结果）
    time_t now = time(nullptr);
    
    DateTime dtLocal = DateTime::FromTimeTLocal(now);
    DateTime dtUTC = DateTime::FromTimeTUTC(now);
    
    // 验证转换后的年份合理
    EXPECT_GT(dtLocal.year(), 2000);
    EXPECT_LT(dtLocal.year(), 2100);
    EXPECT_GT(dtUTC.year(), 2000);
    EXPECT_LT(dtUTC.year(), 2100);
}

TEST(DateTime, NormalizeVariants)
{
    AST_USING_NAMESPACE

    // 测试基本normalize - 不考虑闰秒，时间范围0-23:59:59
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 13;
    dt.day() = 32;
    dt.hour() = 25;
    dt.minute() = 61;
    dt.second() = 62;

    dt.normalize();
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 2);
    EXPECT_EQ(dt.hour(), 2);
    EXPECT_EQ(dt.minute(), 2);
    EXPECT_DOUBLE_EQ(dt.second(), 2);

    // 测试normalizeUTC - 考虑闰秒，时间范围0-23:59:60
    dt.year() = 2023;
    dt.month() = 13;
    dt.day() = 32;
    dt.hour() = 25;
    dt.minute() = 61;
    dt.second() = 62;

    dt.normalizeUTC();
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 2);
    EXPECT_EQ(dt.hour(), 2);
    EXPECT_EQ(dt.minute(), 2);
    EXPECT_DOUBLE_EQ(dt.second(), 2);

    // 测试normalizeUTC处理闰秒的能力
    // 注意：这里只是测试函数接口，实际闰秒处理需要具体实现支持
    dt.year() = 2023;
    dt.month() = 6;
    dt.day() = 30;
    dt.hour() = 23;
    dt.minute() = 59;
    dt.second() = 60; // 闰秒

    dt.normalizeUTC();
    // 如果系统支持闰秒，这里应该保持60秒不变
    // 否则可能会规范化为次日00:00:00

    // 测试normalizeLocal - 考虑时区和闰秒
    // 假设传入的是本地时间，函数会根据时区进行调整
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 31;
    dt.hour() = 23;
    dt.minute() = 59;
    dt.second() = 61;

    dt.normalizeLocal(8); // 东8区
    // 预期：秒数规范化，可能进位到分钟
    EXPECT_TRUE(dt.second() < 61);

    // 测试normalizeBJT - 按北京时间规范化，考虑闰秒
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 31;
    dt.hour() = 23;
    dt.minute() = 59;
    dt.second() = 61;

    dt.normalizeBJT();
    // 预期：秒数规范化，可能进位到分钟
    EXPECT_TRUE(dt.second() < 61);

    // 测试各normalize方法的返回新对象版本
    DateTime dt2;
    dt2.year() = 2023;
    dt2.month() = 1;
    dt2.day() = 1;
    dt2.hour() = 0;
    dt2.minute() = 0;
    dt2.second() = -1;

    // 测试normalizedUTC返回新对象
    DateTime dt3 = dt2.normalizedUTC();
    EXPECT_EQ(dt2.year(), 2023); // 原对象不变
    EXPECT_NE(dt3.year(), dt2.year()); // 新对象已规范化

    // 测试normalizedLocal返回新对象
    dt3 = dt2.normalizedLocal(8);
    EXPECT_EQ(dt2.year(), 2023); // 原对象不变

    // 测试normalizedBJT返回新对象
    dt3 = dt2.normalizedBJT();
    EXPECT_EQ(dt2.year(), 2023); // 原对象不变
}

TEST(DateTime, AddFunctionsUTC)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    // 测试增加日（UTC）
    DateTime dt2 = dt;
    aDateTimeAddDaysUTC(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（UTC）
    dt2 = dt;
    aDateTimeAddHoursUTC(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（UTC）
    dt2 = dt;
    aDateTimeAddMinutesUTC(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（UTC）
    dt2 = dt;
    aDateTimeAddSecondsUTC(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
    
    // 测试减少日（UTC）
    dt2 = dt;
    aDateTimeAddDaysUTC(dt2, -1);
    EXPECT_EQ(dt2.day(), 24);
}

TEST(DateTime, AddFunctionsLocal)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    // 测试增加日（本地时间，东8区）
    DateTime dt2 = dt;
    aDateTimeAddDaysLocal(dt2, 7, 8);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（本地时间，东8区）
    dt2 = dt;
    aDateTimeAddHoursLocal(dt2, 14, 8);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（本地时间，东8区）
    dt2 = dt;
    aDateTimeAddMinutesLocal(dt2, 40, 8);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（本地时间，东8区）
    dt2 = dt;
    aDateTimeAddSecondsLocal(dt2, 20.5, 8);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
}

TEST(DateTime, AddFunctionsBJT)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    // 测试增加日（北京时间）
    DateTime dt2 = dt;
    aDateTimeAddDaysBJT(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（北京时间）
    dt2 = dt;
    aDateTimeAddHoursBJT(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（北京时间）
    dt2 = dt;
    aDateTimeAddMinutesBJT(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（北京时间）
    dt2 = dt;
    aDateTimeAddSecondsBJT(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
}

TEST(DateTime, MemberAddFunctions)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    // 测试成员方法版本的addYears
    dt.addYears(1);
    EXPECT_EQ(dt.year(), 2024);
    
    // 测试成员方法版本的addMonths
    dt.addMonths(1);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.year(), 2025);
    
    // 测试成员方法版本的addDays
    dt.addDays(1);
    EXPECT_EQ(dt.day(), 26);
    
    // 测试成员方法版本的addDaysUTC
    dt.addDaysUTC(1);
    EXPECT_EQ(dt.day(), 27);
    
    // 测试成员方法版本的addDaysLocal
    dt.addDaysLocal(1, 8);
    EXPECT_EQ(dt.day(), 28);
    
    // 测试成员方法版本的addDaysBJT
    dt.addDaysBJT(1);
    EXPECT_EQ(dt.day(), 29);
    
    // 测试成员方法版本的addHours
    dt.addHours(1);
    EXPECT_EQ(dt.hour(), 11);
    
    // 测试成员方法版本的addHoursUTC
    dt.addHoursUTC(1);
    EXPECT_EQ(dt.hour(), 12);
    
    // 测试成员方法版本的addHoursLocal
    dt.addHoursLocal(1, 8);
    EXPECT_EQ(dt.hour(), 13);
    
    // 测试成员方法版本的addHoursBJT
    dt.addHoursBJT(1);
    EXPECT_EQ(dt.hour(), 14);
    
    // 测试成员方法版本的addMinutes
    dt.addMinutes(1);
    EXPECT_EQ(dt.minute(), 31);
    
    // 测试成员方法版本的addMinutesUTC
    dt.addMinutesUTC(1);
    EXPECT_EQ(dt.minute(), 32);
    
    // 测试成员方法版本的addMinutesLocal
    dt.addMinutesLocal(1, 8);
    EXPECT_EQ(dt.minute(), 33);
    
    // 测试成员方法版本的addMinutesBJT
    dt.addMinutesBJT(1);
    EXPECT_EQ(dt.minute(), 34);
    
    // 测试成员方法版本的addSeconds
    dt.addSeconds(1.5);
    EXPECT_DOUBLE_EQ(dt.second(), 46.5);
    
    // 测试成员方法版本的addSecondsUTC
    dt.addSecondsUTC(1.5);
    EXPECT_DOUBLE_EQ(dt.second(), 48.0);
    
    // 测试成员方法版本的addSecondsLocal
    dt.addSecondsLocal(1.5, 8);
    EXPECT_DOUBLE_EQ(dt.second(), 49.5);
    
    // 测试成员方法版本的addSecondsBJT
    dt.addSecondsBJT(1.5);
    EXPECT_DOUBLE_EQ(dt.second(), 51.0);
}

TEST(DateTime, AdditionalParsingFunctions)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    
    // 测试aDateTimeParse自定义格式
    err_t err = aDateTimeParse("2023-12-25 10:30:45", "%Y-%m-%d %H:%M:%S", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45);
    
    // 测试aDateTimeParseAny解析多种格式
    err = aDateTimeParseAny("2023-12-25 10:30:45", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    
    err = aDateTimeParseAny("2023-12-25T10:30:45Z", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
    
    err = aDateTimeParseAny("25 Dec 2023 10:30:45", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2023);
}

TEST(DateTime, FromTmFunction)
{
    AST_USING_NAMESPACE
    
    // 创建一个tm结构体
    tm timeStruct;
    memset(&timeStruct, 0, sizeof(tm));
    timeStruct.tm_year = 2023 - 1900; // 年从1900开始
    timeStruct.tm_mon = 11; // 月从0开始
    timeStruct.tm_mday = 25;
    timeStruct.tm_hour = 10;
    timeStruct.tm_min = 30;
    timeStruct.tm_sec = 45;
    
    // 测试FromTm函数
    DateTime dt = DateTime::FromTm(&timeStruct);
    EXPECT_EQ(dt.year(), 2023);
    EXPECT_EQ(dt.month(), 12);
    EXPECT_EQ(dt.day(), 25);
    EXPECT_EQ(dt.hour(), 10);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_DOUBLE_EQ(dt.second(), 45);
}

TEST(DateTime, DayOfYearAndDayOfWeek)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    
    // 测试1月1日
    dt.year() = 2023;
    dt.month() = 1;
    dt.day() = 1;
    EXPECT_EQ(dt.dayOfYear(), 1);
    EXPECT_EQ(dt.dayOfWeek(), 0); // 星期日
    
    // 测试2月29日（闰年）
    dt.year() = 2024;
    dt.month() = 2;
    dt.day() = 29;
    EXPECT_EQ(dt.dayOfYear(), 60);
    
    // 测试12月31日
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 31;
    EXPECT_EQ(dt.dayOfYear(), 365);
}

#ifdef AST_ENABLE_DATETIME_FORMAT_RFC
TEST(DateTime, RFCFormatting)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45;
    
    std::string str;
    
    // 测试RFC 1123格式
    err_t err = aDateTimeFormatRFC1123(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("GMT") != std::string::npos);
    
    // 测试RFC 2822格式
    err = aDateTimeFormatRFC2822(dt, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
}
#endif

GTEST_MAIN()