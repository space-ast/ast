///
/// @file      Date.hpp
/// @brief     
/// @details   ~
/// @author    axel
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
#include <string>
 
AST_NAMESPACE_BEGIN


/*!
    @ingroup   Core
    @defgroup Time 时间转换
    @brief 提供儒略日、日期时间等时间表示，以及不同时间系统之间的转换功能
    @{
*/


/*!
    @addtogroup Time 时间模块
    @{
*/

class Date;

/// @brief 获取月份的完整英文名称
/// @param month 月份，从1开始计数（1表示一月）
AST_CORE_CAPI const char* aMoonFullName(int month);

/// @brief 获取月份的简写英文名称
/// @param month 月份，从1开始计数（1表示一月）
AST_CORE_CAPI const char* aMoonShortName(int month);

/// @brief 获取星期几的完整英文名称
/// @param wday 星期几，从0开始计数（0表示星期日）
AST_CORE_CAPI const char* aWeekDayFullName(int wday);

/// @brief 获取星期几的简写英文名称
/// @param wday 星期几，从0开始计数（0表示星期日）
AST_CORE_CAPI const char* aWeekDayShortName(int wday);

/// @brief 判断是否为闰年
AST_CORE_CAPI bool aIsLeapYear(int year);

/// @brief 获取某年某月的天数，根据年份判断
/// @param month 月份，从1开始计数（1表示一月）
/// @param year 年份
AST_CORE_CAPI int aDaysInMonthByYear(int month, int year);

/// @brief 获取某年某月的天数，根据是否为闰年判断
/// @param month 月份，从1开始计数（1表示一月）
/// @param isLeapYear 是否为闰年
AST_CORE_CAPI int aDaysInMonthByLeap(int month, bool isLeapYear);

/// @brief 获取日期是这一年的第几天，从1开始计数
AST_CORE_CAPI int aDayOfYear(const Date& date);

/// @brief 获取日期是这一周的第几天
/// @param date 日期对象
/// @return 星期几，从0开始计数（0表示星期日）
AST_CORE_CAPI int aDayOfWeek(const Date& date);


/// @brief 将年份和一年中的天数转换为日期
AST_CORE_CAPI void aYDToDate(int year, int days, Date& date);

/// @brief 将日期转换为年份和一年中的天数
AST_CORE_CAPI void aDateToYD(const Date& date, int& year, int& days);

/// @brief 规范化日期对象
AST_CORE_CAPI void aDateNormalize(Date& date);



/// @brief 将日期转换为当天中午12点的儒略日数
AST_CORE_CAPI int aDateToJDAtNoon(const Date& date);

/// @brief 将当天中午12点的儒略日数转换为当天日期
AST_CORE_CAPI void aJDToDateAtNoon(int jd, Date& date);

/// @brief 将日期转换为当天00:00的儒略日数
AST_CORE_CAPI ImpreciseJD aDateToJD(const Date& date);

/// @brief 将儒略日数转换为当天日期
/// @param jd 当天00:00的儒略日数
/// @param date 日期对象
/// @param secInDay 可选参数，用于返回当天00:00后的秒数
AST_CORE_CAPI void aJDToDate(ImpreciseJD jd, Date& date, double* secInDay=nullptr);


/// @brief 将日期转换为当天00:00的简约儒略日
AST_CORE_CAPI int aDateToMJD(const Date& date);

/// @brief 将当天00:00的简约儒略日转换为当天日期
AST_CORE_CAPI void aMJDToDate(int mjd, Date& date);

/// @brief 将日期转换为字符串表示
AST_CORE_API std::string aDateToString(const Date& date);

/// @brief 日期
class Date
{
public:

    /// @brief 月份，从1开始计数（1表示一月）
    enum EMonth{
        eJAN = 1, eFEB, eMAR, eAPR, eMAY, eJUN,
        eJUL = 7, eAUG, eSEP, eOCT, eNOV, eDEC
    };

