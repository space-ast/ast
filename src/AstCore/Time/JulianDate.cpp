/// @file      JulianDate.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      21.11.2025
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
 

#include "JulianDate.hpp" 
#include "AstCore/Date.hpp"
#include "AstCore/Time.hpp"
#include "AstCore/DateTime.hpp"
#include "AstUtil/Constants.h"

AST_NAMESPACE_BEGIN


void aJDToMJD(const JulianDate& jd, ModJulianDate& mjd)
{
	mjd = jd;
	mjd.setDay(jd.day() - kMJDRefEpoch);
}


void aMJDToJD(const ModJulianDate& mjd, JulianDate& jd)
{
	jd = mjd;
	jd.setDay(mjd.day() + kMJDRefEpoch);
}


double aJDToMJD_Imprecise(const JulianDate& jd)
{
	return (jd.dayPart1() - kMJDRefEpoch) + jd.dayPart2();
}



ImpreciseJD aMJDToJD_Imprecise(ImpreciseMJD mjd)
{
	return mjd + kMJDRefEpoch;
}

ImpreciseJD aMJDToJD_Imprecise(const ModJulianDate& mjd)
{
	return (mjd.dayPart1() + kMJDRefEpoch) + mjd.dayPart2();
}


ImpreciseMJD aJDToMJD_Imprecise(ImpreciseJD jd)
{
	return jd - kMJDRefEpoch;
}

void aDateTimeToJD(const DateTime &dttm, JulianDate &jd)
{
	double day = dttm.date().toJD();
	double sec = dttm.time().totalSecond();
	jd.setDaySecond(day, sec);
}

void aJDToDateTime(const JulianDate& jd, DateTime& dttm)
{
	double secInDay;
	aJDToDate(jd.day(), dttm.date(), &secInDay);
	secInDay += jd.second();
	aTimeFromTotalSecond(secInDay, dttm.time());
}


AST_NAMESPACE_END
 
