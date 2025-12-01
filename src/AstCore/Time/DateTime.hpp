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
 
#include "AstGlobal.h"
#include "Date.hpp"
#include "Time.hpp"
#include <time.h>


struct tm;

AST_NAMESPACE_BEGIN


class DateTime;

/// @brief 获取当前本地日期时间
AST_CORE_CAPI void aCurrentDateTimeLocal(DateTime& dt);

/// @brief 获取当前UTC日期时间
AST_CORE_CAPI void aCurrentDateTimeUTC(DateTime& dt);

/// @brief 规范化日期时间对象
/// @details 将时间调整到0-23时59分59秒之间，并调整日期到正确的范围内，不考虑闰秒
/// @param dt 
/// @return 
AST_CORE_CAPI void aDateTimeNormalize(DateTime& dt);

/// @brief 规范化日期时间对象为UTC时间
/// @details 将日期时间对象调整到UTC时间，考虑闰秒
/// @param dt 
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeUTC(DateTime& dt);

/// @brief 规范化日期时间对象为本地时间
/// @details 将日期时间对象调整到本地时间，考虑时区和闰秒
/// @param dt 
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeLocal(DateTime& dt, int timezone);

/// @brief 规范化日期时间对象为北京时间
/// @details 将日期时间对象调整到北京时间，考虑闰秒
/// @param dt 
/// @return 
AST_CORE_CAPI void aDateTimeNormalizeBJT(DateTime& dt);

/// @brief 增加年
/// @details 将日期时间对象增加指定的年数
/// @param dt 
/// @param years 
/// @return 
AST_CORE_CAPI void aDateTimeAddYears(DateTime& dt, int years);

/// @brief 增加月
/// @details 将日期时间对象增加指定的月数
/// @param dt 
/// @param months 
/// @return 
AST_CORE_CAPI void aDateTimeAddMonths(DateTime& dt, int months);

/// @brief 增加日
/// @details 将日期时间对象增加指定的天数
/// @param dt 
/// @param days 
/// @return 
AST_CORE_CAPI void aDateTimeAddDays(DateTime& dt, int days);

/// @brief 增加时
/// @details 将日期时间对象增加指定的小时数
/// @param dt 
/// @param hours 
/// @return 
AST_CORE_CAPI void aDateTimeAddHours(DateTime& dt, int hours);


/// @brief 增加分
/// @details 将日期时间对象增加指定的分钟数
/// @param dt 
/// @param minutes 
/// @return 
AST_CORE_CAPI void aDateTimeAddMinutes(DateTime& dt, int minutes);

/// @brief 增加秒
/// @details 将日期时间对象增加指定的秒数
/// @param dt 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSeconds(DateTime& dt, double seconds);


/// @brief 增加秒（UTC时间）
/// @details 将日期时间对象增加指定的秒数（UTC时间），考虑闰秒
/// @param dt 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsUTC(DateTime& dt, double seconds);


/// @brief 增加秒（本地时间）
/// @details 将日期时间对象增加指定的秒数（本地时间），考虑时区和闰秒
/// @param dt 
/// @param seconds 
/// @param timezone 时区偏移，单位：小时
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsLocal(DateTime& dt, double seconds, int timezone);


/// @brief 增加秒（北京时间）
/// @details 将日期时间对象增加指定的秒数（北京时间），考虑闰秒
/// @param dt 
/// @param seconds 
/// @return 
AST_CORE_CAPI void aDateTimeAddSecondsBJT(DateTime& dt, double seconds);


/// @brief 格式化日期时间为格里高利历格式
/// @details 将日期时间对象格式化为格里高利历格式的字符串，例如：2025-11-21 12:34:56
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGregorian(const DateTime& dt, std::string& str);


/// @brief 格式化日期时间为格里高利历格式（英文）
/// @details 将日期时间对象格式化为格里高利历格式的字符串（英文），例如：1 Jan 1970 00:00:00
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGregorianEn(const DateTime& dt, std::string& str);


/// @brief 格式化日期时间为GMT格式
/// @details 将日期时间对象格式化为GMT格式的字符串，例如：Sat, 21 Nov 2025 12:34:56 GMT
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatGMT(const DateTime& dt, std::string& str);


/// @brief 格式化日期时间为ISO 8601格式
/// @details 将日期时间对象格式化为ISO 8601格式的字符串，例如：2025-11-21T12:34:56Z
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatISO8601(const DateTime& dt, std::string& str);


/// @brief 格式化日期时间为简单ISO 8601格式
/// @details 将日期时间对象格式化为ISO 8601格式的字符串，例如：2025-11-21 12:34:56
AST_CORE_CAPI err_t aDateTimeFormatISO(const DateTime& dt, std::string& str)
{
    return aDateTimeFormatISO8601(dt, str);
}



