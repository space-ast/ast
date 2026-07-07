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
#include "AstUtil/Literals.hpp"

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

void MSISBase::setUseDailyAp(bool useDailyAp)
{
    useDailyAp_ = useDailyAp;
    this->msis().csw.sw[9] = useDailyAp ? 1 : -1;
}

SpaceWeatherProvider *MSISBase::NewConstantSpaceWeather(double f107Daily, double f107Average, double ap)
{
    return new ConstantSpaceWeather(f107Daily, f107Average, ap, aApToKp(ap));
}

msistype& MSISBase::msis() const { return workSpace().msis_; }
lpolytype& MSISBase::lpoly() const { return workSpace().lpoly_; }
fittype& MSISBase::fit() const { return workSpace().fit_; }
lsqvtype& MSISBase::lsqv() const { return workSpace().lsqv_; }


void MSISBase::getTimeParam(const TimePoint &tp, double lon, int &dayOfYear, double &secOfDay, double &lst)
{
	DateTime dateTime;
    aTimePointToUTC(tp, dateTime);
    dayOfYear = dateTime.dayOfYear();
    secOfDay = dateTime.secOfDay();
	lst = secOfDay / 3600 + lon * kRadToTimeHour;
}

void MSISBase::getMSISParam(const TimePoint& tp, const Vector3d& posInBodyFixed, MSISParam &param) const
{
    this->getGeodetic(posInBodyFixed, param.lat, param.lon, param.alt);
    this->getTimeParam(tp, param.lon, param.dayOfYear, param.secOfDay, param.lst);
    this->getSpaceWeather(tp, param.f107, param.f107A, param.ap);
    param.alt /= 1e3;
    param.lat = rad2deg(param.lat);
    param.lon = rad2deg(param.lon);
    static_assert(param.mass == 48, "param.mass must be 48");
}

void MSISBase::getSpaceWeather(const TimePoint &tp, double &f107, double &f107Average, ApArray &aparray) const
{
    if (spaceWeather_)
    {
        // f107 - daily f10.7 flux for previous day
        // 根据注释 msis 需要 space weather 数据的前一天，所以减去 1 天
        f107 = spaceWeather_->getF10p7Daily(tp - 1_day);
        f107Average = spaceWeather_->getF10p7Average(tp);
        double apDaily = spaceWeather_->getApDaily(tp);
        if(this->useDailyAp_)
        {
            assert(this->msis().csw.sw[9] == 1.0);
            aparray.fill(apDaily);
        }
        else
        {
            assert(this->msis().csw.sw[9] == -1.0);
            // 获取3小时间隔Ap列表（最多向前回溯20个块=60小时）
            double ap3Hourly[20];
            int nBlocks = spaceWeather_->getAp3HourlyList(tp, ap3Hourly, 20);
            if (nBlocks >= 20)  // 有完整的3小时间隔数据
            {
                /*
                ap - magnetic index[daily] or when sw[9] = -1.0 :
                - array containing:
                    [1] daily ap
                    [2] 3 hr ap index for current time
                    [3] 3 hr ap index for 3 hrs before current time
                    [4] 3 hr ap index for 6 hrs before current time
                    [5] 3 hr ap index for 9 hrs before current time
                    [6] average of eight 3 hr ap indicies from 12 to 33 hrs prior
                            to current time
                    [7] average of eight 3 hr ap indicies from 36 to 59 hrs prior
                            to current time
                */
                aparray[0] = apDaily;
                aparray[1] = ap3Hourly[0];   // 当前块
                aparray[2] = ap3Hourly[1];   // 3小时前
                aparray[3] = ap3Hourly[2];   // 6小时前
                aparray[4] = ap3Hourly[3];   // 9小时前
                double sum = 0.0;
                for (int i = 4; i < 12; ++i) sum += ap3Hourly[i];
                aparray[5] = sum / 8.0;
                sum = 0.0;
                for (int i = 12; i < 20; ++i) sum += ap3Hourly[i];
                aparray[6] = sum / 8.0;
            }
            else
            {
                aparray.fill(apDaily);
            }
        }
    }
    else
    {
        aWarning("space weather provider is not set");
        f107 = 0.0;
        f107Average = 0.0;
        aparray.fill(0.0);
    }
}

AST_NAMESPACE_END