    /// @brief 星期几，从0开始计数（0表示星期日）
    enum EWeekDay{
        eSUN = 0, eMON, eTUE, eWED, eTHU, eFRI, eSAT
    };

public:
    /// @brief 根据儒略日数创建日期对象
    static Date FromJDAtNoon(int jd){
        Date date;
        aJDToDateAtNoon(jd, date);
        return date;
    }
    
    /// @brief 根据年份、月份、日期创建日期对象
    static Date FromYMD(int year, int month, int day){
        return {year, month, day};
    }
    /// @brief 根据年份、一年中的天数创建日期对象
    static Date FromYD(int year, int days){
        Date date;
        aYDToDate(year, days, date);
        return date;
    }

public:
    int year() const{return year_;}
    int& year(){return year_;}
    int month() const{return month_;}
    int& month(){return month_;}
    int day() const{return day_;}
    int& day(){return day_;}

    void setYear(int year){year_ = year;}
    void setMonth(int month){month_ = month;}
    void setDay(int day){day_ = day;}
public:
    /// @brief 获取月份的完整英文名称
    const char* monthFullName() const{
        return aMoonFullName(month_);
    }
    /// @brief 获取月份的简写英文名称
    const char* monthShortName() const{
        return aMoonShortName(month_);
    }
    /// @brief 获取星期几的完整英文名称
    const char* weekDayFullName() const{
        return aWeekDayFullName(dayOfWeek());
    }
    /// @brief 获取星期几的简写英文名称
    const char* weekDayShortName() const{
        return aWeekDayShortName(dayOfWeek());
    }
    /// @brief 获取日期是这一周的第几天
    int dayOfWeek() const{
        return aDayOfWeek(*this);
    }

    /// @brief 获取日期是这一年的第几天，从1开始计数
    /// @return 第几天
    int dayOfYear() const{
        return aDayOfYear(*this);
    }

    /// @brief 判断是否为闰年
    bool isLeapYear() const{
        return aIsLeapYear(year_);
    }

    /// @brief 获取该月的天数
    int dayInMonth() const{
        return aDaysInMonthByYear(month_, year_);
    }

    /// @brief 将日期转换为儒略日数
    int toJDAtNoon() const{
        return aDateToJDAtNoon(*this);
    }
    /// @brief 将日期转换为当天00:00的儒略日数
    double toJD() const{
        return aDateToJD(*this);
    }
    
    /// @brief 将日期转换为当天00:00的简约儒略日数
    double toMJD() const{
        return aDateToMJD(*this);
    }

    /// @brief 将当天中午12点的儒略日数转换为日期
    void fromJDAtNoon(int jd){
        aJDToDateAtNoon(jd, *this);
    }
    /// @brief 将年份、月份、日期转换为日期
    void fromYMD(int year, int month, int day){
        year_ = year;
        month_ = month;
        day_ = day;
    }
    /// @brief 将年份和一年中的天数转换为日期
    void fromYD(int year, int days){
        aYDToDate(year, days, *this);
    }
    /// @brief 将日期转换为年份和一年中的天数
    void toYD(int& year, int& days) const{
        aDateToYD(*this, year, days);
    }

    /// @brief 规范化日期对象，将日期调整为有效日期
    void normalize(){
        aDateNormalize(*this);
    }
    
    /// @brief 返回规范化后的日期对象副本
    Date normalized() const {
        Date d = *this;
        aDateNormalize(d);
        return d;
    }
public:
    std::string toString() const{
        return aDateToString(*this);
    }
public:
    int year_;          ///< 年
    int month_;         ///< 月
    int day_;           ///< 日
};

/// @brief 将当天00:00的简约儒略日转换为当天日期
A_ALWAYS_INLINE Date aMJDToDate(int mjd){
    Date date;
    aMJDToDate(mjd, date);
    return date;
}


A_ALWAYS_INLINE int aDateToMJD(int year, int month, int day){
    return aDateToMJD({year, month, day});
}

/*! @} */


AST_NAMESPACE_END
 
