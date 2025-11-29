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
 
