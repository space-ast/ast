///
/// @file      DataTime.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      21.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
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
 
#pragma once
 
#include "AstUtil/StringView.hpp"
#include "AstGlobal.h"
#include "Date.hpp"
#include "Time.hpp"
#include <time.h>
#include <string>


struct tm;

AST_NAMESPACE_BEGIN


class DateTime;

/// @brief 获取当前本地日期时间
AST_CORE_CAPI void aCurrentDateTimeLocal(DateTime& dttm);

/// @brief 获取当前UTC日期时间
AST_CORE_CAPI void aCurrentDateTimeUTC(DateTime& dttm);

/// @brief 规范化日期时间对象
/// @details 将时间调整到0-23时59分59秒之间，并调整日期到正确的范围内，不考虑闰秒
/// @param dttm 
/// @return 
AST_CORE_CAPI void aDateTimeNormalize(DateTime& dttm);

/// @brief 规范化日期时间对象(按UTC时间规范化)
/// @details 将日期时间对象调整到0-23时59分60秒，考虑闰秒
/// @param dttm 
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeUTC(DateTime& dttm);

/// @brief 规范化日期时间对象(按本地时间规范化)
/// @details 将日期时间对象调整到0-23时59分60秒，考虑时区和闰秒
/// @param dttm 本地时间
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeLocal(DateTime& dttm, int timezone);

/// @brief 规范化日期时间对象(按北京时间规范化)
/// @details 将日期时间对象调整到0-23时59分60秒，考虑闰秒
/// @param dttm 北京时间
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeBJT(DateTime& dttm);

/// @brief 增加年
/// @details 将日期时间对象增加指定的年数，并规范化日期时间对象
/// @param dttm 
/// @param years 
/// @return 
AST_CORE_CAPI void aDateTimeAddYears(DateTime& dttm, int years);

/// @brief 增加月
/// @details 将日期时间对象增加指定的月数，并规范化日期时间对象
/// @param dttm 
/// @param months 
/// @return 
AST_CORE_CAPI void aDateTimeAddMonths(DateTime& dttm, int months);

/// @brief 增加日
/// @details 将日期时间对象增加指定的天数，并规范化日期时间对象，不考虑闰秒
/// @param dttm 
/// @param days 
/// @return 
AST_CORE_CAPI void aDateTimeAddDays(DateTime& dttm, int days);


/// @brief 增加日（UTC时间）
/// @details 将日期时间对象增加指定的天数(days*86400s)（UTC时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param days 增加的天数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddDaysUTC(DateTime& dttm, int days);


/// @brief 增加日（本地时间）
/// @details 将日期时间对象增加指定的天数(days*86400s)（本地时间），并规范化日期时间对象，考虑时区和闰秒
/// @param dttm 
/// @param days 增加的天数，可正可负
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeAddDaysLocal(DateTime& dttm, int days, int timezone);


/// @brief 增加日（北京时间）
/// @details 将日期时间对象增加指定的天数(days*86400s)（北京时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param days 增加的天数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddDaysBJT(DateTime& dttm, int days);



/// @brief 增加时
/// @details 将日期时间对象增加指定的小时数(hours*3600s)，并规范化日期时间对象，不考虑闰秒
/// @param dttm 
/// @param hours 增加的小时数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddHours(DateTime& dttm, int hours);


/// @brief 增加时（UTC时间）
/// @details 将日期时间对象增加指定的小时数(hours*3600s)（UTC时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param hours 增加的小时数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddHoursUTC(DateTime& dttm, int hours);

/// @brief 增加时（本地时间）
/// @details 将日期时间对象增加指定的小时数(hours*3600s)（本地时间），并规范化日期时间对象，考虑时区和闰秒
/// @param dttm 
/// @param hours 增加的小时数，可正可负
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeAddHoursLocal(DateTime& dttm, int hours, int timezone);


/// @brief 增加时（北京时间）
/// @details 将日期时间对象增加指定的小时数(hours*3600s)（北京时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param hours 增加的小时数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddHoursBJT(DateTime& dttm, int hours);


/// @brief 增加分
/// @details 将日期时间对象增加指定的分钟数(minutes*60s)，并规范化日期时间对象，不考虑闰秒
/// @param dttm 
/// @param minutes 增加的分钟数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddMinutes(DateTime& dttm, int minutes);


