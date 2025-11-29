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


/// @brief 儒略日
/// @details 儒略日（Julian Date）是一种用于表示时间的方法，常用于天文学和计算机科学中。
///          它以公元前4713年1月1日12:00（儒略历）为起点，计算从该时刻起经过的天数。
class JulianDate
{
public:
    /// @brief 根据不精确的儒略日数创建儒略日对象
    static JulianDate FromImpreciseJD(double JD){
        double day = static_cast<int>(JD);
        double second = (JD - day) * 86400.0;
        return JulianDate::FromDaySecond(day, second);
    }
    /// @brief 根据天数和秒数创建儒略日对象
    static JulianDate FromDaySecond(double day, double second){
        return JulianDate{day, second};
    }
    static JulianDate FromTwoPartJD(double jd1, double jd2){
        double day = jd1;
        double second = jd2 * 86400.0;
        return JulianDate::FromDaySecond(day, second);
    }

public:
    /// @brief 获取不精确的儒略日数
    /// @return 不精确的儒略日数
    double impreciseJD() const{return day_ + second_ /86400.;}
    
    /// @brief 设置不精确的儒略日数
    void setImpreciseJD(double jd){
        double day = static_cast<int>(jd);
        double second = (jd - day) * 86400.0;
        setDaySecond(day, second);
    }
public:
    double day() const{return day_;}
    double& day(){return day_;}
    double second() const {return second_;}
    double& second(){return second_;}
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
    void getJDTwoPart(double& jd1, double& jd2) const{
        jd1 = day_;
        jd2 = second_ / 86400.0;
    }
    void setJDTwoPart(double jd1, double jd2){
        day_ = jd1;
        second_ = jd2 * 86400.0;
    }
public:
    double day_;     // 天数部分 day part of julian date
    double second_;  // 秒数部分 second part of julia date
};
 
AST_NAMESPACE_END
 
