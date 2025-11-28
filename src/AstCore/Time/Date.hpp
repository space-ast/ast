///
/// @file      Date.hpp
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
 
AST_NAMESPACE_BEGIN
 
class Date;

/// @brief 获取月份的完整英文名称
AST_CORE_CAPI const char* aMoonFullName(int month);

/// @brief 获取月份的简写英文名称
AST_CORE_CAPI const char* aMoonShortName(int month);

/// @brief 获取星期几的完整英文名称
AST_CORE_CAPI const char* aWeekDayFullName(int wday);

/// @brief 获取星期几的简写英文名称
AST_CORE_CAPI const char* aWeekDayShortName(int wday);

/// @brief 判断是否为闰年
AST_CORE_CAPI bool aIsLeapYear(int year);

/// @brief 获取某月的天数
AST_CORE_CAPI int aDayInMonth(int month, bool is_leap_year);

/// @brief 获取某年某月的天数
A_ALWAYS_INLINE
int aDayInMonth(int month, int year)
{
    return aDayInMonth(month, aIsLeapYear(year));
}

/// @brief 获取日期是这一年的第几天，从1开始计数
AST_CORE_CAPI int aDayOfYear(const Date& date);

/// @brief 将日期转换为儒略日数
AST_CORE_CAPI int aDateToJD(const Date& date);

/// @brief 将儒略日数转换为日期
AST_CORE_CAPI void aJDToDate(int jd, Date& date);


/// @brief 将年份和一年中的天数转换为日期
AST_CORE_CAPI void aYDToDate(int year, int days, Date& date);

/// @brief 将日期转换为年份和一年中的天数
AST_CORE_CAPI void aDateToYD(const Date& date, int& year, int& days);


/// @brief 日期
class Date
{
public:
    /// @brief 根据儒略日数创建日期对象
    static Date FromJD(int jd){
        Date date;
        aJDToDate(jd, date);
        return date;
    }
    
    /// @brief 根据年份、月份、日期创建日期对象
    static Date FromYMD(int year, int month, int day){
        return {year, month, day};
    }
    /// @brief 根据年份、一年中的天数创建日期对象
    static Date FromYD(int year, double days){
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
public:
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
        return aDayInMonth(month_, year_);
    }

    /// @brief 将日期转换为儒略日数
    int toJD() const{
        return aDateToJD(*this);
    }

    /// @brief 将儒略日数转换为日期
    void fromJD(int jd){
        aJDToDate(jd, *this);
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
public:
    int year_;          ///< 年
    int month_;         ///< 月
    int day_;           ///< 日
};


AST_NAMESPACE_END
 