/// @brief 增加分（UTC时间）
/// @details 将日期时间对象增加指定的分钟数（UTC时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param minutes 增加的分钟数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddMinutesUTC(DateTime& dttm, int minutes);



/// @brief 增加分（本地时间）
/// @details 将日期时间对象增加指定的分钟数(minutes*60s)（本地时间），并规范化日期时间对象，考虑时区和闰秒
/// @param dttm 
/// @param minutes 增加的分钟数，可正可负
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeAddMinutesLocal(DateTime& dttm, int minutes, int timezone);


/// @brief 增加分（北京时间）
/// @details 将日期时间对象增加指定的分钟数(minutes*60s)（北京时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param minutes 增加的分钟数，可正可负
/// @return 
AST_CORE_CAPI void aDateTimeAddMinutesBJT(DateTime& dttm, int minutes);


/// @brief 增加秒
/// @details 将日期时间对象增加指定的秒数，并规范化日期时间对象，不考虑闰秒
/// @param dttm 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSeconds(DateTime& dttm, double seconds);


/// @brief 增加秒（UTC时间）
/// @details 将日期时间对象增加指定的秒数（UTC时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsUTC(DateTime& dttm, double seconds);


/// @brief 增加秒（本地时间）
/// @details 将日期时间对象增加指定的秒数（本地时间），并规范化日期时间对象，考虑时区和闰秒
/// @param dttm 
/// @param seconds 
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsLocal(DateTime& dttm, double seconds, int timezone);


/// @brief 增加秒（北京时间）
/// @details 将日期时间对象增加指定的秒数（北京时间），并规范化日期时间对象，考虑闰秒
/// @param dttm 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsBJT(DateTime& dttm, double seconds);


/// @brief 格式化日期时间为字符串
/// @details 将日期时间对象格式化为指定格式的字符串
/// @param dttm 
/// @param format 格式化字符串，例如："%Y-%m-%d %H:%M:%S"
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormat(const DateTime& dttm, StringView format, std::string& str);


/// @brief 格式化日期时间为格里高利历格式
/// @details 将日期时间对象格式化为格里高利历格式的字符串，例如：2025-11-21 12:34:56
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGregorian(const DateTime& dttm, std::string& str);


/// @brief 格式化日期时间为格里高利历格式（英文）
/// @details 将日期时间对象格式化为格里高利历格式的字符串（英文），例如：1 Jan 1970 00:00:00
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGregorianEn(const DateTime& dttm, std::string& str);


/// @brief 格式化日期时间为GMT格式
/// @details 将日期时间对象格式化为GMT格式的字符串，例如：Sat, 21 Nov 2025 12:34:56 GMT
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGMT(const DateTime& dttm, std::string& str);


/// @brief 格式化日期时间为ISO 8601格式
/// @details 将日期时间对象格式化为ISO 8601格式的字符串，例如：2025-11-21T12:34:56Z
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatISO8601(const DateTime& dttm, std::string& str);


/// @brief 格式化日期时间为简单ISO 8601格式
/// @details 将日期时间对象格式化为ISO 8601格式的字符串，例如：2025-11-21 12:34:56
A_ALWAYS_INLINE err_t aDateTimeFormatISO(const DateTime& dttm, std::string& str)
{
    return aDateTimeFormatISO8601(dttm, str);
}



/// @brief 格式化日期时间为RFC 3339格式
/// @details 将日期时间对象格式化为RFC 3339格式的字符串，例如：2025-11-21T12:34:56+08:00
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC3339(const DateTime& dttm, std::string& str);


#ifdef AST_ENABLE_DATETIME_FORMAT_RFC // 启用其他RFC日期时间格式化

/// @brief 格式化日期时间为RFC 1123格式
/// @details 将日期时间对象格式化为RFC 1123格式的字符串，
/// 例如：Sat, 21 Nov 2025 12:34:56 GMT
/// 时区必须使用 GMT（RFC 1123 要求始终使用格林尼治时间）
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC1123(const DateTime& dttm, std::string& str);


/// @brief 格式化日期时间为RFC 2822格式
/// @details 将日期时间对象格式化为RFC 2822格式的字符串，
/// 例如：Sat, 21 Nov 2025 12:34:56 +0800
///       Wed, 18 Feb 2015 23:16:09 GMT
/// 时区可以是 "+0000" 格式的数字时区，或者是 "GMT"、"UTC" 等标准时区名称
/// @param dttm 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC2822(const DateTime& dttm, std::string& str);

