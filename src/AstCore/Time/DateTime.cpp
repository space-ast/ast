/// @file      DateTime.cpp
/// @brief     
/// @details   ~
/// @author    axel
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
#include "AstCore/JulianDate.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/String.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

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

void aCurrentDateTimeLocal(DateTime& dttm)
{
    time_t t = time(NULL);
	dttm = DateTime::FromTimeTLocal(t);
}

void aCurrentDateTimeUTC(DateTime &dttm)
{
    time_t t = time(NULL);
	dttm = DateTime::FromTimeTUTC(t);
}

void aDateTimeNormalize(DateTime& dttm)
{
    aTimeNormalize(dttm.time());
    int rday = (int)floor(dttm.hour() / 24.);
    dttm.hour() -= rday * 24;
    dttm.day() += rday;
    aDateNormalize(dttm.date());
}


void aDateTimeNormalizeUTC(DateTime& dttm)
{
    using std::min;
    using std::max;

    int minute = dttm.time().hour() * 60 + dttm.minute();
    double second = dttm.time().second();
    int dday = static_cast<int>((second + minute * 60) / 86400.);
    if (dday == 0) {
        aDateTimeNormalize(dttm);
        return;
    }
    else {
        int mjd1 = aDateToMJD(dttm.date());
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
        aMJDToDate(mjd2, dttm.date());
        minute = min(max(minute_new + int(second_new/60), 0), 1440 - 1);
        second = second_new + (minute_new - minute) * 60;
        dttm.time().hour() = minute / 60;
        dttm.time().minute() = minute - dttm.time().hour() * 60;
        dttm.time().second() = second;
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
    
void aDateTimeNormalizeLocal(DateTime& dttm, int timezone)
{
    int dateDiff = _aGregDateDifference(dttm.hour(), timezone);
	dttm.hour() -= (timezone + dateDiff * 24);
    dttm.day() += dateDiff;

    aDateTimeNormalizeUTC(dttm);

    dateDiff = _aTimeZoneDateDifference(dttm.hour(), timezone);
	dttm.day() += dateDiff;
	dttm.hour() += (timezone - dateDiff * 24);
    dttm.date().normalize();
}

void aDateTimeNormalizeBJT(DateTime& dttm)
{
    aDateTimeNormalizeLocal(dttm, 8);
}

void aDateTimeAddYears(DateTime& dttm, int years)
{
    dttm.year() += years;
    dttm.normalize();
}
void aDateTimeAddMonths(DateTime& dttm, int months)
{
    dttm.month() += months;
    dttm.normalize();
}
void aDateTimeAddDays(DateTime& dttm, int days)
{
    dttm.day() += days;
    dttm.normalize();
}
void aDateTimeAddDaysUTC(DateTime& dttm, int days)
{
    dttm.hour() += days * 24;
    dttm.normalizeUTC();
}
void aDateTimeAddDaysLocal(DateTime& dttm, int days, int timezone)
{
    dttm.hour() += days * 24;
    dttm.normalizeLocal(timezone);
}
void aDateTimeAddDaysBJT(DateTime& dttm, int days)
{
    dttm.hour() += days * 24;
    dttm.normalizeBJT();
}
void aDateTimeAddHours(DateTime& dttm, int hours)
{
    dttm.hour() += hours;
    dttm.normalize();
}

void aDateTimeAddHoursUTC(DateTime& dttm, int hours)
{
    dttm.hour() += hours;
    dttm.normalizeUTC();
}

void aDateTimeAddHoursLocal(DateTime& dttm, int hours, int timezone)
{
    dttm.hour() += hours;
    dttm.normalizeLocal(timezone);
}

void aDateTimeAddHoursBJT(DateTime& dttm, int hours)
{
    dttm.hour() += hours;
    dttm.normalizeBJT();
}

void aDateTimeAddMinutes(DateTime& dttm, int minutes)
{
    dttm.minute() += minutes;
    dttm.normalize();
}
void aDateTimeAddMinutesUTC(DateTime& dttm, int minutes)
{
    dttm.minute() += minutes;
    dttm.normalizeUTC();
}
void aDateTimeAddMinutesLocal(DateTime& dttm, int minutes, int timezone)
{
    dttm.minute() += minutes;
    dttm.normalizeLocal(timezone);
}


void aDateTimeAddMinutesBJT(DateTime& dttm, int minutes)
{
    dttm.minute() += minutes;
    dttm.normalizeBJT();
}


void aDateTimeAddSeconds(DateTime& dttm, double seconds)
{
    dttm.second() += seconds;
    dttm.normalize();
}
void aDateTimeAddSecondsUTC(DateTime& dttm, double seconds)
{
    dttm.second() += seconds;
    dttm.normalizeUTC();
}
void aDateTimeAddSecondsLocal(DateTime& dttm, double seconds, int timezone)
{
    dttm.second() += seconds;
    dttm.normalizeLocal(timezone);
}
void aDateTimeAddSecondsBJT(DateTime& dttm, double seconds)
{
    dttm.second() += seconds;
    dttm.normalizeBJT();
}

// 格式化函数实现
err_t aDateTimeFormatISO8601(const DateTime& dttm, std::string& str)
{
    char buffer[100];
    int year = dttm.year();
    int month = dttm.month();
    int day = dttm.day();
    int hour = dttm.hour();
    int minute = dttm.minute();
    int second = static_cast<int>(dttm.second());
    int millisecond = static_cast<int>((dttm.second() - second) * 1000);
    
    if (millisecond > 0) {
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", 
                year, month, day, hour, minute, second, millisecond);
    } else {
        snprintf(buffer, sizeof(buffer), "%04d-%02d-%02dT%02d:%02d:%02dZ", 
                year, month, day, hour, minute, second);
    }
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGregorian(const DateTime& dttm, std::string& str, int precision)
{
    char buffer[64];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02.*f", 
            dttm.year(), dttm.month(), dttm.day(), 
            dttm.hour(), dttm.minute(), precision, dttm.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGregorianEn(const DateTime& dttm, std::string& str)
{
    char buffer[64];
    const char* monthName = dttm.date().monthShortName();
    sprintf(buffer, "%d %s %04d %02d:%02d:%02.3f", 
            dttm.day(), monthName, dttm.year(), 
            dttm.hour(), dttm.minute(), dttm.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatGMT(const DateTime& dttm, std::string& str)
{
    char buffer[64];
    const char* weekdayName = dttm.date().weekDayShortName();
    const char* monthName = dttm.date().monthShortName();
    sprintf(buffer, "%s, %02d %s %04d %02d:%02d:%02.3f GMT", 
            weekdayName, dttm.day(), monthName, dttm.year(), 
            dttm.hour(), dttm.minute(), dttm.second());
    
    str = buffer;
    return eNoError;
}

err_t aDateTimeFormatRFC3339(const DateTime& dttm, std::string& str)
{
    char buffer[64];
    // 默认使用+00:00时区，实际使用时可能需要根据具体时区调整
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02.3f+00:00", 
            dttm.year(), dttm.month(), dttm.day(), 
            dttm.hour(), dttm.minute(), dttm.second());
    
    str = buffer;
    return eNoError;
}

#ifdef AST_ENABLE_DATETIME_FORMAT_RFC
err_t aDateTimeFormatRFC1123(const DateTime& dttm, std::string& str)
{
    // RFC 1123格式与GMT格式类似
    return aDateTimeFormatGMT(dttm, str);
}

err_t aDateTimeFormatRFC2822(const DateTime& dttm, std::string& str)
{
    char buffer[64];
    const char* weekdayName = dttm.date().weekDayShortName();
    const char* monthName = dttm.date().monthShortName();
    // 默认使用+0000时区，实际使用时可能需要根据具体时区调整
    sprintf(buffer, "%s, %02d %s %04d %02d:%02d:%02.3f +0000", 
            weekdayName, dttm.day(), monthName, dttm.year(), 
            dttm.hour(), dttm.minute(), dttm.second());
    
    str = buffer;
    return eNoError;
}
#endif

// 解析函数实现
err_t aDateTimeParseISO8601(StringView str, DateTime& dttm)
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
            dttm.year() = year;
            dttm.month() = month;
            dttm.day() = day;
            dttm.hour() = hour;
            dttm.minute() = minute;
            dttm.second() = second;
            return eNoError;
        }
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseRFC3339(StringView str, DateTime& dttm)
{
    // RFC 3339格式与ISO 8601格式类似，可以复用解析逻辑
    // 这里简化实现，不处理时区部分
    return aDateTimeParseISO8601(str, dttm);
}

err_t aDateTimeParseGregorian(StringView str, DateTime& dttm)
{
    // 解析格式：YYYY-MM-DD HH:mm:ss.sss
    const char* s = str.data();
    int year, month, day, hour, minute;
    double second = 0.0;
    
    if (sscanf(s, "%04d-%02d-%02d %02d:%02d:%lf", 
               &year, &month, &day, &hour, &minute, &second) == 6) {
        dttm.year() = year;
        dttm.month() = month;
        dttm.day() = day;
        dttm.hour() = hour;
        dttm.minute() = minute;
        dttm.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseGregorianEn(StringView str, DateTime& dttm)
{
    // 解析格式：dd Mon YYYY HH:mm:ss.sss
    const char* s = str.data();
    int day, year, hour, minute;
    double second = 0.0;
    char monthName[10]{'\0'};
    
    if (sscanf(s, "%d %9s %d %02d:%02d:%lf", 
               &day, monthName, &year, &hour, &minute, &second) == 6) {
        // 简单的月份名称映射
        int month = 1;
        const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (int i = 0; i < 12; i++) {
            if (posix::strcasecmp(monthName, monthNames[i]) == 0) {
                month = i + 1;
                break;
            }
        }
        
        dttm.year() = year;
        dttm.month() = month;
        dttm.day() = day;
        dttm.hour() = hour;
        dttm.minute() = minute;
        dttm.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}

err_t aDateTimeParseGMT(StringView str, DateTime& dttm)
{
    // 解析格式：Day, dd Mon YYYY HH:mm:ss.sss GMT
    const char* s = str.data();
    char weekdayName[10]{ '\0' }, monthName[10]{ '\0' };
    int day, year, hour, minute;
    double second = 0.0;
    
    if (sscanf(s, "%9[^,], %d %9s %d %02d:%02d:%lf GMT", 
               weekdayName, &day, monthName, &year, &hour, &minute, &second) == 7) {
        // 简单的月份名称映射
        int month = 1;
        const char* monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        for (int i = 0; i < 12; i++) {
            if (posix::strcasecmp(monthName, monthNames[i]) == 0) {
                month = i + 1;
                break;
            }
        }
        
        dttm.year() = year;
        dttm.month() = month;
        dttm.day() = day;
        dttm.hour() = hour;
        dttm.minute() = minute;
        dttm.second() = second;
        return eNoError;
    }
    
    return eErrorInvalidParam;
}


err_t aDateTimeParseAny(StringView str, DateTime &dttm)
{
    err_t ret = aDateTimeParseISO8601(str, dttm);
    if (ret == eNoError) {
        return ret;
    }
    
    ret = aDateTimeParseRFC3339(str, dttm);
    if (ret == eNoError) {
        return ret;
    }
    
    ret = aDateTimeParseGregorian(str, dttm);
    if (ret == eNoError) {
        return ret;
    }
    
    ret = aDateTimeParseGregorianEn(str, dttm);
    if (ret == eNoError) {
        return ret;
    }
    
    ret = aDateTimeParseGMT(str, dttm);
    if (ret == eNoError) {
        return ret;
    }
    
    return eErrorInvalidParam;
}

DateTime DateTime::FromJD(const JulianDate &jd)
{
    DateTime dttm;
    aJDToDateTime(jd, dttm);
    return dttm;
}

// 静态方法实现
DateTime DateTime::FromString(StringView str, StringView format)
{
    DateTime dttm;
    err_t err = aDateTimeParse(str, format, dttm);
    if (err != eNoError) {
        // 如果解析失败
        aError("Failed to parse datetime string '%s' with format '%s'", str.data(), format.data());
    }
    return dttm;
}

DateTime DateTime::FromString(StringView str)
{
    // 采用默认格式解析
    DateTime dttm;
    err_t err = aDateTimeParseAny(str, dttm);
    if(err != eNoError) {
        // 如果解析失败
        aError("Failed to parse datetime string '%s'", str.data());
    }
    return dttm;
}

DateTime DateTime::FromGregorian(StringView str)
{
    DateTime dttm;
    if (aDateTimeParseGregorian(str, dttm) != eNoError) {
        // 如果解析失败，返回当前时间
        aCurrentDateTimeLocal(dttm);
    }
    return dttm;
}

AST_NAMESPACE_END