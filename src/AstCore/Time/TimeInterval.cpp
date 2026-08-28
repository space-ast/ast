///
/// @file      TimeInterval.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-02-04
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "TimeInterval.hpp"
#include "TimeList.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

errc_t aTimeIntervalFormat(const TimeInterval &interval, std::string &strStart, std::string &strStop, int precision)
{
    TimePoint start{}, stop{};
    start = interval.getStart();
    stop = interval.getStop();
    errc_t rc = aTimePointFormat(start, strStart, precision);
    rc |= aTimePointFormat(stop, strStop, precision);
    return rc;
}

errc_t aTimeIntervalParse(StringView strStart, StringView strStop, TimeInterval &interval)
{
    TimePoint start{}, stop{};
    errc_t rc = aTimePointParse(strStart, start);
    if(rc != 0){
        return rc;
    }
    rc = aTimePointParse(strStop, stop);
    if(rc != 0){
        return rc;
    }
    interval = TimeInterval(start, stop);
    return 0;
}

errc_t TimeInterval::discretize(const TimePoint &epoch, double step, std::vector<double> &times) const
{
    double dur = this->duration();
    if (step > 0 && dur >= 0)
    {
        ptrdiff_t nnodes = static_cast<ptrdiff_t>(aDiscretizedCount(dur, step));
        times.resize(nnodes);
        double start = getStart() - epoch;
        
        for(ptrdiff_t i = 0; i < nnodes-1; i++){
            times[i] = start + i * step;
        }
        times[nnodes-1] = getStop() - epoch;
        return eNoError;

    }
    aError("invalid step (%f) or empty interval", step);
    return eErrorInvalidParam;
    
}

errc_t TimeInterval::discretize(double step, std::vector<TimePoint> &times) const
{
    double dur = this->duration();
    if (step > 0 && dur >= 0){
        ptrdiff_t nnodes = static_cast<ptrdiff_t>(aDiscretizedCount(dur, step));
        times.resize(nnodes);
        const TimePoint& start = getStart();
        
        for(ptrdiff_t i = 0; i < nnodes-1; i++){
            times[i] = start + i * step;
        }
        times[nnodes-1] = getStop();
        return eNoError;

    }
    aError("invalid step (%f) or empty interval", step);
    return eErrorInvalidParam;
    
}

errc_t TimeInterval::discretize(double step, TimeList& times) const
{
    double dur = this->duration();
    if (step > 0.0 && dur >= 0){
        ptrdiff_t nnodes = static_cast<ptrdiff_t>(aDiscretizedCount(dur, step));
        times.seconds().resize(nnodes);
        times.setEpoch(getStart());

        for(ptrdiff_t i = 0; i < nnodes-1; i++){
            times.seconds()[i] = (i * step);
        }
        times.seconds()[nnodes-1] = dur;
        return eNoError;
    }
    aError("invalid step (%f) or empty interval", step);
    return eErrorInvalidParam;
}



TimeInterval& TimeInterval::unite(const TimeInterval &other)
{
    const TimePoint& thisStart  = this->start();
    const TimePoint& thisStop   = this->stop();
    const TimePoint& otherStart = other.start();
    const TimePoint& otherStop  = other.stop();

    if(otherStart < thisStart)
    {
        this->setStart(otherStart);
    }
    if(otherStop > thisStop)
    {
        this->setStop(otherStop);
    }
    return *this;
}

TimeInterval TimeInterval::united(const TimeInterval &other) const
{
    TimeInterval result = *this;
    result.unite(other);
    return result;
}

TimeInterval& TimeInterval::intersect(const TimeInterval &other)
{
    // 任一为空/NaN（isEmpty() 为 NaN 安全的弱序否定）→ 交集必为空
    if (isEmpty() || other.isEmpty())
    {
        *this = Empty();
        return *this;
    }

    const TimePoint& thisStart  = this->start();
    const TimePoint& thisStop   = this->stop();
    const TimePoint& otherStart = other.start();
    const TimePoint& otherStop  = other.stop();

    if(otherStart > thisStart)
    {
        this->setStart(otherStart);
    }
    if(otherStop < thisStop)
    {
        this->setStop(otherStop);
    }


    return *this;
}

TimeInterval TimeInterval::intersected(const TimeInterval &other) const
{
    TimeInterval result = *this;
    result.intersect(other);
    return result;
}

template<> struct propagate_nan::allow_efficient_minus<TimePoint> : std::true_type {};

bool TimeInterval::intersects(const TimeInterval &other) const
{
    // 与 intersected() 一致：非空交集（含相切产生的点区间）才算相交；
    auto& start = (propagate_nan::max)(start_, other.start());
    auto& stop  = (propagate_nan::min)(stop_, other.stop());
    return start <= stop;
}


AST_NAMESPACE_END