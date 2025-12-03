/// @file      DateTime.cpp
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
#include "time.h"
#include "AstCore/RunTime.hpp"


AST_NAMESPACE_BEGIN


DateTime DateTime::FromTm(const tm* time)
{
    int year = time->tm_year + 1900;
    int month = time->tm_mon + 1;
    int day = time->tm_mday;
    int hour = time->tm_hour;
    int minute = time->tm_min;
    double second = static_cast<double>(time->tm_sec);
    return DateTime{year, month, day, hour, minute, second};
}

DateTime DateTime::FromTimeTLocal(time_t time)
{
    return FromTm(localtime(&time));
}

DateTime DateTime::FromTimeTUTC(time_t time)
{
    return FromTm(gmtime(&time));
}

void aCurrentDateTimeLocal(DateTime& dt)
{
    time_t t = time(NULL);
	dt = DateTime::FromTimeTLocal(t);
}

void aCurrentDateTimeUTC(DateTime &dt)
{
    time_t t = time(NULL);
	dt = DateTime::FromTimeTUTC(t);
}

void aDateTimeNormalize(DateTime& dt)
{
    aTimeNormalize(dt.time());
    int rday = (int)floor(dt.hour() / 24.);
    dt.hour() -= rday * 24;
    dt.day() += rday;
    aDateNormalize(dt.date());
}


void aDateTimeNormalizeUTC(DateTime& dt)
{
    using std::min;
    using std::max;

    int minute = dt.time().hour() * 60 + dt.minute();
    double second = dt.time().second();
    int dday = (second + minute * 60) / 86400.;
    if (dday == 0) {
        aDateTimeNormalize(dt);
        return;
    }
    else {
        int mjd1 = aDateToMJD(dt.date());
        double leap1 = aLeapSecondUTCMJD(mjd1);

        int minute_new;
        double second_new;
        int mjd2;
        for (;;) {
            mjd2 = mjd1 + dday;
            double leap2 = aLeapSecondUTCMJD(mjd2);
            minute_new = minute - dday * 1440;
            second_new = second + (leap1 - leap2);
            if (second_new + minute_new * 60 >= 0)
            {
                break;
            }
            else {
                dday -= 1;
            }
        }
        aMJDToDate(mjd2, dt.date());
        minute = min(max(minute_new, 0), 1440 - 1);
        second = second_new + (minute_new - minute) * 60;
        dt.time().hour() = minute / 60;
        dt.time().minute() = minute - dt.time().hour() * 60;
        dt.time().second() = second;
    }
}

