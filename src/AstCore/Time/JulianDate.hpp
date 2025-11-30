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
 
AST_NAMESPACE_BEGIN

class JulianDate;
class Date;
class Time;
class DateTime;

typedef class JulianDate ModJulianDate;


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
    static JulianDate FromDaySecond(double day, double second){
        return JulianDate{day, second};
    }

    static JulianDate FromDayTwoPart(double dayp1, double dayp2){
        double day = dayp1;
        double second = dayp2 * 86400.0;
        return JulianDate::FromDaySecond(day, second);
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
    double day() const{return day_;}
    double& day(){return day_;}
    double second() const {return second_;}
    double& second(){return second_;}
    void setDay(double day){day_ = day;}
    void setSecond(double sec){second_ = sec;}

    void getDaySecond(double& day, double& second) const{
        day = day_;
        second = second_;
    }
    void setDaySecond(double day, double second){
        day_ = day;
        second_ = second;
    }
public:
    double dayPart1() const{
        return day_;
    }
    void setDayPart1(double dp1){
        day_ = dp1;
    }
    double dayPart2() const{
        return second_ / 86400.0;
    }
    void setDayPart2(double dp2){
        second_ = dp2 * 86400.0;
    }
    void getDayTwoPart(double& dayp1, double& dayp2) const{
        dayp1 = day_;
        dayp2 = second_ / 86400.0;
    }
    void setDayTwoPart(double dayp1, double dayp2){
        day_ = dayp1;
        second_ = dayp2 * 86400.0;
    }
public:
    double day_;     // 天数部分 day part of julian date
    double second_;  // 秒数部分 second part of julia date
};
 
AST_NAMESPACE_END
 
