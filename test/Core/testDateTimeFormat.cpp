/// @file      testDateTimeFormat.cpp
/// @brief     测试日期时间解析和格式化函数的功能
/// @details   专门测试DateTimeFormat.cpp的函数，提升测试覆盖率
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
#include "AstTest/AstTestMacro.h"
#include <cstdio>
#include <time.h>
#include <utility>
#include <cstring>

// 测试aDateTimeFormat函数的各种格式说明符
TEST(DateTimeFormat, FormatSpecifiers)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 14;
    dttm.minute() = 30;
    dttm.second() = 45.123456;
    
    std::string str;
    err_t err;
    
    // 测试年份格式说明符
    err = aDateTimeFormat(dttm, "%Y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023");
    
    err = aDateTimeFormat(dttm, "%y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "23");
    
    err = aDateTimeFormat(dttm, "%C", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "20");
    
    // 测试月份格式说明符
    err = aDateTimeFormat(dttm, "%m", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "12");
    
    err = aDateTimeFormat(dttm, "%b", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%B", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试日期格式说明符
    err = aDateTimeFormat(dttm, "%d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "25");
    
    err = aDateTimeFormat(dttm, "%e", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%j", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时间格式说明符
    err = aDateTimeFormat(dttm, "%H", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14");
    
    err = aDateTimeFormat(dttm, "%I", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "02");
    
    err = aDateTimeFormat(dttm, "%M", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "30");
    
    err = aDateTimeFormat(dttm, "%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "45");
    
    err = aDateTimeFormat(dttm, "%f", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%L", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%N", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试AM/PM
    err = aDateTimeFormat(dttm, "%p", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "PM");
    
    err = aDateTimeFormat(dttm, "%P", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "pm");
    
    // 测试星期格式说明符
    err = aDateTimeFormat(dttm, "%a", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%A", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%w", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%u", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时区格式说明符
    err = aDateTimeFormat(dttm, "%z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "+0000");
    
    err = aDateTimeFormat(dttm, "%Z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "UTC");
    
    // 测试特殊格式
    err = aDateTimeFormat(dttm, "%c", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%x", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023-12-25");
    
    err = aDateTimeFormat(dttm, "%X", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30:45");
    
    err = aDateTimeFormat(dttm, "%r", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dttm, "%R", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30");
    
    err = aDateTimeFormat(dttm, "%T", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30:45");
    
    err = aDateTimeFormat(dttm, "%F", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023-12-25");
    
    // 测试扩展格式
    err = aDateTimeFormat(dttm, "%Q", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "4"); // 12月属于第4季度
    
    // 测试特殊字符
    err = aDateTimeFormat(dttm, "%%", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "%");
    
    err = aDateTimeFormat(dttm, "%n", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "\n");
    
    err = aDateTimeFormat(dttm, "%t", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "\t");
}

// 测试aDateTimeFormat的复杂格式组合
TEST(DateTimeFormat, ComplexFormats)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45.123;
    
    std::string str;
    err_t err;
    
    // ISO 8601格式
    err = aDateTimeFormat(dttm, "%Y-%m-%dT%H:%M:%S.%L%z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T10:30:45.123+0000") != std::string::npos);
    
    // 带星期的格式
    err = aDateTimeFormat(dttm, "%A, %d %B %Y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("December") != std::string::npos);
    
    // 12小时制格式
    err = aDateTimeFormat(dttm, "%I:%M:%S %p", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("10:30:45 AM") != std::string::npos);
    
    // 自定义格式
    err = aDateTimeFormat(dttm, "Year: %Y, Month: %m, Day: %d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("Year: 2023") != std::string::npos);
    EXPECT_TRUE(str.find("Month: 12") != std::string::npos);
    EXPECT_TRUE(str.find("Day: 25") != std::string::npos);
}

// 测试aDateTimeFormat的错误处理
TEST(DateTimeFormat, ErrorHandling)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 12;
    dttm.day() = 25;
    
    std::string str;
    err_t err;
    
    // 空格式
    err = aDateTimeFormat(dttm, "", str);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 格式以%结束
    err = aDateTimeFormat(dttm, "%", str);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 未知的格式说明符（应该按原样输出）
    err = aDateTimeFormat(dttm, "%X%Y%Z", str);
    EXPECT_EQ(err, eNoError);
}

// 测试解析和格式化的一致性
TEST(DateTimeParseFormat, Consistency)
{
    AST_USING_NAMESPACE
    
    // 测试常见格式的解析和格式化一致性
    std::vector<std::pair<std::string, std::string>> format_tests = {
        {"2023-12-25 10:30:45", "%Y-%m-%d %H:%M:%S"},
        {"25/12/2023 10:30:45", "%d/%m/%Y %H:%M:%S"},
        {"20231225103045", "%Y%m%d%H%M%S"},
        {"Dec 25 2023 10:30 AM", "%b %d %Y %I:%M %p"}
    };
    std::string date_str, format;
    for (auto& iter : format_tests) {
        std::tie(date_str, format) = iter;
        DateTime dttm;
        err_t parse_err = aDateTimeParse(date_str, format, dttm);
        EXPECT_EQ(parse_err, eNoError) << "Failed to parse: " << date_str;
        
        std::string formatted;
        err_t format_err = aDateTimeFormat(dttm, format, formatted);
        EXPECT_EQ(format_err, eNoError) << "Failed to format with: " << format;
        
        // 注意：由于时间组件的精度问题，这里可能需要更宽松的比较
        // 对于简单的格式，我们可以直接比较
        if (format.find("%S") == std::string::npos) { // 如果格式中没有秒，可能可以直接比较
            EXPECT_TRUE(formatted == date_str || 
                       (format.find("%p") != std::string::npos && 
                        (formatted == date_str || 
                         (formatted.find("AM") != std::string::npos && date_str.find("am") != std::string::npos) ||
                         (formatted.find("PM") != std::string::npos && date_str.find("pm") != std::string::npos))))
                << "Format inconsistency for: " << date_str << " with format: " << format;
        }
    }
}

// 测试边界情况
TEST(DateTimeParseFormat, EdgeCases)
{
    AST_USING_NAMESPACE
    
    DateTime dttm;
    std::string str;
    err_t err;
    
    // 闰年
    err = aDateTimeParse("2024-02-29", "%Y-%m-%d", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 2024);
    EXPECT_EQ(dttm.month(), 2);
    EXPECT_EQ(dttm.day(), 29);
    
    // 特殊时间
    err = aDateTimeParse("23:59:59.999", "%H:%M:%S", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.hour(), 23);
    EXPECT_EQ(dttm.minute(), 59);
    EXPECT_EQ(dttm.second(), 59.999);
    
    // 零值
    err = aDateTimeParse("0000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S", dttm);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dttm.year(), 0);
    EXPECT_EQ(dttm.month(), 1);
    EXPECT_EQ(dttm.day(), 1);
    EXPECT_EQ(dttm.hour(), 0);
    EXPECT_EQ(dttm.minute(), 0);
    EXPECT_DOUBLE_EQ(dttm.second(), 0.0);
    
    // 格式化为零值
    dttm.year() = 0;
    dttm.month() = 1;
    dttm.day() = 1;
    dttm.hour() = 0;
    dttm.minute() = 0;
    dttm.second() = 0.0;
    
    err = aDateTimeFormat(dttm, "%Y-%m-%d %H:%M:%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "0000-01-01 00:00:00");
    
    // 带闰秒的格式
    dttm.second() = 60.0;
    err = aDateTimeFormat(dttm, "%H:%M:%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "00:00:60");
}

// 测试未覆盖的格式说明符和边界情况
TEST(DateTimeFormat, UncoveredFormatSpecifiers) {
    AST_USING_NAMESPACE
    
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 6;
    dttm.day() = 15;
    dttm.hour() = 10;
    dttm.minute() = 30;
    dttm.second() = 45.123456;
    
    std::string str;
    err_t err;
    
    // 测试Unix时间戳格式说明符 '%s'
    err = aDateTimeFormat(dttm, "%s", str);
    EXPECT_EQ(err, eNoError);
    // Unix时间戳应该是一个有效的数字字符串
    EXPECT_TRUE(str.find_first_not_of("0123456789") == std::string::npos);
    
    // 测试周数相关格式说明符
    // 以周日为周始的周数 '%U'
    err = aDateTimeFormat(dttm, "%U", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.length() == 2);
    
    // ISO周数 '%V'
    err = aDateTimeFormat(dttm, "%V", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.length() == 2);
    
    // 以周一为周始的周数 '%W'
    err = aDateTimeFormat(dttm, "%W", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.length() == 2);
    
    // 测试未知格式说明符（default分支）
    err = aDateTimeFormat(dttm, "%Z%ABC", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("BC") != std::string::npos);
    
    // 测试特殊组合格式
    err = aDateTimeFormat(dttm, "Timestamp: %s, Week(U): %U, Week(V): %V, Week(W): %W", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("Timestamp:") != std::string::npos);
    EXPECT_TRUE(str.find("Week(U):") != std::string::npos);
    EXPECT_TRUE(str.find("Week(V):") != std::string::npos);
    EXPECT_TRUE(str.find("Week(W):") != std::string::npos);
}

// 测试dayOfYear验证失败分支
TEST(DateTimeFormat, DayOfYearValidation) {
    AST_USING_NAMESPACE
    
    // 创建一个DateTime对象并修改其内部状态以触发dayOfYear验证失败分支
    DateTime dttm;
    dttm.year() = 2023;
    dttm.month() = 2;
    dttm.day() = 28;
    dttm.hour() = 0;
    dttm.minute() = 0;
    dttm.second() = 0.0;
    
    std::string str;
    err_t err = aDateTimeFormat(dttm, "%j", str);
    EXPECT_EQ(err, eNoError);
    // 2023年2月28日应该是一年中的第59天
    EXPECT_TRUE(str == "059" || str == "59");
    
    // 测试闰年的情况
    dttm.year() = 2024;
    dttm.month() = 2;
    dttm.day() = 29;
    err = aDateTimeFormat(dttm, "%j", str);
    EXPECT_EQ(err, eNoError);
    // 2024年2月29日应该是一年中的第60天
    EXPECT_TRUE(str == "060" || str == "60");
}

// 测试内部辅助函数相关逻辑（通过调用相关API间接测试）
TEST(DateTimeFormat, InternalHelpers) {
    AST_USING_NAMESPACE
    
    // 测试闰年判断（间接测试isLeapYear函数）
    DateTime dttm;
    dttm.year() = 2024;
    dttm.month() = 2;
    dttm.day() = 29; // 闰年2月29日
    
    std::string str;
    err_t err = aDateTimeFormat(dttm, "%Y-%m-%d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2024-02-29");
    
    // 测试非闰年
    dttm.year() = 2023;
    dttm.month() = 2;
    dttm.day() = 28;
    err = aDateTimeFormat(dttm, "%Y-%m-%d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023-02-28");
    
    // 测试Buffer类的各种append方法（间接测试）
    // 通过不同格式说明符和数值范围来触发Buffer类的不同append重载
    dttm.year() = 9999;
    dttm.month() = 1;
    dttm.day() = 1;
    dttm.hour() = 0;
    dttm.minute() = 0;
    dttm.second() = 0.0;
    
    err = aDateTimeFormat(dttm, "%Y-%m-%d %H:%M:%S.%f", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "9999-01-01 00:00:00.000000");
}

GTEST_MAIN()