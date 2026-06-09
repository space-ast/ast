///
/// @file      NRLMSIS00.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-09
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "NRLMSIS00.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstWeather/nrlmsise-00.hpp"

AST_NAMESPACE_BEGIN

NRLMSIS00::NRLMSIS00(Frame* ecf, BodyShape* bodyShape, double f107Daily, double f107Average, double ap)
    : earthFixedFrame_{ecf}
    , bodyShape_{bodyShape}
    , F107Daily_{f107Daily}
    , F107Average_{f107Average}
    , ap_{ap}
{
    new (&storage_) class nrlmsise();
}

NRLMSIS00::~NRLMSIS00()
{
    static_assert(sizeof(storage_) == sizeof(class nrlmsise), "storage_ size must be same as nrlmsise");
    this->nrlmsise().~nrlmsise();
}


double NRLMSIS00::getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const
{
    assert(bodyShape_ != nullptr);
    assert(earthFixedFrame_ != nullptr);

    #define USE_METER 1
		// for correctness with parallel, with some loss of efficiency for single thread
		auto& nrlmsise00 = this->nrlmsise(); 
		nrlmsise_flags flags{
			// switches
			{
				USE_METER,	1, 1, 1, 1, 1, 1, 1,
				1,			1, 1, 1, 1, 1, 1, 1,
				1,			1, 1, 1, 1, 1, 1, 1
			},
			// sw
			{},
			// swc
			{}
		};
        DateTime dateTime;
        aTimePointToUTC(tp, dateTime);
        auto dayOfYear = dateTime.dayOfYear();
        auto secOfDay = dateTime.secOfDay();
        GeodeticPoint geodeticPoint;
        bodyShape_->transform(posInBodyFixed, geodeticPoint);
        double lat = geodeticPoint.latitude();
        double lon = geodeticPoint.longitude();
        double alt = geodeticPoint.altitude();

		nrlmsise_input  input;
		input.year = 2000; 
		input.doy = dayOfYear;
		input.sec = secOfDay;  // UT
		input.alt = alt/1e3; 
		input.g_lat = rad2deg(lat);  
		input.g_long = rad2deg(lon);
		input.lst = secOfDay / 3600 + input.g_long / 15;
		if (input.lst > 12.0)
			input.lst -=  24.0;
		if (input.lst < -12.0)
			input.lst += 24.0;
		input.f107A = F107Average_;
		input.f107 = F107Daily_;
		// if (useDailyAp)  // flags.switches[9] ==  1
        {	
			input.ap = ap_;
			input.ap_a = NULL;
			// constrained variable range
			if (input.ap < 0.0)
				input.ap = 0.0;
			if (input.ap > 1000.0)
				input.ap = 1000;
		}
		// else {				// flags.switches[9] == -1
		// 	flags.switches[9]   = -1;
		// 	input.ap_a = (ap_array*) & ap;
		// }
		// constrained variable range
		if (input.f107A < 30.0)
			input.f107A = 30;
		if (input.f107A > 10000.0)
			input.f107A = 10000;

		if (input.f107 < 30.0)
			input.f107 = 30;

		if (input.f107 > 10000.0)
			input.f107 = 10000;

		nrlmsise_output output;
		nrlmsise00.gtd7d(&input, &flags,  &output);
	#if USE_METER
		return output.d[5];
	#else
		return output.d[5] * 1e3;  // g/cm^3 -> kg/m^3    *=(100^3 / 1000)
	#endif
}


AST_NAMESPACE_END
