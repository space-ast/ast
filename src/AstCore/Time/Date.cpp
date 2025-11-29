#include "Date.hpp"
#include <assert.h>
#include <math.h>       // for floor


AST_NAMESPACE_BEGIN


const char* aMoonFullName(int mon)
{
  static constexpr const char* full_name_list[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  return mon >= 0 && mon <= 11 ? full_name_list[mon] : "?";
}
const char* aMoonShortName(int mon)
{
  static constexpr const char* short_name_list[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };
  return mon >= 0 && mon <= 11 ? short_name_list[mon] : "???";
}


const char* aWeekDayFullName(int wday) 
{
  static constexpr const char* full_name_list[] = {
      "Sunday",   "Monday", "Tuesday", "Wednesday",
      "Thursday", "Friday", "Saturday"};
  return wday >= 0 && wday <= 6 ? full_name_list[wday] : "?";
}


const char* aWeekDayShortName(int wday) 
{
  static constexpr const char* short_name_list[] = {"Sun", "Mon", "Tue", "Wed",
                                                    "Thu", "Fri", "Sat"};
  return wday >= 0 && wday <= 6 ? short_name_list[wday] : "???";
}

A_ALWAYS_INLINE
bool _aIsLeapYear(int year)
{
	if ((year % 400 == 0) || (year % 100 != 0 && year % 4 == 0))
		return true;
	else
		return false;
}

bool aIsLeapYear(int year)
{
	return _aIsLeapYear(year);
}

int aDayInMonth(int month, bool is_leap_year)
{
	// 28: 2
	// 30: 4, 6, 9, 11
	// 31: 1, 3, 5, 7, 8, 10, 12
	assert(month >= 1 && month <= 12);
	if (month == 2) {
		if (is_leap_year) {
			return 29;
		}
		else {
			return 28;
		}
	}
	else {
		if (month <= 7 && month % 2 == 1 || month >= 8 && month % 2 == 0) {
			return 31;
		}
		else {
			return 30;
		}
	}
}


int aDayOfYear(const Date& date)
{
    int year = date.year();
    int month = date.month();
    int dayofyear = date.day();
	assert(month >= 1 && month <= 12);

    bool is_leap_year = _aIsLeapYear(year);
	for (int m = 1; m < month; m++) {
		dayofyear += aDayInMonth(m, is_leap_year);
	}
	return dayofyear;
}

int aDayOfWeek(const Date& date)
{
    int jd = aDateToJD(date);
    return (jd + 1) % 7; // 0: Sunday, 1: Monday, ..., 6: Saturday
}


int aDateToJD(const Date& date)
{
    int Y = date.year();
    int M = date.month();
    int D = date.day();

    int Mi = (M - 14) / 12; // int类型，整数运算
	int jd = (1461 * (Y + 4800 + Mi) / 4 )
       + ( 367 * (M - 2 - Mi * 12) / 12 )
       - (   3 * ((Y + 4900 + Mi) / 100) / 4)
	   - 32075 + D;
    return jd;
}


void aJDToDate(int J, Date& date)
{
	int N = 4 * (J + 68569) / 146097;
	int L1 = J + 68569 - ((N * 146097 + 3) / 4);
	int Y1 = (4000 * (L1 + 1) / 1461001);
	int L2 = L1 - (1461 * Y1 / 4) + 31;
	int M1 = (80 * L2 / 2447);
	int L3 = (M1 / 11);

	int day = L2 - (2447 * M1 / 80);
	int month = M1 + 2 - 12 * L3;
	int year = (100 * (N - 49) + Y1 + L3);


    date.year() = year;
    date.month() = month;
    date.day() = day;
}


void aDateToYD(const Date& date, int& year, int& days)
{
	year = date.year();
	days = aDayOfYear(date);
}

void aDateNormalize(Date &date)
{
	int dayofmonth;
#ifndef NDEBUG
	int niter = 0;
#endif // !NDEBUG

	while (1) {
#ifndef NDEBUG
		niter++;
		assert(niter <= 100000);
#endif // !NDEBUG

		int ryear = (int)floor((date.month() - 1) / 12.);
		date.month() -= ryear * 12;
		date.year() += ryear;

		dayofmonth = aDayInMonth(date.month(), _aIsLeapYear(date.year()));
		if (date.day() < 1) {
			date.month()--;
			date.day() += dayofmonth;
		}
		else if (date.day() >= dayofmonth + 1) {
			date.month()++;
			date.day() -= dayofmonth;
		}
		else {
			// date.day(): [1, dayofmonth+1)
			break;
		}
	}
}

void aYDToDate(int year, int days, Date& date)
{
	//assert(days > 0 && days <= 367); 
	bool is_leap = aIsLeapYear(year);
	if (days < 1) {
		aYDToDate(year - 1, days + (is_leap?366:365), date);
		return;
	}
	int dayinmonth;
	for (int m = 1; m <= 12; m++) {
		dayinmonth = aDayInMonth(m, is_leap);
		if (days < dayinmonth + 1) {
			date.month() = m;
			date.day() = (int)floor(days);
			date.year() = year;
			return;
		}
		days -= dayinmonth;
	}
	aYDToDate(year + 1, days, date);
}


AST_NAMESPACE_END