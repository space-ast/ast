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
#include "AstTest/AstTestMacro.h"
#include <stdio.h>
#include <time.h>

TEST(DateTime, BasicConstructor) 
{
    AST_USING_NAMESPACE
    
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
    dt = DateTime::FromString("25/12/2023 10:30:45", "dd/MM/yyyy HH:mm:ss");
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
    EXPECT_DOUBLE_EQ(dt2.second(), 65.5);
    
    // 测试增加秒后的规范化
    dt2.normalize();
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

GTEST_MAIN()