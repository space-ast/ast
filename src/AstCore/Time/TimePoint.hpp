///
/// @file      TimePoint.hpp
/// @brief     绝对时间点
/// @details   ~
/// @author    axel
/// @date      15.11.2025
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
#include "Duration.hpp"             // for LongDuration
#include "AstCore/Constants.h"      // for kJ2000XXXX
#include "AstCore/JulianDate.hpp"    
#include "AstCore/DateTime.hpp"
#include <stdint.h>                 // for int64_t

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Time
    @{
*/



class DateTime;

class LongDuration;
class ShortDuration;
class DaySecDuration;

class TAIScale;
class TDBScale;
class TTScale;
class UTCScale;

class TimePoint;

constexpr static double kTimePointDefaultFormatPrecision = 6;


// --------------------
// 时间点与时间系统的转换
// --------------------

/// @brief 将时间点转换为儒略日数（协调世界时 UTC）
AST_CORE_API void aTimePointToUTC(const TimePoint& time, JulianDate& jdUTC);

/// @brief 将时间点转换为儒略日数（地球时TT）
AST_CORE_API void aTimePointToTT(const TimePoint& time, JulianDate& jdTT);

/// @brief 将时间点转换为儒略日数（质心动力学时TDB）
AST_CORE_API void aTimePointToTDB(const TimePoint& time, JulianDate& jdTDB);

/// @brief 将时间点转换为儒略日数（原子时 TAI）
AST_CORE_API void aTimePointToTAI(const TimePoint& time, JulianDate& jdTAI);

/// @brief 将时间点转换为儒略日数（世界时 UT1）
AST_CORE_API void aTimePointToUT1(const TimePoint& time, JulianDate& jdUT1);

/// @brief 将时间点转换为日期时间（协调世界时 UTC）
AST_CORE_API void aTimePointToUTC(const TimePoint& time, DateTime& dttmUTC);

/// @brief 将时间点转换为日期时间（地球时TT）
AST_CORE_API void aTimePointToTT(const TimePoint& time, DateTime& dttmTT);

/// @brief 将时间点转换为日期时间（质心动力学时TDB）
AST_CORE_API void aTimePointToTDB(const TimePoint& time, DateTime& dttmTDB);

/// @brief 将时间点转换为日期时间（原子时 TAI）
AST_CORE_API void aTimePointToTAI(const TimePoint& time, DateTime& dttmTAI);

/// @brief 将时间点转换为日期时间（世界时 UT1）
AST_CORE_API void aTimePointToUT1(const TimePoint& time, DateTime& dttmUT1);



// ----------------
// 时间点格式化与解析
// ----------------


/// @brief 将时间点格式化为字符串
AST_CORE_CAPI err_t aTimePointFormat(const TimePoint& time, std::string& str, int precision = kTimePointDefaultFormatPrecision);

/// @brief 从字符串解析时间点
AST_CORE_CAPI err_t aTimePointParse(StringView str, TimePoint& time);


/// @brief 绝对时间点
class TimePoint
{
public:
    /// @brief 从UTC日期时间创建时间点
    AST_CORE_API
    static TimePoint FromUTC(const DateTime& dttmUTC);

    /// @brief 从UTC日期时间创建时间点
    AST_CORE_API
    static TimePoint FromUTC(int year, int month, int day, int hour, int minute, double second);

    /// @brief 从原子时TAI创建时间点
    AST_CORE_API 
    static TimePoint FromTAI(const JulianDate& jdTAI);

    /// @brief 从地球时TT创建时间点
    AST_CORE_API
    static TimePoint FromTT(const JulianDate& jdTT);

    /// @brief J2000.0 TT 时间点
    static TimePoint J2000TT(){
        return {0, -kTTMinusTAI};
    }

    /// @brief 从字符串解析时间点
    /// @param str 时间点字符串
    /// @return TimePoint 解析后的时间点对象
    static TimePoint Parse(StringView str){
        TimePoint time;
        aTimePointParse(str, time);
        return time;
    }

public:
    /// @brief 时间点的整数秒数部分
    int64_t integerPart() const { return duration_.integer_; }

    /// @brief 时间点的小数秒数部分
    double fractionalPart() const { return duration_.fractional_; }
public:
    /// @brief 计算时间点与 J2000  epoch 的时间差（天）
    double daysFromJ2000TT() const{
        return ((double)integerPart() / kSecondsPerDay) + (fractionalPart() + kTTMinusTAI) / kSecondsPerDay;
    }

    /// @brief 计算时间点与 J2000  epoch 的时间差（儒略世纪）
    double julianCenturyFromJ2000TT() const{
        return daysFromJ2000TT() / kDaysPerJulianCentury;
    }

    /// @brief 将时间点转换为儒略日数（地球时TT）
    void toTT(JulianDate& jdTT) const{
        aTimePointToTT(*this, jdTT);
    }
    /// @brief 将时间点转换为儒略日数（地球时TT）
    JulianDate toTT() const{
        JulianDate jdTT;
        toTT(jdTT);
        return jdTT;
    }
    /// @brief 将时间点转换为儒略日数（质心动力学时TDB）
    void toTDB(JulianDate& jdTDB) const{
        aTimePointToTDB(*this, jdTDB);
    }
    /// @brief 将时间点转换为儒略日数（质心动力学时TDB）
    JulianDate toTDB() const{
        JulianDate jdTDB;
        toTDB(jdTDB);
        return jdTDB;
    }
    /// @brief 将时间点转换为儒略日数（原子时 TAI）
    void toTAI(JulianDate& jdTAI) const{
        aTimePointToTAI(*this, jdTAI);
    }
    /// @brief 将时间点转换为儒略日数（原子时 TAI）
    JulianDate toTAI() const{
        JulianDate jdTAI;
        toTAI(jdTAI);
        return jdTAI;
    }
    /// @brief 计算与另一个时间点的时间差（秒数）
    double durationFrom(const TimePoint& other) const{
        return duration_ - other.duration_;
    }
    /// @brief 时间点减法运算符
    /// @param other 另一个时间点
    /// @return 两个时间点的时间差（秒数）
    double operator-(const TimePoint& other) const{
        return durationFrom(other);
    }
    /// @brief 时间点加法运算符
    /// @param second 秒数
    /// @return 新的时间点
    TimePoint operator+(double second) const{
        return {integerPart(), fractionalPart() + second};
    }

    /// @brief 将时间点格式化为字符串
    /// @param precision 格式化精度（秒数小数位数）
    /// @return 格式化后的字符串
    std::string toString(int precision = kTimePointDefaultFormatPrecision) const
    {
        std::string str;
        aTimePointFormat(*this, str, precision);
        return str;
    }
protected:
    static TimePoint FromIntegerFractional(int64_t integer, double fractional);
public:
    LongDuration    duration_;
};


// typedef TimePoint<TDBScale, LongDuration> TDBTime;
// typedef TimePoint<TTScale, LongDuration> TTTime;
// typedef TimePoint<UTCScale, LongDuration> UTCTime;
// typedef TimePoint<TAIScale, LongDuration> TAITime;

// typedef TAITime AbsTime;
// typedef TAITime AstTime;


/*! @} */
  
AST_NAMESPACE_END