// @brief  计算格林尼治世界时相对当前时区的日期修正量(+1/0/-1)
int _aGregDateDifference(int hour, int timeZone) {
		if (timeZone == 0) {
			return 0;
		}
		else if (timeZone > 0) {
			if (hour >= timeZone) {
				return 0;
			}
			else {
				return -1;
			}
		}
		else {
			if (hour >= 24 + timeZone) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}


/// @brief  计算当前时区相对格林尼治世界时的日期修正量(+1/0/-1)
int _aTimeZoneDateDifference(int hour, int timeZone) {
	int h = hour + timeZone;
	if (h >= 24) {
		return 1;
	}
	else if (h < 0) {
		return -1;
	}
	else {
		return 0;
	}
}
    
void aDateTimeNormalizeLocal(DateTime& dt, int timezone)
{
    int dateDiff = _aGregDateDifference(dt.hour(), timezone);
	dt.hour() -= (timezone + dateDiff * 24);
    dt.day() += dateDiff;

    aDateTimeNormalizeUTC(dt);

    dateDiff = _aTimeZoneDateDifference(dt.hour(), timezone);
	dt.day() += dateDiff;
	dt.hour() += (timezone - dateDiff * 24);
    dt.date().normalize();
}

void aDateTimeNormalizeBJT(DateTime& dt)
{
    aDateTimeNormalizeLocal(dt, 8);
}

void aDateTimeAddYears(DateTime& dt, int years)
{
    dt.year() += years;
    dt.normalize();
}
void aDateTimeAddMonths(DateTime& dt, int months)
{
    dt.month() += months;
    dt.normalize();
}
void aDateTimeAddDays(DateTime& dt, int days)
{
    dt.day() += days;
    dt.normalize();
}
void aDateTimeAddDaysUTC(DateTime& dt, int days)
{
    dt.hour() += days * 24;
    dt.normalizeUTC();
}
void aDateTimeAddDaysLocal(DateTime& dt, int days, int timezone)
{
    dt.hour() += days * 24;
    dt.normalizeLocal(timezone);
}
void aDateTimeAddDaysBJT(DateTime& dt, int days)
{
    dt.hour() += days * 24;
    dt.normalizeBJT();
}
void aDateTimeAddHours(DateTime& dt, int hours)
{
    dt.hour() += hours;
    dt.normalize();
}

void aDateTimeAddHoursUTC(DateTime& dt, int hours)
{
    dt.hour() += hours;
    dt.normalizeUTC();
}

void aDateTimeAddHoursLocal(DateTime& dt, int hours, int timezone)
{
    dt.hour() += hours;
    dt.normalizeLocal(timezone);
}

void aDateTimeAddHoursBJT(DateTime& dt, int hours)
{
    dt.hour() += hours;
    dt.normalizeBJT();
}

void aDateTimeAddMinutes(DateTime& dt, int minutes)
{
    dt.minute() += minutes;
    dt.normalize();
}
void aDateTimeAddMinutesUTC(DateTime& dt, int minutes)
{
    dt.minute() += minutes;
    dt.normalizeUTC();
}
void aDateTimeAddMinutesLocal(DateTime& dt, int minutes, int timezone)
{
    dt.minute() += minutes;
    dt.normalizeLocal(timezone);
}


void aDateTimeAddMinutesBJT(DateTime& dt, int minutes)
{
    dt.minute() += minutes;
    dt.normalizeBJT();
}


void aDateTimeAddSeconds(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalize();
}
void aDateTimeAddSecondsUTC(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalizeUTC();
}
void aDateTimeAddSecondsLocal(DateTime& dt, double seconds, int timezone)
{
    dt.second() += seconds;
    dt.normalizeLocal(timezone);
}
void aDateTimeAddSecondsBJT(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalizeBJT();
}

// 格式化函数实现
err_t aDateTimeFormatISO8601(const DateTime& dt, std::string& str)
{
    char buffer[64];
    int year = dt.year();
    int month = dt.month();
    int day = dt.day();
    int hour = dt.hour();
    int minute = dt.minute();
    int second = static_cast<int>(dt.second());
    int millisecond = static_cast<int>((dt.second() - second) * 1000);
    
    if (millisecond > 0) {
        sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", 
                year, month, day, hour, minute, second, millisecond);
    } else {
        sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
                year, month, day, hour, minute, second);
    }
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGregorian(const DateTime& dt, std::string& str)
{
    char buffer[64];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02.3f", 
            dt.year(), dt.month(), dt.day(), 
            dt.hour(), dt.minute(), dt.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGregorianEn(const DateTime& dt, std::string& str)
{
    char buffer[64];
    const char* monthName = dt.date().monthShortName();
    sprintf(buffer, "%d %s %04d %02d:%02d:%02.3f", 
            dt.day(), monthName, dt.year(), 
            dt.hour(), dt.minute(), dt.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGMT(const DateTime& dt, std::string& str)
{
    char buffer[64];
    const char* weekdayName = dt.date().weekDayShortName();
    const char* monthName = dt.date().monthShortName();
    sprintf(buffer, "%s, %02d %s %04d %02d:%02d:%02.3f GMT", 
            weekdayName, dt.day(), monthName, dt.year(), 
            dt.hour(), dt.minute(), dt.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatRFC3339(const DateTime& dt, std::string& str)
{
    char buffer[64];
    // 默认使用+00:00时区，实际使用时可能需要根据具体时区调整
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02.3f+00:00", 
            dt.year(), dt.month(), dt.day(), 
            dt.hour(), dt.minute(), dt.second());
    
    str = buffer;
    return eNoError;
}

#ifdef AST_ENABLE_DATETIME_FORMAT_RFC
err_t aDateTimeFormatRFC1123(const DateTime& dt, std::string& str)
{
    // RFC 1123格式与GMT格式类似
    return aDateTimeFormatGMT(dt, str);
}

err_t aDateTimeFormatRFC2822(const DateTime& dt, std::string& str)
{
    char buffer[64];
    const char* weekdayName = dt.date().weekDayShortName();
    const char* monthName = dt.date().monthShortName();
    // 默认使用+0000时区，实际使用时可能需要根据具体时区调整
    sprintf(buffer, "%s, %02d %s %04d %02d:%02d:%02.3f +0000", 
            weekdayName, dt.day(), monthName, dt.year(), 
            dt.hour(), dt.minute(), dt.second());
    
    str = buffer;
    return eNoError;
}
#endif

// 解析函数实现
err_t aDateTimeParseISO8601(StringView str, DateTime& dt)
{
    // 简化实现，支持基本的ISO 8601格式：YYYY-MM-DDThh:mm:ss.sssZ
    // 实际应用中可能需要更复杂的解析逻辑
    
    const char* s = str.data();
    int year, month, day, hour, minute;
    double second = 0.0;
    
    // 尝试解析基本格式
    if (strlen(s) >= 19) {
        if (sscanf(s, "%04d-%02d-%02dT%02d:%02d:%lf", 
                   &year, &month, &day, &hour, &minute, &second) == 6) {
            dt.year() = year;
            dt.month() = month;
            dt.day() = day;
            dt.hour() = hour;
            dt.minute() = minute;
            dt.second() = second;
            return eNoError;
        }
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseRFC3339(StringView str, DateTime& dt)
{
    // RFC 3339格式与ISO 8601格式类似，可以复用解析逻辑
    // 这里简化实现，不处理时区部分
    return aDateTimeParseISO8601(str, dt);
}

err_t aDateTimeParseGregorian(StringView str, DateTime& dt)
{
    // 解析格式：YYYY-MM-DD HH:mm:ss.sss
    const char* s = str.data();
    int year, month, day, hour, minute;
    double second = 0.0;
    
    if (sscanf(s, "%04d-%02d-%02d %02d:%02d:%lf", 
               &year, &month, &day, &hour, &minute, &second) == 6) {
        dt.year() = year;
        dt.month() = month;
        dt.day() = day;
        dt.hour() = hour;
        dt.minute() = minute;
        dt.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseGregorianEn(StringView str, DateTime& dt)
{
    // 解析格式：dd Mon YYYY HH:mm:ss.sss
    const char* s = str.data();
    int day, year, hour, minute;
    double second = 0.0;
    char monthName[10];
    
    if (sscanf(s, "%d %9s %d %02d:%02d:%lf", 
               &day, monthName, &year, &hour, &minute, &second) == 6) {
        // 简单的月份名称映射
        int month = 1;
        const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (int i = 0; i < 12; i++) {
            if (strcmp(monthName, monthNames[i]) == 0) {
                month = i + 1;
                break;
            }
        }
        
        dt.year() = year;
        dt.month() = month;
        dt.day() = day;
        dt.hour() = hour;
        dt.minute() = minute;
        dt.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseGMT(StringView str, DateTime& dt)
{
    // 解析格式：Day, dd Mon YYYY HH:mm:ss.sss GMT
    const char* s = str.data();
    char weekdayName[10], monthName[10];
    int day, year, hour, minute;
    double second = 0.0;
    
    if (sscanf(s, "%9[^,], %d %9s %d %02d:%02d:%lf GMT", 
               weekdayName, &day, monthName, &year, &hour, &minute, &second) == 7) {
        // 简单的月份名称映射
        int month = 1;
        const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (int i = 0; i < 12; i++) {
            if (strcmp(monthName, monthNames[i]) == 0) {
                month = i + 1;
                break;
            }
        }
        
        dt.year() = year;
        dt.month() = month;
        dt.day() = day;
        dt.hour() = hour;
        dt.minute() = minute;
        dt.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}

// 静态方法实现
DateTime DateTime::FromString(StringView str, StringView format)
{
    DateTime dt;
    // 这里实现一个简化版本，实际应用中可能需要更复杂的格式解析
    // 支持的格式占位符：yyyy, MM, dd, HH, mm, ss
    
    const char* s = str.data();
    const char* fmt = format.data();
    
    // 尝试基本的ISO格式解析
    if (aDateTimeParseISO8601(str, dt) == eNoError) {
        return dt;
    }
    
    // 尝试格里高利格式解析
    if (aDateTimeParseGregorian(str, dt) == eNoError) {
        return dt;
    }
    
    // 如果都失败，返回当前时间
    aCurrentDateTimeLocal(dt);
    return dt;
}

DateTime DateTime::FromString(StringView str)
{
    // 默认使用ISO 8601格式
    return FromString(str, "yyyy-MM-dd HH:mm:ss");
}

DateTime DateTime::FromGregorian(StringView str)
{
    DateTime dt;
    if (aDateTimeParseGregorian(str, dt) != eNoError) {
        // 如果解析失败，返回当前时间
        aCurrentDateTimeLocal(dt);
    }
    return dt;
}

AST_NAMESPACE_END