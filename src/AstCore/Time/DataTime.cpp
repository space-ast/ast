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


AST_NAMESPACE_END