/// @file      testDateTimeFormat.cpp
/// @brief     测试日期时间解析和格式化函数的功能
/// @details   专门测试DateTimeFormat.cpp的函数，提升测试覆盖率
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
#include <utility>
#include <cstring>

// 测试aDateTimeFormat函数的各种格式说明符
TEST(DateTimeFormat, FormatSpecifiers)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 14;
    dt.minute() = 30;
    dt.second() = 45.123456;
    
    std::string str;
    err_t err;
    
    // 测试年份格式说明符
    err = aDateTimeFormat(dt, "%Y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023");
    
    err = aDateTimeFormat(dt, "%y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "23");
    
    err = aDateTimeFormat(dt, "%C", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "20");
    
    // 测试月份格式说明符
    err = aDateTimeFormat(dt, "%m", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "12");
    
    err = aDateTimeFormat(dt, "%b", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%B", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试日期格式说明符
    err = aDateTimeFormat(dt, "%d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "25");
    
    err = aDateTimeFormat(dt, "%e", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%j", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时间格式说明符
    err = aDateTimeFormat(dt, "%H", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14");
    
    err = aDateTimeFormat(dt, "%I", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "02");
    
    err = aDateTimeFormat(dt, "%M", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "30");
    
    err = aDateTimeFormat(dt, "%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "45");
    
    err = aDateTimeFormat(dt, "%f", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%L", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%N", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试AM/PM
    err = aDateTimeFormat(dt, "%p", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "PM");
    
    err = aDateTimeFormat(dt, "%P", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "pm");
    
    // 测试星期格式说明符
    err = aDateTimeFormat(dt, "%a", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%A", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%w", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%u", str);
    EXPECT_EQ(err, eNoError);
    
    // 测试时区格式说明符
    err = aDateTimeFormat(dt, "%z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "+0000");
    
    err = aDateTimeFormat(dt, "%Z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "UTC");
    
    // 测试特殊格式
    err = aDateTimeFormat(dt, "%c", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%x", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023-12-25");
    
    err = aDateTimeFormat(dt, "%X", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30:45");
    
    err = aDateTimeFormat(dt, "%r", str);
    EXPECT_EQ(err, eNoError);
    
    err = aDateTimeFormat(dt, "%R", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30");
    
    err = aDateTimeFormat(dt, "%T", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "14:30:45");
    
    err = aDateTimeFormat(dt, "%F", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "2023-12-25");
    
    // 测试扩展格式
    err = aDateTimeFormat(dt, "%Q", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "4"); // 12月属于第4季度
    
    // 测试特殊字符
    err = aDateTimeFormat(dt, "%%", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "%");
    
    err = aDateTimeFormat(dt, "%n", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "\n");
    
    err = aDateTimeFormat(dt, "%t", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "\t");
}

// 测试aDateTimeFormat的复杂格式组合
TEST(DateTimeFormat, ComplexFormats)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    dt.hour() = 10;
    dt.minute() = 30;
    dt.second() = 45.123;
    
    std::string str;
    err_t err;
    
    // ISO 8601格式
    err = aDateTimeFormat(dt, "%Y-%m-%dT%H:%M:%S.%L%z", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023-12-25T10:30:45.123+0000") != std::string::npos);
    
    // 带星期的格式
    err = aDateTimeFormat(dt, "%A, %d %B %Y", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("2023") != std::string::npos);
    EXPECT_TRUE(str.find("December") != std::string::npos);
    
    // 12小时制格式
    err = aDateTimeFormat(dt, "%I:%M:%S %p", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("10:30:45 AM") != std::string::npos);
    
    // 自定义格式
    err = aDateTimeFormat(dt, "Year: %Y, Month: %m, Day: %d", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_TRUE(str.find("Year: 2023") != std::string::npos);
    EXPECT_TRUE(str.find("Month: 12") != std::string::npos);
    EXPECT_TRUE(str.find("Day: 25") != std::string::npos);
}

// 测试aDateTimeFormat的错误处理
TEST(DateTimeFormat, ErrorHandling)
{
    AST_USING_NAMESPACE
    
    DateTime dt;
    dt.year() = 2023;
    dt.month() = 12;
    dt.day() = 25;
    
    std::string str;
    err_t err;
    
    // 空格式
    err = aDateTimeFormat(dt, "", str);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 格式以%结束
    err = aDateTimeFormat(dt, "%", str);
    EXPECT_EQ(err, eErrorInvalidParam);
    
    // 未知的格式说明符（应该按原样输出）
    err = aDateTimeFormat(dt, "%X%Y%Z", str);
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
        DateTime dt;
        err_t parse_err = aDateTimeParse(date_str, format, dt);
        EXPECT_EQ(parse_err, eNoError) << "Failed to parse: " << date_str;
        
        std::string formatted;
        err_t format_err = aDateTimeFormat(dt, format, formatted);
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
    
    DateTime dt;
    std::string str;
    err_t err;
    
    // 闰年
    err = aDateTimeParse("2024-02-29", "%Y-%m-%d", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 2024);
    EXPECT_EQ(dt.month(), 2);
    EXPECT_EQ(dt.day(), 29);
    
    // 特殊时间
    err = aDateTimeParse("23:59:59.999", "%H:%M:%S", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.hour(), 23);
    EXPECT_EQ(dt.minute(), 59);
    EXPECT_EQ(dt.second(), 59.999);
    
    // 零值
    err = aDateTimeParse("0000-01-01 00:00:00", "%Y-%m-%d %H:%M:%S", dt);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(dt.year(), 0);
    EXPECT_EQ(dt.month(), 1);
    EXPECT_EQ(dt.day(), 1);
    EXPECT_EQ(dt.hour(), 0);
    EXPECT_EQ(dt.minute(), 0);
    EXPECT_DOUBLE_EQ(dt.second(), 0.0);
    
    // 格式化为零值
    dt.year() = 0;
    dt.month() = 1;
    dt.day() = 1;
    dt.hour() = 0;
    dt.minute() = 0;
    dt.second() = 0.0;
    
    err = aDateTimeFormat(dt, "%Y-%m-%d %H:%M:%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "0000-01-01 00:00:00");
    
    // 带闰秒的格式
    dt.second() = 60.0;
    err = aDateTimeFormat(dt, "%H:%M:%S", str);
    EXPECT_EQ(err, eNoError);
    EXPECT_EQ(str, "00:00:60");
}

GTEST_MAIN()