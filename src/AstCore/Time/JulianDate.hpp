///
/// @file      JulianDate.hpp
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
#include "Duration.hpp"
#include "AstUtil/Constants.hpp"
 
AST_NAMESPACE_BEGIN

/*!
    @addtogroup Time
    @{
*/


class JulianDate;
class Date;
class Time;
class DateTime;



/// @brief 将儒略日转换为简约儒略日
AST_CORE_CAPI void aJDToMJD(const JulianDate& jd, ModJulianDate& mjd);


/// @brief 将简约儒略日转换为儒略日
AST_CORE_CAPI void aMJDToJD(const ModJulianDate& mjd, JulianDate& jd);


/// @brief 将不精确的儒略日 Julian Date（MJD）转换为不精确的简约儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseMJD aJDToMJD_Imprecise(ImpreciseJD jd);


/// @brief 将儒略日转换为不精确的简约儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseMJD aJDToMJD_Imprecise(const JulianDate& jd);


/// @brief 将不精确的简约儒略日Modified Julian Date（MJD）转换为不精确的儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseJD aMJDToJD_Imprecise(ImpreciseMJD mjd);


/// @brief 将简约儒略日Modified Julian Date（MJD）转换为不精确的儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseJD aMJDToJD_Imprecise(const ModJulianDate& mjd);


/// @brief 将不精确的简约儒略日Modified Julian Date（MJD）转换为儒略日
AST_CORE_API void aMJDToJD_Imprecise(ImpreciseMJD mjd, JulianDate& jd);


/// @brief 将日期时间转换为儒略日
AST_CORE_CAPI void aDateTimeToJD(const DateTime& dttm, JulianDate& jd);


/// @brief 将儒略日转换为日期时间
AST_CORE_CAPI void aJDToDateTime(const JulianDate& jd, DateTime& dttm);


/// @brief 儒略日
/// @details 儒略日（Julian Date）是一种用于表示时间的方法，常用于天文学和计算机科学中。
/// 在 JulianDate 类的实现中，将儒略日表示为整数天 + 小数秒的形式，保证数值精度
class JulianDate
{
public:
    /// @brief 根据不精确的天数创建儒略日
    /// @param day 天数 [day] (单个double表示的天数精度不高)
    static JulianDate FromImpreciseDay(double day){
        int d = static_cast<int>(day);
        double second = (day - d) * 86400.0;
        return JulianDate::FromDaySecond(d, second);
    }

    /// @brief 根据天数(分两部分)创建儒略日
    /// @param day1 天数第1部分 [day]
    /// @param day2 天数第2部分 [day]
    static JulianDate FromDays(double day1, double day2)
    {
        int d1 = static_cast<int>(day1);
        int d2 = static_cast<int>(day2);
        double second = ((day1 - d1) + (day2 - d2)) * 86400.0;
        return JulianDate::FromDaySecond(d1 + d2, second);
    }

    /// @brief 根据天数和秒数创建儒略日
    /// @param day 天数 [day]
    /// @param second 秒数 [second]
    static JulianDate FromDaySecond(int day, double second){
        return JulianDate{day, second};
    }

    /// @brief 根据天数和秒数创建儒略日
    /// @param day 天数 [day] 
    /// @param second 秒数 [second]
    static JulianDate FromDaySecond(double day, double second)
    {
        int d = static_cast<int>(day);
        double s = (day - d) * 86400.0;
        return JulianDate{d, second + s};
    }

    /// @brief 根据日期时间创建儒略日
    static JulianDate FromDateTime(const DateTime& dttm)
    {
        JulianDate jd;
        aDateTimeToJD(dttm, jd);
        return jd;
    }

    /// @brief 获取 J2000.0 历元的儒略日
    static JulianDate J2000(){
        return JulianDate::FromDaySecond(static_cast<int>(kJ2000Epoch), 0.0);
    }

    /// @brief 根据日期时间创建儒略日对象
    AST_CORE_API
    static JulianDate FromDateTime(int year, int month, int day, int hour, int minute, double second);
public:
    /// @brief 获取不精确的天数
    /// @return 不精确的天数
    double impreciseDay() const{return day_ + second_ /86400.;}
    
    /// @brief 设置不精确的天数
    void setImpreciseDay(double jd){
        int day = static_cast<int>(jd);
        double second = (jd - day) * 86400.0;
        setDaySecond(day, second);
    }
public:
    int day() const{return day_;}
    int& day(){return day_;}
    double second() const {return second_;}
    double& second(){return second_;}
    void setDay(int day){day_ = day;}
    void setSecond(double sec){second_ = sec;}

    /// @brief 获取天数和秒数
    void getDaySecond(int& day, double& second) const{
        day = day_;
        second = second_;
    }
    /// @brief 设置天数和秒数
    void setDaySecond(int day, double second){
        day_ = day;
        second_ = second;
    }
public:
    /// @brief 获取小数部分的日数
    double dayFractional() const{
        return second_ / kSecondsPerDay;
    }
    /// @brief 设置小数部分的日数
    void setDayFractional(double df){
        second_ = df * kSecondsPerDay;
    }
public:
    /// @brief 计算儒略日与 J2000.0 历元的时间差（儒略世纪）
    double julianCenturyFromJ2000() const{
        return daysFromJ2000() / kDaysPerJulianCentury;
    }
    /// @brief 计算儒略日与 J2000.0 历元的天数差
    double daysFromJ2000() const{
        return daysFromJulianDate(kJ2000Epoch);
    }
    /// @brief 计算儒略日与 J2000.0 历元的时间差（秒）
    double secondsFromJ2000() const{
        return (day_ - kJ2000Epoch) * kSecondsPerDay + second_;
    }
    /// @brief 计算儒略日与另一个儒略日的天数差
    double daysFromJulianDate(double jd) const
    {
        return ((day_ - jd) + dayFractional());
    }
    /// @brief 计算儒略日与另一个儒略日的时间差（秒）
    double secondsFromJulianDate(double jd) const
    {
        return (day_ - jd) * kSecondsPerDay + second_;
    }
    
    double daysFrom(const JulianDate& other) const{
        return (*this - other).totalDay();
    }
public:
    JulianDate& operator += (double sec)
    {
        this->second() += sec;  // 适用于任何时间尺度，包括考核闰秒的和不考虑闰秒的
        return *this;
    }
    JulianDate& operator -= (double sec)
    {
        this->second() -= sec;  // 适用于任何时间尺度，包括考核闰秒的和不考虑闰秒的
        return *this;
    }
    JulianDate operator + (double sec) const
    {
        JulianDate jd{ *this };
        jd += sec;
        return jd;
    }
    JulianDate operator - (double sec) const
    {
        JulianDate jd{ *this };
        jd -= sec;
        return jd;
    }
    DaySecDuration operator - (const JulianDate& other) const
    {
        return {day() - other.day(), second() - other.second()};
    }
public:
    /// @brief 计算儒略日偏移后的新儒略日
    /// @param second 偏移秒数
    /// @return 偏移后的新儒略日
    JulianDate shiftedBySecond(double second) const{
        return JulianDate::FromDaySecond(day_, second_ + second);
    }
public:
    int    day_;     // 天数部分 day part of julian date
    double second_;  // 秒数部分 second part of julia date
};



/*! @} */
 
AST_NAMESPACE_END
 
