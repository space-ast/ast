///
/// @file      Scenario.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-07
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

#include "Scenario.hpp"
#include "AstCore/EventTime.hpp"
#include "AstCore/EventInterval.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/EventTimeExplicit.hpp"
#include "AstCore/EventIntervalExplicit.hpp"

AST_NAMESPACE_BEGIN



Scenario::Scenario()
{

}

Scenario::~Scenario()
{

}

errc_t Scenario::getEpoch(TimePoint &epoch) const
{
    if(epoch_)
    {
        return epoch_->getTime(epoch);
    }
    return eErrorNullPtr;
}

void Scenario::setEpoch(EventTime *epoch)
{
    if(epoch)
        epoch_ = epoch;
}

void Scenario::setEpoch(const TimePoint &epoch)
{
    epoch_ = EventTimeExplicit::New(epoch);
}

errc_t Scenario::getInterval(TimeInterval& interval) const
{
    if(interval_)
    {
        return interval_->getInterval(interval);
    }
    return eErrorNullPtr;
}

void Scenario::setInterval(EventInterval *interval)
{
    if(interval)
        interval_ = interval;
}

void Scenario::setInterval(const TimeInterval &interval)
{
    interval_ = EventIntervalExplicit::New(interval);
}

void Scenario::setPrimaryBody(CelestialBody* body)
{
    if(body)
        primaryBody_ = body;
}

AST_NAMESPACE_END

