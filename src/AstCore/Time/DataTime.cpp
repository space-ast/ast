#include "DataTime.hpp"
#include "time.h"


AST_NAMESPACE_BEGIN


DateTime DateTime::FromTm(const tm* time)
{
    int year = time->tm_year + 1900;
    int month = time->tm_mon + 1;
    int day = time->tm_mday;
    int hour = time->tm_hour;
    int minute = time->tm_min;
    double second = static_cast<double>(time->tm_sec);
    return DateTime{year, month, day, hour, minute, second};
}

DateTime DateTime::FromTimeTLocal(time_t time)
{
    return FromTm(localtime(&time));
}

DateTime DateTime::FromTimeTUTC(time_t time)
{
    return FromTm(gmtime(&time));
}

void aCurrentDateTimeLocal(DateTime& dt)
{
    time_t t = time(NULL);
	dt = DateTime::FromTimeTLocal(t);
}

void aCurrentDateTimeUTC(DateTime &dt)
{
    time_t t = time(NULL);
	dt = DateTime::FromTimeTUTC(t);
}

void aDateTimeNormalize(DateTime& dt)
{
    aTimeNormalize(dt.time());
    // @todo 
}


void aDateTimeNormalizeUTC(DateTime& dt)
{
    // @todo 

}
void aDateTimeNormalizeLocal(DateTime& dt, int timezone)
{
    // @todo 
}

void aDateTimeNormalizeBJT(DateTime& dt)
{
    // @todo 
}

void aDateTimeAddYears(DateTime& dt, int years)
{
    dt.year() += years;
    dt.normalize();
}
void aDateTimeAddMonths(DateTime& dt, int months)
{
    dt.month() += months;
    dt.normalize();
}
void aDateTimeAddDays(DateTime& dt, int days)
{
    dt.day() += days;
    dt.normalize();
}
void aDateTimeAddHours(DateTime& dt, int hours)
{
    dt.hour() += hours;
    dt.normalize();
}
void aDateTimeAddMinutes(DateTime& dt, int minutes)
{
    dt.minute() += minutes;
    dt.normalize();
}
void aDateTimeAddSeconds(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalize();
}
void aDateTimeAddSecondsUTC(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalizeUTC();
}
void aDateTimeAddSecondsLocal(DateTime& dt, double seconds, int timezone)
{
    dt.second() += seconds;
    dt.normalizeLocal(timezone);
}
void aDateTimeAddSecondsBJT(DateTime& dt, double seconds)
{
    dt.second() += seconds;
    dt.normalizeBJT();
}


AST_NAMESPACE_END