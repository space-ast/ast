/// @file      DateTimeFormat.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      3.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
 
#include "DateTime.hpp"
#include <cstdio>
#include <cstring>
#include <cassert>

AST_NAMESPACE_BEGIN

// 内部辅助函数和常量
namespace
{

    // 月份名称（英文）
    static const char* monthNamesShort[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    static const char* monthNamesLong[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    // 星期名称（英文）
    static const char* weekdayNamesShort[] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    static const char* weekdayNamesLong[] = {
        "Sunday", "Monday", "Tuesday", "Wednesday",
        "Thursday", "Friday", "Saturday"
    };

    // 将数字格式化为固定宽度，不足前面补0
    static void formatNumber(char* buf, int num, int width)
    {
        char format[16];
        if (width <= 0) {
            std::sprintf(buf, "%d", num);
        }
        else {
            std::sprintf(format, "%%0%dd", width);
            std::sprintf(buf, format, num);
        }
    }

    // 获取小数秒的小数部分
    static int getFractionalPart(double seconds, int precision)
    {
        double fractional = seconds - static_cast<int>(seconds);
        for (int i = 0; i < precision; ++i) {
            fractional *= 10;
        }
        return static_cast<int>(fractional + 0.5); // 四舍五入
    }

    // 判断是否是闰年
    static bool isLeapYear(int year)
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    // 获取月份的天数
    static int getDaysInMonth(int year, int month)
    {
        static const int daysInMonth[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        if (month < 1 || month > 12) {
            return 0;
        }

        int days = daysInMonth[month - 1];
        if (month == 2 && isLeapYear(year)) {
            days = 29;
        }

        return days;
    }

    // 计算一年中的第几天
    static int getDayOfYear(int year, int month, int day)
    {
        static const int daysToMonth[] = {
            0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
        };

        int dayOfYear = daysToMonth[month - 1] + day;
        if (month > 2 && isLeapYear(year)) {
            dayOfYear += 1;
        }

        return dayOfYear;
    }

    // 计算星期几（0=周日，1=周一，...，6=周六）
    // 使用Zeller同余公式
    static int getWeekday(int year, int month, int day)
    {
        if (month < 3) {
            month += 12;
            year -= 1;
        }

        int century = year / 100;
        int yearOfCentury = year % 100;

        int weekday = (day + (13 * (month + 1)) / 5 + yearOfCentury +
            yearOfCentury / 4 + century / 4 + 5 * century) % 7;

        // 调整结果为0=周日，1=周一，...，6=周六
        weekday = (weekday + 6) % 7;

        return weekday;
    }

    // 安全的缓冲区追加函数
    class Buffer
    {
    private:
        std::string& str_;
        size_t capacity_;

    public:
        Buffer(std::string& str, size_t initial_capacity = 256)
            : str_(str), capacity_(initial_capacity)
        {
            str_.clear();
            str_.reserve(capacity_);
        }

        void append(const char* s, size_t n)
        {
            str_.append(s, n);
        }

        void append(char c)
        {
            str_.push_back(c);
        }

        void append(const char* s)
        {
            str_.append(s);
        }

        void append(int num, int width = 0, bool zeroPad = true)
        {
            char buf[32];
            if (width > 0) {
                if (zeroPad) {
                    std::snprintf(buf, sizeof(buf), "%0*d", width, num);
                }
                else {
                    std::snprintf(buf, sizeof(buf), "%*d", width, num);
                }
            }
            else {
                std::snprintf(buf, sizeof(buf), "%d", num);
            }
            str_.append(buf);
        }
        void append(long long num, int width = 0, bool zeroPad = true)
        {
            char buf[32];
            if (width > 0) {
                if (zeroPad) {
                    std::snprintf(buf, sizeof(buf), "%0*ld", width, num);
                }
                else {
                    std::snprintf(buf, sizeof(buf), "%*ld", width, num);
                }
            }
            else {
                std::snprintf(buf, sizeof(buf), "%ld", num);
            }
            str_.append(buf);
        }
        void append(double value, int precision = 6)
        {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%.*f", precision, value);
            str_.append(buf);
        }
    };

} // 匿名命名空间

// 主格式化函数
err_t aDateTimeFormat(const DateTime& dt, StringView format, std::string& str)
{
    if (format.empty()) {
        str.clear();
        return eErrorInvalidParam;
    }

    Buffer buffer(str);
    const char* fmt = format.data();
    const char* fmt_end = fmt + format.length();

    // 从 DateTime 中提取值
    int year = dt.year();
    int month = dt.month(); // 假设 month() 返回 1-12
    int day = dt.day();     // 假设 day() 返回 1-31
    int hour = dt.hour();
    int minute = dt.minute();
    double seconds = dt.second();
    int secondInt = static_cast<int>(seconds);

    // 计算一些可能需要的信息
    int weekday = dt.dayOfWeek(); // 假设已实现
    int dayOfYear = dt.dayOfYear(); // 假设已实现

    // 如果没有 dayOfWeek() 和 dayOfYear()，计算它们
    if (weekday < 0 || weekday > 6) {
        weekday = getWeekday(year, month, day);
    }

    if (dayOfYear < 1 || dayOfYear > 366) {
        dayOfYear = getDayOfYear(year, month, day);
    }

    while (fmt < fmt_end) {
        if (*fmt != '%') {
            // 普通字符直接追加
            buffer.append(*fmt++);
            continue;
        }

        // 处理格式说明符
        ++fmt; // 跳过 '%'
        if (fmt >= fmt_end) {
            // 格式字符串以 '%' 结束，错误
            return eErrorInvalidParam;
        }

        char specifier = *fmt++;

        switch (specifier) {
            // 年份相关
        case 'Y': // 四位年份
            buffer.append(year, 4);
            break;

        case 'y': // 两位年份
            buffer.append(year % 100, 2);
            break;

        case 'C': // 世纪 (年份/100)
            buffer.append(year / 100, 2);
            break;

            // 月份相关
        case 'm': // 月份 (01-12)
            buffer.append(month, 2);
            break;

        case 'b': // 月份缩写
        case 'h':
            if (month >= 1 && month <= 12) {
                buffer.append(monthNamesShort[month - 1]);
            }
            else {
                buffer.append("???");
            }
            break;

        case 'B': // 月份全称
            if (month >= 1 && month <= 12) {
                buffer.append(monthNamesLong[month - 1]);
            }
            else {
                buffer.append("???");
            }
            break;

            // 日期相关
        case 'd': // 月中的日 (01-31)
            buffer.append(day, 2);
            break;

        case 'e': // 月中的日 ( 1-31)，单数字前加空格
            buffer.append(day, 2, false);
            break;

            // 星期相关
        case 'a': // 星期缩写
            if (weekday >= 0 && weekday <= 6) {
                buffer.append(weekdayNamesShort[weekday]);
            }
            else {
                buffer.append("???");
            }
            break;

        case 'A': // 星期全称
            if (weekday >= 0 && weekday <= 6) {
                buffer.append(weekdayNamesLong[weekday]);
            }
            else {
                buffer.append("???");
            }
            break;

        case 'w': // 星期几的数字表示 (0-6，0=周日)
            buffer.append(weekday, 1);
            break;

        case 'u': // 星期几的 ISO 表示 (1-7，1=周一)
            buffer.append((weekday == 0) ? 7 : weekday, 1);
            break;

            // 一年中的第几天
        case 'j': // 一年中的第几天 (001-366)
            buffer.append(dayOfYear, 3);
            break;

            // 时间相关
        case 'H': // 24小时制小时 (00-23)
            buffer.append(hour, 2);
            break;

        case 'I': // 12小时制小时 (01-12)
        {
            int hour12 = hour % 12;
            if (hour12 == 0) hour12 = 12;
            buffer.append(hour12, 2);
        }
        break;

        case 'M': // 分钟 (00-59)
            buffer.append(minute, 2);
            break;

        case 'S': // 秒 (00-60)
            buffer.append(secondInt, 2);
            break;

        case 'f': // 微秒 (000000-999999)
        {
            int microsecond = getFractionalPart(seconds, 6);
            buffer.append(microsecond, 6);
        }
        break;

        case 's': // Unix 时间戳
        {
            // 这里需要将日期时间转换为 Unix 时间戳
            // 简化实现：使用 mktime
            struct tm tm_time;
            tm_time.tm_year = year - 1900;
            tm_time.tm_mon = month - 1;
            tm_time.tm_mday = day;
            tm_time.tm_hour = hour;
            tm_time.tm_min = minute;
            tm_time.tm_sec = secondInt;
            tm_time.tm_isdst = -1;

            time_t timestamp = mktime(&tm_time);
            buffer.append(static_cast<long long>(timestamp), 0);
        }
        break;

        // AM/PM
        case 'p': // AM 或 PM
            buffer.append((hour >= 12) ? "PM" : "AM");
            break;

        case 'P': // am 或 pm
            buffer.append((hour >= 12) ? "pm" : "am");
            break;

            // 时区相关（简化处理）
        case 'z': // 时区偏移 +HHMM 或 -HHMM
            // 这里假设是本地时间，需要根据实际情况调整
            buffer.append("+0000");
            break;

        case 'Z': // 时区名称缩写
            buffer.append("UTC");
            break;

            // 特殊格式
        case 'c': // 日期和时间表示
        {
            char buf[64];
            std::snprintf(buf, sizeof(buf), "%s %s %02d %02d:%02d:%02d %04d",
                weekdayNamesShort[weekday],
                monthNamesShort[month - 1],
                day, hour, minute, secondInt, year);
            buffer.append(buf);
        }
        break;

        case 'x': // 日期表示
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
            buffer.append(buf);
        }
        break;

        case 'X': // 时间表示
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minute, secondInt);
            buffer.append(buf);
        }
        break;

        case 'r': // 12小时制时间
        {
            int hour12 = hour % 12;
            if (hour12 == 0) hour12 = 12;
            const char* ampm = (hour >= 12) ? "PM" : "AM";
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d %s",
                hour12, minute, secondInt, ampm);
            buffer.append(buf);
        }
        break;

        case 'R': // 24小时制的小时和分钟
        {
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%02d:%02d", hour, minute);
            buffer.append(buf);
        }
        break;

        case 'T': // ISO 8601 时间格式
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minute, secondInt);
            buffer.append(buf);
        }
        break;

        case 'F': // ISO 8601 日期格式
        {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
            buffer.append(buf);
        }
        break;

        // 特殊字符
        case 'n': // 换行符
            buffer.append('\n');
            break;

        case 't': // 制表符
            buffer.append('\t');
            break;

        case '%': // 字面百分号
            buffer.append('%');
            break;

            // 扩展格式说明符（非标准）
        case 'L': // 毫秒 (000-999)
        {
            int millisecond = getFractionalPart(seconds, 3);
            buffer.append(millisecond, 3);
        }
        break;

        case 'N': // 纳秒 (000000000-999999999)
        {
            int nanosecond = getFractionalPart(seconds, 9);
            buffer.append(nanosecond, 9);
        }
        break;

        case 'Q': // 季度 (1-4)
        {
            int quarter = (month - 1) / 3 + 1;
            buffer.append(quarter, 1);
        }
        break;

        case 'U': // 一年中的第几周（以周日为一周的开始）(00-53)
        {
            // 简化实现
            int week = dayOfYear / 7;
            buffer.append(week, 2);
        }
        break;

        case 'V': // ISO 周数 (01-53)
        {
            // 简化实现
            int week = dayOfYear / 7;
            if (week == 0) week = 1;
            buffer.append(week, 2);
        }
        break;

        case 'W': // 一年中的第几周（以周一为一周的开始）(00-53)
        {
            // 简化实现
            int week = dayOfYear / 7;
            buffer.append(week, 2);
        }
        break;

        default:
            // 未知格式说明符，按原样输出
            buffer.append('%');
            buffer.append(specifier);
            break;
        }
    }

    return eNoError;
}


AST_NAMESPACE_END