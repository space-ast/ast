/// @file      DateTimeParse.cpp
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
#include <cctype>
#include <cstring>


AST_NAMESPACE_BEGIN

// 内部辅助函数
namespace
{

    // 跳过空格
    static inline void skipSpaces(const char*& s)
    {
        while (*s && isspace(static_cast<unsigned char>(*s))) {
            ++s;
        }
    }

    // 解析数字
    static bool parseNumber(const char*& s, int min, int max, int& value, int width = 0)
    {
        if (!isdigit(static_cast<unsigned char>(*s))) {
            return false;
        }

        value = 0;
        int digits = 0;

        while (*s && isdigit(static_cast<unsigned char>(*s)) && (width == 0 || digits < width)) {
            value = value * 10 + (*s - '0');
            ++s;
            ++digits;
        }

        return (value >= min && value <= max);
    }

    // 解析小数秒
    static bool parseFractionalSeconds(const char*& s, double& seconds)
    {
        if (*s != '.') {
            return true; // 可选的小数部分
        }

        ++s; // 跳过 '.'

        if (!isdigit(static_cast<unsigned char>(*s))) {
            return false;
        }

        double fraction = 0.0;
        double divisor = 10.0;

        while (*s && isdigit(static_cast<unsigned char>(*s))) {
            fraction += (*s - '0') / divisor;
            divisor *= 10.0;
            ++s;
        }

        seconds += fraction;
        return true;
    }

    // 月份名称映射
    struct MonthName
    {
        const char* shortName;
        const char* longName;
        int month; // 1-12
    };

    static const MonthName monthNames[] = {
        {"Jan", "January", 1},
        {"Feb", "February", 2},
        {"Mar", "March", 3},
        {"Apr", "April", 4},
        {"May", "May", 5},
        {"Jun", "June", 6},
        {"Jul", "July", 7},
        {"Aug", "August", 8},
        {"Sep", "September", 9},
        {"Oct", "October", 10},
        {"Nov", "November", 11},
        {"Dec", "December", 12},
        {nullptr, nullptr, 0}
    };

    // 星期名称映射
    struct WeekdayName
    {
        const char* shortName;
        const char* longName;
        int weekday; // 0-6, Sunday=0
    };

    static const WeekdayName weekdayNames[] = {
        {"Sun", "Sunday", 0},
        {"Mon", "Monday", 1},
        {"Tue", "Tuesday", 2},
        {"Wed", "Wednesday", 3},
        {"Thu", "Thursday", 4},
        {"Fri", "Friday", 5},
        {"Sat", "Saturday", 6},
        {nullptr, nullptr, 0}
    };

    // 解析月份名称
    static bool parseMonthName(const char*& s, int& month, bool longForm)
    {
        // 保存起始位置用于错误恢复
        const char* start = s;

        for (const MonthName* mn = monthNames; mn->shortName; ++mn) {
            const char* name = longForm ? mn->longName : mn->shortName;
            const char* p = s;
            const char* n = name;

            while (*n && *p && tolower(static_cast<unsigned char>(*p)) == tolower(static_cast<unsigned char>(*n))) {
                ++p;
                ++n;
            }

            if (*n == '\0') {
                // 完全匹配
                month = mn->month;
                s = p;
                return true;
            }
        }

        // 没有匹配
        s = start;
        return false;
    }

    // 解析星期名称
    static bool parseWeekdayName(const char*& s, int& weekday, bool longForm)
    {
        const char* start = s;

        for (const WeekdayName* wn = weekdayNames; wn->shortName; ++wn) {
            const char* name = longForm ? wn->longName : wn->shortName;
            const char* p = s;
            const char* n = name;

            while (*n && *p && tolower(static_cast<unsigned char>(*p)) == tolower(static_cast<unsigned char>(*n))) {
                ++p;
                ++n;
            }

            if (*n == '\0') {
                weekday = wn->weekday;
                s = p;
                return true;
            }
        }

        s = start;
        return false;
    }

    // 解析AM/PM
    static bool parseAmPm(const char*& s, bool& isPm)
    {
        if ((s[0] == 'A' || s[0] == 'a') && (s[1] == 'M' || s[1] == 'm')) {
            isPm = false;
            s += 2;
            return true;
        }

        if ((s[0] == 'P' || s[0] == 'p') && (s[1] == 'M' || s[1] == 'm')) {
            isPm = true;
            s += 2;
            return true;
        }

        return false;
    }