/// @brief 格式化日期时间为RFC 3339格式
/// @details 将日期时间对象格式化为RFC 3339格式的字符串，例如：2025-11-21T12:34:56+08:00
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC3339(const DateTime& dt, std::string& str);


#ifdef AST_ENABLE_DATETIME_FORMAT_RFC // 启用其他RFC日期时间格式化

/// @brief 格式化日期时间为RFC 1123格式
/// @details 将日期时间对象格式化为RFC 1123格式的字符串，
/// 例如：Sat, 21 Nov 2025 12:34:56 GMT
/// 时区必须使用 GMT（RFC 1123 要求始终使用格林尼治时间）
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC1123(const DateTime& dt, std::string& str);


/// @brief 格式化日期时间为RFC 2822格式
/// @details 将日期时间对象格式化为RFC 2822格式的字符串，
/// 例如：Sat, 21 Nov 2025 12:34:56 +0800
///       Wed, 18 Feb 2015 23:16:09 GMT
/// 时区可以是 "+0000" 格式的数字时区，或者是 "GMT"、"UTC" 等标准时区名称
/// @param dt 
/// @param str 
/// @return err_t 
AST_CORE_CAPI err_t aDateTimeFormatRFC2822(const DateTime& dt, std::string& str);

#endif


/// @brief 解析ISO 8601格式的日期时间字符串
/// @details 将ISO 8601格式的字符串解析为日期时间对象
/// @param str 包含ISO 8601格式日期时间的字符串
/// @param dt 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseISO8601(const std::string& str, DateTime& dt);


A_ALWAYS_INLINE err_t aDateTimeParseISO(const std::string& str, DateTime& dt)
{
    return aDateTimeParseISO8601(str, dt);
}


/// @brief 解析RFC 3339格式的日期时间字符串
/// @details 将RFC 3339格式的字符串解析为日期时间对象
/// @param str 包含RFC 3339格式日期时间的字符串
/// @param dt 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseRFC3339(const std::string& str, DateTime& dt);


/// @brief 解析格里高利历格式的日期时间字符串
/// @details 将格里高利历格式的字符串解析为日期时间对象
/// @param str 包含格里高利历格式日期时间的字符串
/// @param dt 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGregorian(const std::string& str, DateTime& dt);



/// @brief 解析格里高利历格式（英文）的日期时间字符串
/// @details 将格里高利历格式（英文）的字符串解析为日期时间对象
/// @param str 包含格里高利历格式（英文）日期时间的字符串
/// @param dt 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGregorianEn(const std::string& str, DateTime& dt);


/// @brief 解析GMT格式的日期时间字符串
/// @details 将GMT格式的字符串解析为日期时间对象
/// @param str 包含GMT格式日期时间的字符串
/// @param dt 输出参数，解析后的日期时间对象
/// @return err_t 错误码，eNoError表示成功
AST_CORE_CAPI err_t aDateTimeParseGMT(const std::string& str, DateTime& dt);




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
    /// @brief 从字符串解析日期时间
    /// @param str 包含日期时间的字符串
    /// @param format 日期时间格式，例如："yyyy-MM-dd HH:mm:ss"
    /// @return DateTime 解析后的日期时间对象
    static DateTime FromString(const std::string& str, const std::string& format);

    /// @brief 从字符串解析日期时间（默认格式："yyyy-MM-dd HH:mm:ss"）
    /// @param str 包含日期时间的字符串
    /// @return DateTime 解析后的日期时间对象
    static DateTime FromString(const std::string& str);

    /// @brief 从格里高利历日期时间创建日期时间对象
    /// @return DateTime 日期时间对象
    static DateTime FromGregorian(const std::string& str);


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
        DateTime dt = *this;
        dt.normalize();
        return dt;
    }
    DateTime normalizedUTC() const{
        DateTime dt = *this;
        dt.normalizeUTC();
        return dt;
    }
    DateTime normalizedLocal(int timezone) const{
        DateTime dt = *this;
        dt.normalizeLocal(timezone);
        return dt;
    }
    DateTime normalizedBJT() const{
        DateTime dt = *this;
        dt.normalizeBJT();
        return dt;
    }
    

public:
    Date date_;     ///< 日期
    Time time_;     ///< 时间
};

A_ALWAYS_INLINE
DateTime aCurrentDateTimeLocal()
{
    DateTime dt;
    aCurrentDateTimeLocal(dt);
    return dt;
}

A_ALWAYS_INLINE
DateTime aCurrentDateTimeUTC()
{
    DateTime dt;
    aCurrentDateTimeUTC(dt);
    return dt;
}


AST_NAMESPACE_END
 
