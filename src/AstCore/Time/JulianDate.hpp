///
/// @file      JulianDate.hpp
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
#include "Duration.hpp"
 
AST_NAMESPACE_BEGIN

class JulianDate;
class Date;
class Time;
class DateTime;



/// @brief 将儒略日转换为简约儒略日
AST_CORE_CAPI void aJDToMJD(const JulianDate& jd, ModJulianDate& mjd);


/// @brief 将简约儒略日转换为儒略日
AST_CORE_CAPI void aMJDToJD(const ModJulianDate& mjd, JulianDate& jd);


/// @brief 将儒略日转换为简约儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API double aJDToMJD_Imprecise(const JulianDate& jd);


/// @brief 将简约儒略日Modified Julian Date（MJD）转换为儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseMJD aJDToMJD_Imprecise(ImpreciseJD jd);


/// @brief 将简约儒略日Modified Julian Date（MJD）转换为不精确的儒略日
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseJD aMJDToJD_Imprecise(const ModJulianDate& mjd);


/// @brief 将儒略日转换为简约儒略日Modified Julian Date（MJD）
/// @warning 注意用单个double表示的时间精度不高
AST_CORE_API ImpreciseJD aMJDToJD_Imprecise(ImpreciseMJD mjd);


/// @brief 将日期时间转换为儒略日
AST_CORE_CAPI void aDateTimeToJD(const DateTime& dttm, JulianDate& jd);


/// @brief 将儒略日转换为日期时间
AST_CORE_CAPI void aJDToDateTime(const JulianDate& jd, DateTime& dttm);


/// @brief 儒略日
/// @details 儒略日（Julian Date）是一种用于表示时间的方法，常用于天文学和计算机科学中。
class JulianDate
{
public:
    /// @brief 根据不精确的天数创建儒略日对象
    static JulianDate FromImpreciseDay(double JD){
        double day = static_cast<int>(JD);
        double second = (JD - day) * 86400.0;
        return JulianDate::FromDaySecond(day, second);
    }
    /// @brief 根据天数和秒数创建儒略日对象
    static JulianDate FromDaySecond(int day, double second){
        return JulianDate{day, second};
    }
    /// @brief 根据日期时间创建儒略日对象
    static JulianDate FromDateTime(const DateTime& dttm)
    {
        JulianDate jd;
        aDateTimeToJD(dttm, jd);
        return jd;
    }
public:
    /// @brief 获取不精确的天数
    /// @return 不精确的天数
    double impreciseDay() const{return day_ + second_ /86400.;}
    
    /// @brief 设置不精确的天数
    void setImpreciseDay(double jd){
        double day = static_cast<int>(jd);
        double second = (jd - day) * 86400.0;
        setDaySecond(day, second);
    }
public:
    int day() const{return day_;}
    int& day(){return day_;}
    double second() const {return second_;}
    double& second(){return second_;}
    void setDay(double day){day_ = day;}
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
        return second_ / 86400.0;
    }
    /// @brief 设置小数部分的日数
    void setDayFractional(double df){
        second_ = df * 86400.0;
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
    int    day_;     // 天数部分 day part of julian date
    double second_;  // 秒数部分 second part of julia date
};
 
AST_NAMESPACE_END
 
