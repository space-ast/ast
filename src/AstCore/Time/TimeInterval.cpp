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
    if (step <= 0.0 || duration() <= 0.0){
        aError("invalid step (%f) or duration (%f)", step, duration());
        return eErrorInvalidParam;
    }
    ptrdiff_t nnodes = static_cast<ptrdiff_t>(std::ceil(duration() / step) + 1);
    times.resize(nnodes);
    double offset = getStart() - epoch;
    
    for(ptrdiff_t i = 0; i < nnodes-1; i++){
        times[i] = offset + i * step;
    }
    times[nnodes-1] = getStop() - epoch;
    return eNoError;
}

errc_t TimeInterval::discretize(double step, std::vector<TimePoint> &times) const
{
    if (step <= 0.0 || duration() <= 0.0){
        aError("invalid step (%f) or duration (%f)", step, duration());
        return eErrorInvalidParam;
    }
    ptrdiff_t nnodes = static_cast<ptrdiff_t>(std::ceil(duration() / step) + 1);
    times.resize(nnodes);
    TimePoint start = getStart();
    
    for(ptrdiff_t i = 0; i < nnodes-1; i++){
        times[i] = start + i * step;
    }
    times[nnodes-1] = getStop();
    return eNoError;
}

errc_t TimeInterval::discretize(double step, TimeList& times) const
{
    double duration = this->duration();
    if (step <= 0.0 || duration <= 0.0){
        aError("invalid step (%f) or duration (%f)", step, duration);
        return eErrorInvalidParam;
    }
    ptrdiff_t nnodes = static_cast<ptrdiff_t>(std::ceil(duration / step) + 1);
    times.seconds().resize(nnodes);
    times.setEpoch(getStart());

    for(ptrdiff_t i = 0; i < nnodes-1; i++){
        times.seconds()[i] = (i * step);
    }
    times.seconds()[nnodes-1] = duration;
    return eNoError;
}


AST_NAMESPACE_END