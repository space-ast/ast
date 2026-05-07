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
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

namespace{
    /// @brief 零时刻点的儒略日数（地球时TT）
    static const JulianDate epochTTJulianDate{ (int)kJ2000Epoch, kTTMinusTAI};

    /// @brief 零时刻点的儒略日数（原子时 TAI）
    static const JulianDate epochTAIJulianDate{ (int)kJ2000Epoch, 0};

    /// @brief 零时刻点的日期时间（地球时TT）
    // static const DateTime epochTTDateTime{2000,1,1,12,0,kTTMinusTAI};

    /// @brief 零时刻点的日期时间（原子时 TAI）
    // static const DateTime epochTAIDateTime{2000,1,1,12,0,0};
}

void aTimePointToUTC(const TimePoint &time, JulianDate &jdUTC)
{
    aTimePointToTAI(time, jdUTC);
    aTAIToUTC(jdUTC, jdUTC);
}

void aTimePointToTT(const TimePoint &time, JulianDate &jdTT)
{
    jdTT = epochTTJulianDate;
    int day1 = static_cast<int>(time.integerPart()/86400U);
    int day2 = static_cast<int>(time.fractionalPart()/86400U);
    double sec1 = static_cast<double>(time.integerPart() - (day1)*86400LL);
    double sec2 = static_cast<double>(time.fractionalPart() - (day2)*86400LL);
    jdTT.day() += day1 + day2;
    jdTT.second() += sec1 + sec2;
}


void aTimePointToTAI(const TimePoint &time, JulianDate &jdTAI)
{
    jdTAI = epochTAIJulianDate;
    int day1 = static_cast<int>(time.integerPart()/86400U);
    int day2 = static_cast<int>(time.fractionalPart()/86400U);
    double sec1 = static_cast<double>(time.integerPart() - (day1)*86400LL);
    double sec2 = static_cast<double>(time.fractionalPart() - (day2)*86400LL);
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
    JulianDate jdUTC{};
    aTimePointToUTC(time, jdUTC);
    aJDToDateTime(jdUTC, dttmUTC);
    dttmUTC.normalizeUTC();  // 在这里确保UTC时间是标准化的
}

errc_t aTimePointFormat(const TimePoint &time, std::string &str, int precision)
{
    DateTime utc{};
    aTimePointToUTC(time, utc);
    errc_t err = aDateTimeFormatDefault(utc, str, precision);
    str += " UTC";
    return err;
}

void processTimeSystemm(StringView& str, StringView &timeSystem)
{
    
}

void processQuote(StringView& str)
{
    if(str.back() == '"' && str[0] == '"')
    {
        str = str.substr(1, str.size() - 2);
    }
}

errc_t aTimePointParse(StringView str, TimePoint &time)
{
    str = aStripAsciiWhitespace(str);
    if(str.empty())
        return eErrorInvalidParam;

    StringView timeSystem; // 时间系统信息

    if(str.back() == '"' && str[0] == '"')
    {
        str = str.substr(1, str.size() - 2);
    }

    if(std::isalpha(static_cast<unsigned char>(str.back())))
    {
        size_t lastSpace = str.find_last_of(' ');
        if(lastSpace != std::string::npos)
            timeSystem = str.substr(lastSpace + 1);
        str = aStripTrailingAsciiWhitespace(str.substr(0, lastSpace));
        if(str.back() == '"' && str[0] == '"')
        {
            str = str.substr(1, str.size() - 2);
        }
    }

    DateTime dttm;
    errc_t rc = aDateTimeParseAny(str, dttm);
    if(rc != eNoError)
        return rc;
    if(timeSystem.empty())
    {
        time = TimePoint::FromUTC(dttm);
    }
    else if(timeSystem == "TAI" || timeSystem == "TAIG")
    {
        time = TimePoint::FromTAI(dttm);
    }
    else if(timeSystem == "UTC" || timeSystem == "UTCG")
    {
        time = TimePoint::FromUTC(dttm);
    }
    else if(timeSystem == "TT")
    {
        time = TimePoint::FromTT(dttm);
    }
    else
    {
        // @todo: 支持解析不同的时间系统
        aError("unsupported time system: '%.*s'", timeSystem.size(), timeSystem.data());
        return eErrorInvalidParam;
    }
    return eNoError;
}

void aTimePointToTDB(const TimePoint& time, JulianDate& jdTDB)
{
    aTimePointToTT(time, jdTDB);
    aTTToTDB(jdTDB, jdTDB);
}

TimePoint TimePoint::Default()
{
    return Epoch();
}


TimePoint TimePoint::CurrentTime()
{
    return FromUTC(aCurrentDateTimeUTC());
}

TimePoint TimePoint::TodayUTC()
{
    DateTime t{};
    aTodayDateTimeUTC(t);
    return FromUTC(t);
}

TimePoint TimePoint::TomorrowUTC()
{
    DateTime t{};
    aTomorrowDateTimeUTC(t);
    return FromUTC(t);
}

TimePoint TimePoint::FromUTC(int year, int month, int day, int hour, int minute, double second)
{
    return TimePoint::FromUTC({year, month, day, hour, minute, second});
}

TimePoint TimePoint::FromUTC(const DateTime &dttmUTC)
{
    JulianDate jdUTC{};
    JulianDate jdTAI{};
    aDateTimeToJD(dttmUTC, jdUTC);
    aUTCToTAI(jdUTC, jdTAI);
    return TimePoint::FromTAI(jdTAI);
}

TimePoint TimePoint::FromTAI(const DateTime& dttmTAI)
{
    JulianDate jdTAI{};
    aDateTimeToJD(dttmTAI, jdTAI);
    return TimePoint::FromTAI(jdTAI);
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

TimePoint TimePoint::FromTT(const DateTime &dttmTT)
{
    JulianDate jdTT{};
    aDateTimeToJD(dttmTT, jdTT);
    return FromTT(jdTT);
}

TimePoint TimePoint::FromTDB(const JulianDate &jdTDB)
{
    return TimePoint::FromTT(aTDBToTT(jdTDB));
}

TimePoint TimePoint::FromImpreciseJDTT(double jdTT)
{
    return TimePoint::FromTT(JulianDate::FromImpreciseDay(jdTT));
}

TimePoint TimePoint::shiftedBySecondInTDB(double second) const
{
    return TimePoint::FromTDB(toTDB().shiftedBySecond(second));
}

TimePoint TimePoint::FromIntegerFractional(int64_t integer, double fractional)
{
    return {integer, fractional};
}

TimePoint TimePoint::Parse(StringView str)
{
    TimePoint time;
    errc_t rc = aTimePointParse(str, time);
    if(rc != eNoError){
        aError("failed to parse TimePoint '%.*s'", str.size(), str.data());
    }
    return time;
}

AST_NAMESPACE_END