/// @file      testDateTime.cpp
/// @brief     测试日期时间类的功能
/// @details   ~
/// @author    axel
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
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45.5;
    
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45.5);
    
    // 测试直接访问date_和time_成员
    EXPECT_EQ(dttm.date_.year(), 2023);
    EXPECT_EQ(dttm.time_.hour(), 10);
}

TEST(DateTime, Accessors) 
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.setYear(2024);
    dttm.setMonth(2);
    dttm.setDay(29);  // 闰年
    dttm.setHour(23);
    dttm.setMinute(59);
    dttm.setSecond(59.999);
    
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 29);
    EXPECT_EQ(dttm.hour(), 23);
    EXPECT_EQ(dttm.minute(), 59);
    EXPECT_DOUBLE_EQ(dttm.second(), 59.999);
    
    // 测试getter返回引用
    dttm.year() = 2025;
    dttm.month() = 11;
    dttm.day() = 21;
    
    EXPECT_EQ(dttm.year(), 2025);
    EXPECT_EQ(dttm.month(), 11);
    EXPECT_EQ(dttm.day(), 21);
    
    // 测试date()和time()方法
    Date& date = dttm.date();
    Time& time = dttm.time();
    
    date.year() = 2026;
    time.hour() = 12;
    
    EXPECT_EQ(dttm.year(), 2026);
    EXPECT_EQ(dttm.hour(), 12);
}

