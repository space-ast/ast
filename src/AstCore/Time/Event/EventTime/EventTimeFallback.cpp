///
/// @file      EventTimeFallback.cpp
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

#include "EventTimeFallback.hpp"
#include "AstUtil/Logger.hpp"
#include "AstCore/EventTimeExplicit.hpp"

AST_NAMESPACE_BEGIN

EventTimeFallback *EventTimeFallback::New(EventTime* primary, EventTime* fallback)
{
    return new EventTimeFallback(primary, fallback);
}

EventTimeFallback *EventTimeFallback::New(EventTime *primary, const TimePoint &fallback)
{
    return new EventTimeFallback(primary, fallback);
}

SharedPtr<EventTimeFallback> EventTimeFallback::MakeShared(EventTime* primary, EventTime* fallback)
{
    return new EventTimeFallback(primary, fallback);
}

SharedPtr<EventTimeFallback> EventTimeFallback::MakeShared(EventTime *primary, const TimePoint &fallback)
{
    return new EventTimeFallback(primary, fallback);
}

EventTimeFallback::EventTimeFallback(EventTime* primary, EventTime* fallback)
    : primary_(primary)
    , fallback_(fallback)
{
}

EventTimeFallback::EventTimeFallback(EventTime *primary, const TimePoint &fallback)
    : primary_(primary)
    , fallback_(EventTimeExplicit::New(fallback))
{
}

errc_t EventTimeFallback::getTime(TimePoint &tp) const
{
    if (primary_)
    {
        errc_t err = primary_->getTime(tp);
        if (err == 0)
        {
            return err;
        }
    }
    
    if (fallback_)
    {
        return fallback_->getTime(tp);
    }
    
    return -1;
}

void EventTimeFallback::setPrimary(EventTime* primary)
{
    if(primary == this){
        aError(_("primary 不能是自身"));
        return;
    }
    primary_ = primary;
}

void EventTimeFallback::setFallback(EventTime* fallback)
{
    if(fallback == this){
        aError(_("fallback 不能是自身"));
        return;
    }
    fallback_ = fallback;
}

AST_NAMESPACE_END