#endif


/// @brief 解析ISO 8601格式的日期时间字符串
/// @details 将ISO 8601格式的字符串解析为日期时间对象
/// @param str 包含ISO 8601格式日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseISO8601(StringView str, DateTime& dttm);


A_ALWAYS_INLINE err_t aDateTimeParseISO(StringView str, DateTime& dttm)
{
    return aDateTimeParseISO8601(str, dttm);
}



/// @brief 解析RFC 3339格式的日期时间字符串
/// @details 将RFC 3339格式的字符串解析为日期时间对象
/// @param str 包含RFC 3339格式日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseRFC3339(StringView str, DateTime& dttm);


/// @brief 解析格里高利历格式的日期时间字符串
/// @details 将格里高利历格式的字符串解析为日期时间对象
/// @param str 包含格里高利历格式日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGregorian(StringView str, DateTime& dttm);



/// @brief 解析格里高利历格式（英文）的日期时间字符串
/// @details 将格里高利历格式（英文）的字符串解析为日期时间对象
/// @param str 包含格里高利历格式（英文）日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGregorianEn(StringView str, DateTime& dttm);


/// @brief 解析GMT格式的日期时间字符串
/// @details 将GMT格式的字符串解析为日期时间对象
/// @param str 包含GMT格式日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGMT(StringView str, DateTime& dttm);


/// @brief 解析自定义格式的日期时间字符串
/// @details 将自定义格式的字符串解析为日期时间对象
/// @param str 包含自定义格式日期时间的字符串
/// @param format 日期时间格式，参考strptime函数的格式规范
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParse(StringView str, StringView format, DateTime& dttm);


/// @brief 解析任意格式的日期时间字符串
/// @details 尝试解析多种日期时间格式，包括ISO 8601、RFC 3339、格里高利历等
/// @param str 包含日期时间的字符串
/// @param dttm 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseAny(StringView str, DateTime& dttm);


/// @brief 日期时间
class DateTime
{
public:
    /// @brief 
    enum ETimeZone{
        eUTC = 0,   ///< UTC时间（东0区）
        eBJT = 8,   ///< 北京时间（东8区）
    };
public:
    /// @brief 从儒略日数创建日期时间对象
    /// @param jd 儒略日数
    /// @return DateTime 日期时间对象
    AST_CORE_API
    static DateTime FromJD(const JulianDate& jd);

    /// @brief 从儒略日数创建日期时间对象
    /// @param jd 儒略日数
    /// @return DateTime 日期时间对象
    A_ALWAYS_INLINE
    static DateTime FromJulianDate(const JulianDate& jd){return FromJD(jd);}

    /// @brief 从字符串解析日期时间
    /// @param str 包含日期时间的字符串
    /// @param format 日期时间格式，参考strptime函数的格式规范
    /// @return DateTime 解析后的日期时间对象
    AST_CORE_API
    static DateTime FromString(StringView str, StringView format);

    /// @brief 从字符串解析日期时间（默认格式："yyyy-MM-dd HH:mm:ss"）
    /// @param str 包含日期时间的字符串
    /// @return DateTime 解析后的日期时间对象
    AST_CORE_API
    static DateTime FromString(StringView str);

    /// @brief 从格里高利历日期时间创建日期时间对象
    /// @return DateTime 日期时间对象
    AST_CORE_API
    static DateTime FromGregorian(StringView str);


    /// @brief 根据系统时间创建日期时间对象
    AST_CORE_API
    static DateTime FromTm(const tm* time);

    /// @brief 根据time_t创建本地时间的日期时间对象
    AST_CORE_API
    static DateTime FromTimeTLocal(time_t time);