TEST(DateTime, Normalize) 
{
    AST_USING_NAMESPACE
    
    // 测试基本规范化
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 13;  // 无效月份
    dttm.day() = 32;    // 无效日期
    dttm.hour() = 25;   // 无效小时
    dttm.minute() = 61; // 无效分钟
    dttm.second() = 62; // 无效秒
    
    dttm.normalize();
    
    // 预期结果: 2024-02-02 02:02:02
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 2);
    EXPECT_EQ(dttm.hour(), 2);
    EXPECT_EQ(dttm.minute(), 2);
    EXPECT_DOUBLE_EQ(dttm.second(), 2);
    
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
    DateTime dttm = DateTime::FromString("2023-12-25 10:30:45");
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45);

    // 测试指定格式的字符串解析
    dttm = DateTime::FromString("2025-11-21 12:34:56", "%Y-%m-%d %H:%M:%S");
    EXPECT_EQ(dttm.year(), 2025);
    EXPECT_EQ(dttm.month(), 11);
    EXPECT_EQ(dttm.day(), 21);
    EXPECT_EQ(dttm.hour(), 12);
    EXPECT_EQ(dttm.minute(), 34);
    EXPECT_DOUBLE_EQ(dttm.second(), 56);

    // 测试指定格式的字符串解析
    dttm = DateTime::FromString("25/12/2023 10:30:45", "%d/%m/%Y %H:%M:%S");
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45);
    
    // 测试FromGregorian方法
    dttm = DateTime::FromGregorian("2024-02-29 23:59:59");
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 29);
    EXPECT_EQ(dttm.hour(), 23);
    EXPECT_EQ(dttm.minute(), 59);
    EXPECT_DOUBLE_EQ(dttm.second(), 59);
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
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45.5;
    
    std::string str;
    
    // 测试格里高利历格式
    err_t err = aDateTimeFormatGregorian(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25") != std::string::npos);
    EXPECT_TRUE(str.find("10:30:45") != std::string::npos);
    
    // 测试格里高利历格式（英文）
err = aDateTimeFormatGregorianEn(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("25 Dec 2023") != std::string::npos);
    
    // 测试GMT格式
    err = aDateTimeFormatGMT(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("GMT") != std::string::npos);
    
    // 测试ISO 8601格式
    err = aDateTimeFormatISO8601(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T") != std::string::npos);
    
    // 测试ISO别名
    err = aDateTimeFormatISO(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T") != std::string::npos);
    
    // 测试RFC 3339格式
    err = aDateTimeFormatRFC3339(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25") != std::string::npos);
}

TEST(DateTime, Parsing) 
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    
    // 测试解析ISO 8601格式
    err_t err = aDateTimeParseISO8601("2023-12-25T10:30:45Z", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45);
    
    // 测试解析ISO别名
    err = aDateTimeParseISO("2023-12-25T10:30:45Z", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    
    // 测试解析RFC 3339格式
    err = aDateTimeParseRFC3339("2023-12-25T10:30:45+08:00", dttm);
    EXPECT_EQ(err, eNoError);
    
    // 测试解析格里高利历格式
    err = aDateTimeParseGregorian("2023-12-25 10:30:45", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    
    // 测试解析格里高利历格式（英文）
    err = aDateTimeParseGregorianEn("25 Dec 2023 10:30:45", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    
    // 测试解析GMT格式
    err = aDateTimeParseGMT("Mon, 25 Dec 2023 10:30:45 GMT", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
}

TEST(DateTime, AddFunctions) 
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    // 测试增加年
    DateTime dt2 = dttm;
    aDateTimeAddYears(dt2, 1);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 12);
    EXPECT_EQ(dt2.day(), 25);
    
    // 测试增加月
    dt2 = dttm;
    aDateTimeAddMonths(dt2, 2);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 2);
    EXPECT_EQ(dt2.day(), 25);
    
    // 测试增加日
    dt2 = dttm;
    aDateTimeAddDays(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加小时
    dt2 = dttm;
    aDateTimeAddHours(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分钟
    dt2 = dttm;
    aDateTimeAddMinutes(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒
    dt2 = dttm;
    aDateTimeAddSeconds(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
    EXPECT_EQ(dt2.minute(), 31);
}

TEST(DateTime, EdgeCases) 
{
    AST_USING_NAMESPACE
    
    // 测试闰年边界
    DateTime dttm;
    dttm.year() = 2024;
    dttm.month() = 2;
    dttm.day() = 29;  // 闰年2月29日
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 59;
    
    aDateTimeAddDays(dttm, 1);
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 3);
    EXPECT_EQ(dttm.day(), 1);
    
    // 测试时区枚举
    EXPECT_EQ(DateTime::eUTC, 0);
    EXPECT_EQ(DateTime::eBJT, 8);
    
    // 测试负数年份
    dttm.year() = -45;
    dttm.month() = 1;
    dttm.day() = 1;
    
    std::string str;
    err_t err = aDateTimeFormatGregorian(dttm, str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时间组件的边界值
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 59.999;
    dttm.normalize();
    EXPECT_EQ(dttm.hour(), 23);
    EXPECT_EQ(dttm.minute(), 59);
    EXPECT_DOUBLE_EQ(dttm.second(), 59.999);
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
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 13;
    dttm.day() = 32;
    dttm.hour() = 25;
    dttm.minute() = 61;
    dttm.second() = 62;

    dttm.normalize();
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 2);
    EXPECT_EQ(dttm.hour(), 2);
    EXPECT_EQ(dttm.minute(), 2);
    EXPECT_DOUBLE_EQ(dttm.second(), 2);

    // 测试normalizeUTC - 考虑闰秒，时间范围0-23:59:60
    dttm.year() = 2023;
    dttm.month() = 13;
    dttm.day() = 32;
    dttm.hour() = 25;
    dttm.minute() = 61;
    dttm.second() = 62;

    dttm.normalizeUTC();
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 2);
    EXPECT_EQ(dttm.hour(), 2);
    EXPECT_EQ(dttm.minute(), 2);
    EXPECT_DOUBLE_EQ(dttm.second(), 2);

    // 测试normalizeUTC处理闰秒的能力
    // 注意：这里只是测试函数接口，实际闰秒处理需要具体实现支持
    dttm.year() = 2023;
    dttm.month() = 6;
    dttm.day() = 30;
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 60; // 闰秒

    dttm.normalizeUTC();
    // 如果系统支持闰秒，这里应该保持60秒不变
    // 否则可能会规范化为次日00:00:00

    // 测试normalizeLocal - 考虑时区和闰秒
    // 假设传入的是本地时间，函数会根据时区进行调整
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 31;
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 61;

    dttm.normalizeLocal(8); // 东8区
    // 预期：秒数规范化，可能进位到分钟
    EXPECT_TRUE(dttm.second() < 61);

    // 测试normalizeBJT - 按北京时间规范化，考虑闰秒
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 31;
    dttm.hour() = 23;
    dttm.minute() = 59;
    dttm.second() = 61;

    dttm.normalizeBJT();
    // 预期：秒数规范化，可能进位到分钟
    EXPECT_TRUE(dttm.second() < 61);

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
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    // 测试增加日（UTC）
    DateTime dt2 = dttm;
    aDateTimeAddDaysUTC(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（UTC）
    dt2 = dttm;
    aDateTimeAddHoursUTC(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（UTC）
    dt2 = dttm;
    aDateTimeAddMinutesUTC(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（UTC）
    dt2 = dttm;
    aDateTimeAddSecondsUTC(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
    
    // 测试减少日（UTC）
    dt2 = dttm;
    aDateTimeAddDaysUTC(dt2, -1);
    EXPECT_EQ(dt2.day(), 24);
}

TEST(DateTime, AddFunctionsLocal)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    // 测试增加日（本地时间，东8区）
    DateTime dt2 = dttm;
    aDateTimeAddDaysLocal(dt2, 7, 8);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（本地时间，东8区）
    dt2 = dttm;
    aDateTimeAddHoursLocal(dt2, 14, 8);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（本地时间，东8区）
    dt2 = dttm;
    aDateTimeAddMinutesLocal(dt2, 40, 8);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（本地时间，东8区）
    dt2 = dttm;
    aDateTimeAddSecondsLocal(dt2, 20.5, 8);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
}

TEST(DateTime, AddFunctionsBJT)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    // 测试增加日（北京时间）
    DateTime dt2 = dttm;
    aDateTimeAddDaysBJT(dt2, 7);
    EXPECT_EQ(dt2.year(), 2024);
    EXPECT_EQ(dt2.month(), 1);
    EXPECT_EQ(dt2.day(), 1);
    
    // 测试增加时（北京时间）
    dt2 = dttm;
    aDateTimeAddHoursBJT(dt2, 14);
    EXPECT_EQ(dt2.hour(), 0);
    EXPECT_EQ(dt2.day(), 26);
    
    // 测试增加分（北京时间）
    dt2 = dttm;
    aDateTimeAddMinutesBJT(dt2, 40);
    EXPECT_EQ(dt2.minute(), 10);
    EXPECT_EQ(dt2.hour(), 11);
    
    // 测试增加秒（北京时间）
    dt2 = dttm;
    aDateTimeAddSecondsBJT(dt2, 20.5);
    EXPECT_DOUBLE_EQ(dt2.second(), 5.5);
}

TEST(DateTime, MemberAddFunctions)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    // 测试成员方法版本的addYears
    dttm.addYears(1);
    EXPECT_EQ(dttm.year(), 2024);
    
    // 测试成员方法版本的addMonths
    dttm.addMonths(1);
    EXPECT_EQ(dttm.month(), 1);
    EXPECT_EQ(dttm.year(), 2025);
    
    // 测试成员方法版本的addDays
    dttm.addDays(1);
    EXPECT_EQ(dttm.day(), 26);
    
    // 测试成员方法版本的addDaysUTC
    dttm.addDaysUTC(1);
    EXPECT_EQ(dttm.day(), 27);
    
    // 测试成员方法版本的addDaysLocal
    dttm.addDaysLocal(1, 8);
    EXPECT_EQ(dttm.day(), 28);
    
    // 测试成员方法版本的addDaysBJT
    dttm.addDaysBJT(1);
    EXPECT_EQ(dttm.day(), 29);
    
    // 测试成员方法版本的addHours
    dttm.addHours(1);
    EXPECT_EQ(dttm.hour(), 11);
    
    // 测试成员方法版本的addHoursUTC
    dttm.addHoursUTC(1);
    EXPECT_EQ(dttm.hour(), 12);
    
    // 测试成员方法版本的addHoursLocal
    dttm.addHoursLocal(1, 8);
    EXPECT_EQ(dttm.hour(), 13);
    
    // 测试成员方法版本的addHoursBJT
    dttm.addHoursBJT(1);
    EXPECT_EQ(dttm.hour(), 14);
    
    // 测试成员方法版本的addMinutes
    dttm.addMinutes(1);
    EXPECT_EQ(dttm.minute(), 31);
    
    // 测试成员方法版本的addMinutesUTC
    dttm.addMinutesUTC(1);
    EXPECT_EQ(dttm.minute(), 32);
    
    // 测试成员方法版本的addMinutesLocal
    dttm.addMinutesLocal(1, 8);
    EXPECT_EQ(dttm.minute(), 33);
    
    // 测试成员方法版本的addMinutesBJT
    dttm.addMinutesBJT(1);
    EXPECT_EQ(dttm.minute(), 34);
    
    // 测试成员方法版本的addSeconds
    dttm.addSeconds(1.5);
    EXPECT_DOUBLE_EQ(dttm.second(), 46.5);
    
    // 测试成员方法版本的addSecondsUTC
    dttm.addSecondsUTC(1.5);
    EXPECT_DOUBLE_EQ(dttm.second(), 48.0);
    
    // 测试成员方法版本的addSecondsLocal
    dttm.addSecondsLocal(1.5, 8);
    EXPECT_DOUBLE_EQ(dttm.second(), 49.5);
    
    // 测试成员方法版本的addSecondsBJT
    dttm.addSecondsBJT(1.5);
    EXPECT_DOUBLE_EQ(dttm.second(), 51.0);
}

TEST(DateTime, AdditionalParsingFunctions)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    
    // 测试aDateTimeParse自定义格式
    err_t err = aDateTimeParse("2023-12-25 10:30:45", "%Y-%m-%d %H:%M:%S", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45);
    
    // 测试aDateTimeParseAny解析多种格式
    err = aDateTimeParseAny("2023-12-25 10:30:45", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    
    err = aDateTimeParseAny("2023-12-25T10:30:45Z", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
    
    err = aDateTimeParseAny("25 Dec 2023 10:30:45", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2023);
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
    DateTime dttm = DateTime::FromTm(&timeStruct);
    EXPECT_EQ(dttm.year(), 2023);
    EXPECT_EQ(dttm.month(), 12);
    EXPECT_EQ(dttm.day(), 25);
    EXPECT_EQ(dttm.hour(), 10);
    EXPECT_EQ(dttm.minute(), 30);
    EXPECT_DOUBLE_EQ(dttm.second(), 45);
}

TEST(DateTime, DayOfYearAndDayOfWeek)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    
    // 测试1月1日
    dttm.year() = 2023;
    dttm.month() = 1;
    dttm.day() = 1;
    EXPECT_EQ(dttm.dayOfYear(), 1);
    EXPECT_EQ(dttm.dayOfWeek(), 0); // 星期日
    
    // 测试2月29日（闰年）
    dttm.year() = 2024;
    dttm.month() = 2;
    dttm.day() = 29;
    EXPECT_EQ(dttm.dayOfYear(), 60);
    
    // 测试12月31日
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 31;
    EXPECT_EQ(dttm.dayOfYear(), 365);
}

#ifdef AST_ENABLE_DATETIME_FORMAT_RFC
TEST(DateTime, RFCFormatting)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45;
    
    std::string str;
    
    // 测试RFC 1123格式
    err_t err = aDateTimeFormatRFC1123(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("GMT") != std::string::npos);
    
    // 测试RFC 2822格式
    err = aDateTimeFormatRFC2822(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
}
#endif

// 添加新的测试用例来覆盖未测试的代码行
TEST(DateTime, UncoveredBranches) 
{ 
    AST_USING_NAMESPACE
    
    // 1. 测试aDateTimeFormatISO8601函数中millisecond=0的分支
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45.0;  // 确保毫秒为0
    
    std::string str;
    err_t err = aDateTimeFormatISO8601(dttm, str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str == "2023-12-25T10:30:45Z");
    
    // 2. 测试aDateTimeParseGregorianEn函数的错误处理分支
    err = aDateTimeParseGregorianEn("invalid date string", dttm);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 3. 测试aDateTimeParseGMT函数的错误处理分支
    err = aDateTimeParseGMT("invalid GMT string", dttm);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 4. 测试aDateTimeParseAny函数中各个分支
    // 测试RFC3339成功的情况 - 但实际上RFC3339复用了ISO8601的实现，这里需要特殊构造
    // 为了测试RFC3339的成功路径，我们需要确保RFC3339解析成功但ISO8601解析失败
    // 由于当前实现中RFC3339直接调用ISO8601，这部分测试可能无法完全分离
    
    // 测试GMT解析成功的情况
    // 先确保其他格式都失败，只让GMT格式成功
    // 这里我们使用一个只有GMT格式能解析的特殊格式
    DateTime dtGMT;
    std::string gmtStr = "Sun, 01 Jan 2023 00:00:00 GMT";
    
    // 先测试这个字符串用其他格式解析会失败
    EXPECT_NE(aDateTimeParseISO8601(gmtStr, dtGMT), eNoError);
    EXPECT_NE(aDateTimeParseGregorian(gmtStr, dtGMT), eNoError);
    EXPECT_NE(aDateTimeParseGregorianEn(gmtStr, dtGMT), eNoError);
    
    // 然后测试aDateTimeParseAny可以正确解析它
    err = aDateTimeParseAny(gmtStr, dtGMT);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dtGMT.year(), 2023);
    EXPECT_EQ(dtGMT.month(), 1);
    EXPECT_EQ(dtGMT.day(), 1);
    
    // 测试aDateTimeParseAny返回错误的情况
    err = aDateTimeParseAny("completely invalid date string", dttm);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 5. 测试DateTime::FromString函数的错误处理（有格式参数）
    // 这个函数在解析失败时会调用aError，但我们无法直接验证aError是否被调用
    // 我们只能测试它能处理错误输入但仍返回一个DateTime对象
    DateTime dtFromStringErr = DateTime::FromString("invalid date", "%Y-%m-%d");
    // 只要不崩溃即可，无法验证错误日志
    
    // 6. 测试DateTime::FromString函数的错误处理（无格式参数）
    DateTime dtFromStringNoFormatErr = DateTime::FromString("invalid date");
    // 只要不崩溃即可，无法验证错误日志
    
    // 7. 测试DateTime::FromGregorian函数的错误处理
    DateTime dtFromGregErr = DateTime::FromGregorian("invalid gregorian date");
    // 函数应该返回当前时间，我们只能验证返回的对象是有效的
    EXPECT_GT(dtFromGregErr.year(), 2000);
    EXPECT_LT(dtFromGregErr.year(), 2100);
}

// 8. 测试时区相关的未覆盖分支
TEST(DateTime, TimeZoneRelatedBranches) 
{ 
    AST_USING_NAMESPACE
    
    // 为了测试_aGregDateDifference和_aTimeZoneDateDifference函数中的未覆盖分支
    // 我们需要通过调用使用这些内部函数的公共API来间接测试
    
    DateTime dt1, dt2;
    dt1.year() = 2023;
    dt1.month() = 1;
    dt1.day() = 1;
    dt1.hour() = 12;
    dt1.minute() = 0;
    dt1.second() = 0;
    
    dt2.year() = 2023;
    dt2.month() = 1;
    dt2.day() = 1;
    dt2.hour() = 12;
    dt2.minute() = 0;
    dt2.second() = 0;
    
    // 测试时区相关函数 - 由于这些是内部函数，我们通过调用公共API间接测试
    // 例如，通过测试不同时区的日期时间转换来触发这些分支
    
    // 测试不同时区的日期时间计算
    aDateTimeAddDaysLocal(dt2, 1, 0);   // timeZone=0，测试第120行分支
    aDateTimeAddDaysLocal(dt2, 1, -8);  // timeZone<0，测试第131-132行和第135行分支
    
    // 测试可能触发h<0情况的时区计算（第148行）
    DateTime dt3;
    dt3.year() = 2023;
    dt3.month() = 1;
    dt3.day() = 1;
    dt3.hour() = 0;
    dt3.minute() = 0;
    dt3.second() = 0;
    
    aDateTimeAddHoursLocal(dt3, -1, 0);  // 尝试让小时变为负数，触发相关分支
}

GTEST_MAIN()