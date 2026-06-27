///
/// @file      MSISBase.cpp
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

#include "MSISBase.hpp"
#include "AstWeather/MSIS_Vers.h"
#include "AstWeather/GeomagneticIndex.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

using namespace MSIS_Vers;



class MSISBase::WorkSpace
{
public:
    msistype msis_{};
    lpolytype lpoly_{};
    fittype fit_{};
    lsqvtype lsqv_{};
};

MSISBase::MSISBase(Frame* frame, BodyShape* bodyShape, double f107Daily, double f107Average, double ap)
    : MSISBase(frame, bodyShape, NewConstantSpaceWeather(f107Daily, f107Average, ap))
{

}

MSISBase::MSISBase(Frame *frame, BodyShape *bodyShape, SpaceWeatherProvider *spaceWeather)
    : AtmosphereBase(frame, bodyShape)
    , spaceWeather_(spaceWeather)
{
    static_assert(sizeof(storage_) >= sizeof(MSISBase::WorkSpace), "storage_ size must not be less than WorkSpace");
    new (&storage_) WorkSpace;
}

MSISBase::~MSISBase()
{
    workSpace().~WorkSpace();
}

void MSISBase::setSpaceWeatherProvider(SpaceWeatherProvider *spaceWeather)
{
    if(spaceWeather != nullptr)
        spaceWeather_ = spaceWeather;
}

void MSISBase::setConstantSpaceWeather(double f107Daily, double f107Average, double ap)
{
    spaceWeather_ = NewConstantSpaceWeather(f107Daily, f107Average, ap);
}

SpaceWeatherProvider *MSISBase::NewConstantSpaceWeather(double f107Daily, double f107Average, double ap)
{
    return new ConstantSpaceWeather(f107Daily, f107Average, ap, aApToKp(ap));
}

msistype& MSISBase::msis() const { return workSpace().msis_; }
lpolytype& MSISBase::lpoly() const { return workSpace().lpoly_; }
fittype& MSISBase::fit() const { return workSpace().fit_; }
lsqvtype& MSISBase::lsqv() const { return workSpace().lsqv_; }


void MSISBase::getMSISParam(const TimePoint &tp, double lon, int &dayOfYear, double &secOfDay, double &lst)
{
	DateTime dateTime;
    aTimePointToUTC(tp, dateTime);
    dayOfYear = dateTime.dayOfYear();
    secOfDay = dateTime.secOfDay();
	lst = secOfDay / 3600 + lon * kRadToTimeHour;
}

void MSISBase::getSpaceWeather(const TimePoint &tp, double &f107, double &f107Average, double &ap) const
{
    if (spaceWeather_)
    {
        f107 = spaceWeather_->getF10p7Daily(tp);
        f107Average = spaceWeather_->getF10p7Average(tp);
        ap = spaceWeather_->getApDaily(tp);
    }
    else
    {
        aWarning("space weather provider is not set");
        f107 = 0.0;
        f107Average = 0.0;
        ap = 0.0;
    }
}

AST_NAMESPACE_END
