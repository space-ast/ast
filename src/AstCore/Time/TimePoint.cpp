/// @file      Time.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      4.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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


#include "TimePoint.hpp"
#include "AstCore/TimeSystem.hpp"


AST_NAMESPACE_BEGIN

namespace{
    /// @brief 零时刻点的儒略日数（地球时TT）
    static const JulianDate epochTTJulianDate{ (int)kJ2000Epoch, kTTMinusTAI};

    /// @brief 零时刻点的儒略日数（原子时 TAI）
    static const JulianDate epochTAIJulianDate{ (int)kJ2000Epoch, 0};

    /// @brief 零时刻点的日期时间（地球时TT）
    static const DateTime epochTTDateTime{2000,1,1,12,0,kTTMinusTAI};

    /// @brief 零时刻点的日期时间（原子时 TAI）
    static const DateTime epochTAIDateTime{2000,1,1,12,0,0};
}

void aTimePointToUTC(const TimePoint &time, JulianDate &jdUTC)
{
    aTimePointToTAI(time, jdUTC);
    aTAIToUTC(jdUTC, jdUTC);
}

void aTimePointToTT(const TimePoint &time, JulianDate &jdTT)
{
    jdTT = epochTTJulianDate;
    int day1 = time.integerPart()/86400;
    int day2 = time.fractionalPart()/86400;
    double sec1 = time.integerPart() - (day1)*86400LL;
    double sec2 = time.fractionalPart() - (day2)*86400LL;
    jdTT.day() += day1 + day2;
    jdTT.second() += sec1 + sec2;
}


void aTimePointToTAI(const TimePoint &time, JulianDate &jdTAI)
{
    jdTAI = epochTAIJulianDate;
    int day1 = time.integerPart()/86400;
    int day2 = time.fractionalPart()/86400;
    double sec1 = time.integerPart() - (day1)*86400LL;
    double sec2 = time.fractionalPart() - (day2)*86400LL;
    jdTAI.day() += day1 + day2;
    jdTAI.second() += sec1 + sec2;
}

void aTimePointToUT1(const TimePoint &time, JulianDate &jdUT1)
{
    aTimePointToUTC(time, jdUT1);
    aUTCToUT1(jdUT1, jdUT1);
}

void aTimePointToUTC(const TimePoint &time, DateTime &dttmUTC)
{
    JulianDate jdUTC;
    aTimePointToUTC(time, jdUTC);
    aJDToDateTime(jdUTC, dttmUTC);
}

void aTimePointToTDB(const TimePoint& time, JulianDate& jdTDB)
{
    aTimePointToTT(time, jdTDB);
    aTTToTDB(jdTDB, jdTDB);
}

TimePoint TimePoint::FromUTC(const DateTime& dttmUTC)
{
    JulianDate jdUTC;
    JulianDate jdTAI;
    aDateTimeToJD(dttmUTC, jdUTC);
    aUTCToTAI(jdUTC, jdTAI);
    return TimePoint::FromTAI(jdTAI);
}

TimePoint TimePoint::FromUTC(int year, int month, int day, int hour, int minute, double second)
{
    return TimePoint::FromUTC({year, month, day, hour, minute, second});
}

TimePoint TimePoint::FromTAI(const JulianDate& jdTAI)
{
    auto epochTAI = epochTAIJulianDate;
    auto duration = jdTAI - epochTAI;
    return {duration.day() * 86400LL, duration.second()};
}

TimePoint TimePoint::FromTT(const JulianDate &jdTT)
{
    auto epochTT = epochTTJulianDate;
    auto duration = jdTT - epochTT;
    return {duration.day() * 86400LL, duration.second()};
}

TimePoint TimePoint::FromIntegerFractional(int64_t integer, double fractional)
{
    return {integer, fractional};
}

AST_NAMESPACE_END