    /// @brief 根据time_t创建UTC时间的日期时间对象
    AST_CORE_API
    static DateTime FromTimeTUTC(time_t time);

public: 
    const Date& date() const{return date_;}
    Date& date() {return date_;}
    const Time& time() const {return time_;}
    Time& time() {return time_;}
    int year() const {return date_.year();}
    int& year() {return date_.year();}
    int month() const {return date_.month();}
    int& month() {return date_.month();}
    int day() const {return date_.day();}
    int& day() {return date_.day();}
    int hour() const {return time_.hour();}
    int& hour() {return time_.hour();}
    int minute() const {return time_.minute();}
    int& minute() {return time_.minute();}
    double second() const {return time_.second();}
    double& second() {return time_.second();}
    int dayOfYear() const {return date_.dayOfYear();}
    int dayOfWeek() const {return date_.dayOfWeek();}
    void setYear(int year) {date_.setYear(year);}
    void setMonth(int month) {date_.setMonth(month);}
    void setDay(int day) {date_.setDay(day);}
    void setHour(int hour) {time_.setHour(hour);}
    void setMinute(int minute) {time_.setMinute(minute);}
    void setSecond(double second) {time_.setSecond(second);}
public:
    void normalize(){
        aDateTimeNormalize(*this);
    }
    void normalizeUTC(){
        aDateTimeNormalizeUTC(*this);
    }
    void normalizeLocal(int timezone){
        aDateTimeNormalizeLocal(*this, timezone);
    }
    void normalizeBJT(){
        aDateTimeNormalizeBJT(*this);
    }
    DateTime normalized() const{
        DateTime dttm = *this;
        dttm.normalize();
        return dttm;
    }
    DateTime normalizedUTC() const{
        DateTime dttm = *this;
        dttm.normalizeUTC();
        return dttm;
    }
    DateTime normalizedLocal(int timezone) const{
        DateTime dttm = *this;
        dttm.normalizeLocal(timezone);
        return dttm;
    }
    DateTime normalizedBJT() const{
        DateTime dttm = *this;
        dttm.normalizeBJT();
        return dttm;
    }
public:
    void addYears(int years){
        aDateTimeAddYears(*this, years);
    }
    void addMonths(int months){
        aDateTimeAddMonths(*this, months);
    }
    void addDays(int days){
        aDateTimeAddDays(*this, days);
    }
    void addDaysUTC(int days){
        aDateTimeAddDaysUTC(*this, days);
    }
    void addDaysLocal(int days, int timezone){
        aDateTimeAddDaysLocal(*this, days, timezone);
    }
    void addDaysBJT(int days){
        aDateTimeAddDaysBJT(*this, days);
    }
    void addHours(int hours){
        aDateTimeAddHours(*this, hours);
    }
    void addHoursUTC(int hours){
        aDateTimeAddHoursUTC(*this, hours);
    }
    void addHoursLocal(int hours, int timezone){
        aDateTimeAddHoursLocal(*this, hours, timezone);
    }
    void addHoursBJT(int hours){
        aDateTimeAddHoursBJT(*this, hours);
    }
    void addMinutes(int minutes){
        aDateTimeAddMinutes(*this, minutes);
    }
    void addMinutesUTC(int minutes){
        aDateTimeAddMinutesUTC(*this, minutes);
    }
    void addMinutesLocal(int minutes, int timezone){
        aDateTimeAddMinutesLocal(*this, minutes, timezone);
    }
    void addMinutesBJT(int minutes){
        aDateTimeAddMinutesBJT(*this, minutes);
    }
    void addSeconds(double seconds){
        aDateTimeAddSeconds(*this, seconds);
    }
    void addSecondsUTC(double seconds){
        aDateTimeAddSecondsUTC(*this, seconds);
    }
    void addSecondsLocal(double seconds, int timezone){
        aDateTimeAddSecondsLocal(*this, seconds, timezone);
    }
    void addSecondsBJT(double seconds){
        aDateTimeAddSecondsBJT(*this, seconds);
    }
public:
    DateTime& operator += (double sec)
    {
        this->second() += sec;  // 适用于任何时间尺度，包括考虑闰秒的和不考虑闰秒的
        return *this;
    }
    DateTime& operator -= (double sec)
    {
        this->second() -= sec;  // 适用于任何时间尺度，包括考虑闰秒的和不考虑闰秒的
        return *this;
    }
    DateTime operator + (double sec) const
    {
        auto dttm = DateTime{ *this };
        dttm += sec;
        return dttm;
    }
    DateTime operator -(double sec) const
    {
        auto dttm = DateTime{ *this };
        dttm -= sec;
        return dttm;
    }
    
public:
    Date date_;     ///< 日期
    Time time_;     ///< 时间
};

A_ALWAYS_INLINE
DateTime aCurrentDateTimeLocal()
{
    DateTime dttm;
    aCurrentDateTimeLocal(dttm);
    return dttm;
}

A_ALWAYS_INLINE
DateTime aCurrentDateTimeUTC()
{
    DateTime dttm;
    aCurrentDateTimeUTC(dttm);
    return dttm;
}


AST_NAMESPACE_END
 
