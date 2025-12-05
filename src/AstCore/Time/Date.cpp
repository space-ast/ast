/// @file      Date.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      30.11.2025
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
 
 

#include "Date.hpp"
#include "AstUtil/Constants.h"
#include <assert.h>
#include <cmath>       // for floor


AST_NAMESPACE_BEGIN


const char* aMoonFullName(int mon)
{
  static constexpr const char* full_name_list[] = {
      "January", "February", "March",     "April",   "May",      "June",
      "July",    "August",   "September", "October", "November", "December"};
  return mon >= 1 && mon <= 12 ? full_name_list[mon - 1] : "?";
}
const char* aMoonShortName(int mon)
{
  static constexpr const char* short_name_list[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };
  return mon >= 1 && mon <= 12 ? short_name_list[mon - 1] : "???";
}


const char* aWeekDayFullName(int wday) 
{
  static constexpr const char* full_name_list[] = {
      "Sunday",   "Monday", "Tuesday", "Wednesday",
      "Thursday", "Friday", "Saturday" };
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
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool aIsLeapYear(int year)
{
	return _aIsLeapYear(year);
}

int aDaysInMonthByYear(int month, int year)
{
	static const int daysInMonth[] = {
			31, 28, 31, 30, 31, 30,
			31, 31, 30, 31, 30, 31
	};

	if (month < 1 || month > 12) {
		return 0;
	}
	else {
		assert(month >= 1 && month <= 12);
	}

	int days = daysInMonth[month - 1];
	if (month == 2 && aIsLeapYear(year)) {
		days = 29;
	}
	return days;
}

int aDaysInMonthByLeap(int month, bool isLeapYear)
{
	static const int daysInMonth[] = {
			31, 28, 31, 30, 31, 30,
			31, 31, 30, 31, 30, 31
	};

	if (month < 1 || month > 12) {
		return 0;
	}
	else {
		assert(month >= 1 && month <= 12);
	}

	int days = daysInMonth[month - 1];
	if (month == 2 && isLeapYear) {
		days = 29;
	}
	return days;
}


int aDayOfYear(const Date& date)
{
	static const int daysToMonth[] = {
		0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
	};
	int month = date.month();
	int day = date.day();
	int year = date.year();

	int dayOfYear = daysToMonth[month - 1] + day;
	if (month > 2 && _aIsLeapYear(year)) {
		dayOfYear += 1;
	}

	return dayOfYear;
}

int aDayOfWeek(const Date& date)
{
#if 1
    int jd = aDateToJDAtNoon(date);
    return (jd + 1) % 7; // 0: Sunday, 1: Monday, ..., 6: Saturday
#else
	// 计算星期几（0=周日，1=周一，...，6=周六）
	// 使用Zeller同余公式

	int month = date.month();
	int year = date.year();
	int day = date.day();

	if (month < 3) {
		month += 12;
		year -= 1;
	}

	int century = year / 100;
	int yearOfCentury = year % 100;

	int weekday = (day + (13 * (month + 1)) / 5 + yearOfCentury +
		yearOfCentury / 4 + century / 4 + 5 * century) % 7;

	// 调整结果为0=周日，1=周一，...，6=周六
	weekday = (weekday + 6) % 7;

	return weekday;
#endif
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

		dayofmonth = aDaysInMonthByYear(date.month(), date.year());
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
		dayinmonth = aDaysInMonthByLeap(m, is_leap);
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




int aDateToJDAtNoon(const Date& date)
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


void aJDToDateAtNoon(int J, Date& date)
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

ImpreciseJD aDateToJD(const Date &date)
{
	return aDateToJDAtNoon(date) - 0.5;
}

void aJDToDate(ImpreciseJD jd, Date& date, double* secInDay)
{
	// 计算日期部分
	int jdAtNoon = (int)floor(jd + 0.5);
	aJDToDateAtNoon(jdAtNoon, date);

	// 计算秒数部分
	if (secInDay != nullptr) {
		*secInDay = (jd - jdAtNoon) * 86400.0 + 43200.0;
	}
}


int aDateToMJD(const Date& date)
{
    return aDateToJDAtNoon(date) - 2400001;
}

void aMJDToDate(int mjd, Date& date)
{
    return aJDToDateAtNoon(mjd + 2400001, date);
}



AST_NAMESPACE_END