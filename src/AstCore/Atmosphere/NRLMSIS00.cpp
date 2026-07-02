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
#include "AstCore/TimePoint.hpp"
#include "AstWeather/nrlmsise-00.hpp"
#include "AstWeather/MSIS_Vers.h"

AST_NAMESPACE_BEGIN

#ifdef _AST_USE_MSIS_VERS_FOR_NRLMSIS00

using namespace MSIS_Vers;

NRLMSIS00::NRLMSIS00(Frame* frame, BodyShape* bodyShape, double f107Daily, double f107Average, double ap)
    : NRLMSIS00(frame, bodyShape, NewConstantSpaceWeather(f107Daily, f107Average, ap))
{
	
}


NRLMSIS00::NRLMSIS00(Frame* frame, BodyShape* bodyShape, SpaceWeatherProvider* spaceWeather)
    : MSISBase(frame, bodyShape, spaceWeather)
{
	msis00init(this->msis());
	int sv[26]{
        0,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1
    };
    tselec(this->msis().csw, sv);
}


double NRLMSIS00::getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const
{
    MSISParam param;
    this->getMSISParam(tp, posInBodyFixed, param);

    std::array<double, 10> d{};
    std::array<double, 3> t{};
	gtd7d(
        this->msis(), this->lpoly(), this->fit(), this->lsqv(),
		param.dayOfYear, param.secOfDay, param.alt, param.lat, param.lon, param.lst,
        param.f107A, param.f107, param.ap.data()-1, param.mass, d.data(), t.data()
    );
    return d[6];
}

#else

NRLMSIS00::NRLMSIS00(Frame* ecf, BodyShape* bodyShape, double f107Daily, double f107Average, double ap)
    : AtmosphereBase(ecf, bodyShape, f107Daily, f107Average, ap)
{
    new (&storage_) NRLMSISE();
    static_assert(sizeof(storage_) >= sizeof(NRLMSISE), "storage_ size must not be less than NRLMSISE");
}

NRLMSIS00::~NRLMSIS00()
{
    this->nrlmsise().~NRLMSISE();
}


double NRLMSIS00::getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const
{
    assert(bodyShape_ != nullptr);
    assert(frame_ != nullptr);
    
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
		double lat, lon, alt;
		this->getGeodetic(posInBodyFixed, lat, lon, alt);

		int dayOfYear;
		double secOfDay, lst;
		this->getMSISParam(tp, lon, dayOfYear, secOfDay, lst);

		nrlmsise_input  input;
		input.year = 2000; 
		input.doy = dayOfYear;
		input.sec = secOfDay;  // UT
		input.alt = alt/1e3; 
		input.g_lat = rad2deg(lat);  
		input.g_long = rad2deg(lon);
		input.lst = lst;
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

#endif

AST_NAMESPACE_END
