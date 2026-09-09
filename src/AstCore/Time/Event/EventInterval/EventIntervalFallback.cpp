///
/// @file      EventIntervalFallback.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "EventIntervalFallback.hpp"
#include "EventIntervalExplicit.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

EventIntervalFallback *EventIntervalFallback::New(EventInterval* primary, EventInterval* fallback)
{
    return new EventIntervalFallback(primary, fallback);
}

EventIntervalFallback *EventIntervalFallback::New(EventInterval *primary, const TimeInterval &fallback)
{
    return new EventIntervalFallback(primary, fallback);
}

SharedPtr<EventIntervalFallback> EventIntervalFallback::MakeShared(EventInterval* primary, EventInterval* fallback)
{
    return new EventIntervalFallback(primary, fallback);
}

EventIntervalFallback::EventIntervalFallback(EventInterval* primary, EventInterval* fallback)
    : primary_(primary)
    , fallback_(fallback)
{
}

EventIntervalFallback::EventIntervalFallback(EventInterval *primary, const TimeInterval &fallback)
    : primary_(primary)
    , fallback_(EventIntervalExplicit::New(fallback))
{
}

errc_t EventIntervalFallback::getInterval(TimeInterval &interval) const
{
    if (primary_)
    {
        errc_t err = primary_->getInterval(interval);
        if (err == 0)
        {
            return err;
        }
    }
    
    if (fallback_)
    {
        return fallback_->getInterval(interval);
    }
    
    return -1;
}

void EventIntervalFallback::setPrimary(EventInterval* primary)
{
    if(primary == this){
        aError(_("primary 不能是自身"));
        return;
    }
    primary_ = primary;
}

void EventIntervalFallback::setFallback(EventInterval* fallback)
{
    if(fallback == this){
        aError(_("fallback 不能是自身"));
        return;
    }
    fallback_ = fallback;
}

AST_NAMESPACE_END