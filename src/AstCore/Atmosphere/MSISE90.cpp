///
/// @file      MSISE90.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-25
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

#include "MSISE90.hpp"
#include "AstUtil/Math.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstWeather/MSIS_Vers.h"

AST_NAMESPACE_BEGIN

using namespace MSIS_Vers;



MSISE90::MSISE90(Frame* frame, BodyShape* bodyShape, double f107Daily, double f107Average, double ap)
    : MSISBase(frame, bodyShape, f107Daily, f107Average, ap)
{
    msis90init(this->msis());
    int sv[26]{
        0,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1
    };
    tselec(this->msis().csw, sv);
}


double MSISE90::getDensity(const TimePoint &tp, const Vector3d &posInBodyFixed) const
{
    double lat, lon, alt;
    this->getGeodetic(posInBodyFixed, lat, lon, alt);
    int dayOfYear;
    double secOfDay, lst;
    this->getMSISParam(tp, lon, dayOfYear, secOfDay, lst);

    alt /= 1e3;
	lat = rad2deg(lat);
	lon = rad2deg(lon);
    double f107A = this->F107Average_;
    double f107 = this->F107Daily_;

	int mass = 48;
    std::array<double, 8> ap{};
    ap[1] = ap_;
    std::array<double, 10> d{};
    std::array<double, 3> t{};
	gtd6(
        this->msis(), this->lpoly(), this->fit(), this->lsqv(), 
        dayOfYear, secOfDay, alt, lat, lon, lst,
        f107A, f107, ap.data(), mass, d.data(), t.data()
    );
    return d[6];
}

AST_NAMESPACE_END