    // 解析时区偏移
    static bool parseTimezoneOffset(const char*& s, int& hourOffset, int& minuteOffset)
    {
        const char* start = s;

        if (*s != '+' && *s != '-') {
            return false;
        }

        bool negative = (*s == '-');
        ++s;

        // 解析小时
        if (!isdigit(static_cast<unsigned char>(s[0])) || !isdigit(static_cast<unsigned char>(s[1]))) {
            s = start;
            return false;
        }

        hourOffset = (s[0] - '0') * 10 + (s[1] - '0');
        s += 2;

        // 解析分钟（可选）
        minuteOffset = 0;
        if (*s == ':') {
            ++s;
            if (!isdigit(static_cast<unsigned char>(s[0])) || !isdigit(static_cast<unsigned char>(s[1]))) {
                s = start;
                return false;
            }
            minuteOffset = (s[0] - '0') * 10 + (s[1] - '0');
            s += 2;
        }

        if (negative) {
            hourOffset = -hourOffset;
            minuteOffset = -minuteOffset;
        }

        return true;
    }

} // 匿名命名空间

// 主解析函数
err_t aDateTimeParse(StringView str, StringView format, DateTime& dttm, bool strict)
{
    if (str.empty() || format.empty()) {
        return eErrorInvalidParam;
    }

    const char* s = str.data();
    const char* send = s + str.length();
    const char* f = format.data();
    const char* fend = f + format.length();

    // 临时存储解析的值


    struct ParserState
    {
        bool yearSet = false, monthSet = false, daySet = false;
        bool hourSet = false, minuteSet = false, secondSet = false;
        bool dayofYearSet = false;
        bool isPm = false;
        bool hasHour12 = false;
        bool hasTimezone = false;

        int year = 0, month = 0, day = 0;
        int hour = 0, minute = 0;
        int weekday = -1; // -1 表示未解析
        int dayOfYear = 0;
        int tzHourOffset = 0, tzMinuteOffset = 0;
        
        double second = 0.0;
    } state;


    while (f < fend && s < send) {
        // 跳过格式字符串中的空格
        // skipSpaces(f);
        // if (f >= fend) break;

        // 跳过输入字符串中的空格（但只有在格式字符串有空格时才跳过）
        if (isspace(static_cast<unsigned char>(*f))) {
            skipSpaces(s);
            ++f; // 跳过格式字符串中的空格
            continue;
        }

        if (*f != '%') {
            // 字面字符匹配
            if (*s != *f) {
                return eErrorParse;
            }
            ++s;
            ++f;
            continue;
        }

        // 处理格式说明符
        ++f; // 跳过 '%'
        if (f >= fend) {
            return eErrorInvalidParam;
        }

        char specifier = *f++;

        switch (specifier) {
        case 'Y': // 四位年份
            if (!parseNumber(s, strict?0:-9999, 9999, state.year, 4)) {
                return eErrorParse;
            }
            state.yearSet = true;
            break;

        case 'y': // 两位年份
        {
            int shortYear;
            if (!parseNumber(s, strict?0:-99, 99, shortYear, 2)) {
                return eErrorParse;
            }
            // 按照 glibc 规则：00-68 -> 2000-2068, 69-99 -> 1969-1999
            state.year = (shortYear < 69) ? shortYear + 100 : shortYear;
            // tm_year 是从 1900 开始的，所以我们存储相对 1900 的偏移
            state.year += 1900;
        }
        state.yearSet = true;
        break;

        case 'm': // 月份 (01-12)
            if (!parseNumber(s, strict?1:-99, strict?12:99, state.month, 2)) {
                return eErrorParse;
            }
            state.monthSet = true;
            break;

        case 'd': // 日 (01-31)
            if (!parseNumber(s, strict?1:-99, strict?31:99, state.day, 2)) {
                return eErrorParse;
            }
            state.daySet = true;
            break;

        case 'H': // 24小时制小时 (00-23)
            if (!parseNumber(s, strict?0:-99, strict?23:99, state.hour, 2)) {
                return eErrorParse;
            }
            state.hourSet = true;
            state.hasHour12 = false;
            break;

        case 'I': // 12小时制小时 (01-12)
        {
            int hour12;
            if (!parseNumber(s, strict?1:-99, strict?12:99, hour12, 2)) {
                return eErrorParse;
            }
            state.hour = hour12;
            state.hourSet = true;
            state.hasHour12 = true;
        }
        break;

        case 'M': // 分钟 (00-59)
            if (!parseNumber(s, strict?0:-99, strict?59:99, state.minute, 2)) {
                return eErrorParse;
            }
            state.minuteSet = true;
            break;

        case 'S': // 秒 (00-60, 60是闰秒)
        {
            int secInt;
            if (!parseNumber(s, strict?0:-99, strict?60:99, secInt, 2)) {
                return eErrorParse;
            }
            state.second = static_cast<double>(secInt);
            if (!parseFractionalSeconds(s, state.second)) {
                return eErrorParse;
            }
            state.secondSet = true;
        }
        break;

        case 'p': // AM/PM
            if (!parseAmPm(s, state.isPm)) {
                return eErrorParse;
            }
            break;

        case 'b': // 月份缩写
        case 'h':
            if (!parseMonthName(s, state.month, false)) {
                return eErrorParse;
            }
            state.monthSet = true;
            break;

        case 'B': // 月份全称
            if (!parseMonthName(s, state.month, true)) {
                return eErrorParse;
            }
            state.monthSet = true;
            break;

        case 'a': // 星期缩写
            if (!parseWeekdayName(s, state.weekday, false)) {
                return eErrorParse;
            }
            break;

        case 'A': // 星期全称
            if (!parseWeekdayName(s, state.weekday, true)) {
                return eErrorParse;
            }
            break;

        case 'w': // 星期数字 (0-6, 星期日=0)
        {
            int w;
            if (!parseNumber(s, 0, 6, w, 1)) {
                return eErrorParse;
            }
            state.weekday = w;
        }
        break;

        case 'j': // 一年中的第几天 (001-366)
        {
            if (!parseNumber(s, strict?1:-999, strict?366:999, state.dayOfYear, 3)) {
                return eErrorParse;
            }
            state.dayofYearSet = true;
        }
        break;

        case 'z': // 时区偏移 +HHMM 或 -HHMM
        {
            const char* saved = s;
            if (parseTimezoneOffset(s, state.tzHourOffset, state.tzMinuteOffset)) {
                state.hasTimezone = true;
            }
            else {
                s = saved; // 回溯
                return eErrorParse;
            }
        }
        break;

        case 'Z': // 时区名称 (如 GMT, UTC)
            // 简单跳过时区名称
            while (s < send && isalpha(static_cast<unsigned char>(*s))) {
                ++s;
            }
            break;

        case 't': // 制表符或空格
        case 'n': // 换行符
            skipSpaces(s);
            break;

        case '%': // 字面百分号
            if (*s != '%') {
                return eErrorParse;
            }
            ++s;
            break;

        default:
            // 未知格式说明符
            return eErrorInvalidParam;
        }
    }

    // 检查是否还有未处理的格式字符串
    if (f < fend) {
        skipSpaces(f);
        if (f < fend) {
            return eErrorParse;
        }
    }

    // 后处理
    if (state.hasHour12) {
        // 处理 12 小时制转换
        if (state.isPm) {
            if (state.hour != 12) {
                state.hour += 12;
            }
        }
        else {
            if (state.hour == 12) {
                state.hour = 0;
            }
        }
    }

    dttm = DateTime{};

    // 设置日期时间对象
    if (state.yearSet) {
        dttm.setYear(state.year);
    }
    else {
        // 默认年份（如果需要的话）
        // 在实际应用中，可能需要设置默认值
    }

    if (!state.monthSet || !state.daySet)
    {
        if (state.dayofYearSet && state.yearSet) {
            Date date;
            aYDToDate(state.year, state.dayOfYear, date);
            state.month = date.month();
            state.monthSet = true;
            state.day = date.day();
            state.daySet = true;
        }
    }

    if (state.monthSet) {
        dttm.setMonth(state.month);
    }
    else {
        dttm.setMonth(1); // 默认一月
    }

    if (state.daySet) {
        dttm.setDay(state.day);
    }
    else {
        dttm.setDay(1); // 默认第一天
    }



    if (state.hourSet) {
        dttm.setHour(state.hour);
    }

    if (state.minuteSet) {
        dttm.setMinute(state.minute);
    }

    if (state.secondSet) {
        dttm.setSecond(state.second);
    }

    // 如果有时区信息，可能需要调整
    if (state.hasTimezone) {
        // 根据时区偏移调整时间
        // 注意：这里需要知道原始时间是 UTC 还是本地时间
        // 暂时不实现时区转换，只是保存偏移信息
    }

    // @todo 验证日期有效性
    // dttm.normalize();

    return eNoError;
}


err_t aDateTimeParse(StringView str, StringView format, DateTime& dttm)
{
    return aDateTimeParse(str, format, dttm, false);
}
AST_NAMESPACE_END
 
