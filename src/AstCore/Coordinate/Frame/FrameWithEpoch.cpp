///
/// @file      FrameWithEpoch.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-15
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

#include "FrameWithEpoch.hpp"



AST_NAMESPACE_BEGIN

PFrameWithEpoch FrameWithEpoch::New(Point *origin, Axes *axes)
{
    return new FrameWithEpoch(origin, axes);
}

PFrameWithEpoch FrameWithEpoch::New(Point *origin, Axes *axes, EventTime *epoch)
{
    return new FrameWithEpoch(origin, axes, epoch);
}

HFrameWithEpoch FrameWithEpoch::MakeShared(Point *origin, Axes *axes)
{
    return new FrameWithEpoch(origin, axes);
}

HFrameWithEpoch FrameWithEpoch::MakeShared(Point *origin, Axes *axes, EventTime *epoch)
{
    return new FrameWithEpoch(origin, axes, epoch);
}

FrameWithEpoch::FrameWithEpoch()
    : origin_(), frozenAxes_{new AxesFrozenAtEventTime()}, sourceAxes_()
{
}

FrameWithEpoch::FrameWithEpoch(Point *origin, Axes *axes)
    : origin_{origin}, frozenAxes_{new AxesFrozenAtEventTime(axes, nullptr, nullptr)}, sourceAxes_{axes}, useEpoch_{false}
{
}

FrameWithEpoch::FrameWithEpoch(Point *origin, Axes *axes, EventTime *epoch)
    : origin_{origin}
    , frozenAxes_{new AxesFrozenAtEventTime(axes, epoch, nullptr)}
    , sourceAxes_{axes}
    , useEpoch_{true}
{
}

Axes *FrameWithEpoch::getAxes() const
{
    if(useEpoch_)
        return frozenAxes_.get();
    else
        return sourceAxes_.get();
}

Point *FrameWithEpoch::getOrigin() const
{
    return origin_;
}

SharedPtr<EventTime> &FrameWithEpoch::getEpochHandle()
{
    return frozenAxes_->getEpochHandle();
}

EventTime *FrameWithEpoch::getEpoch() const
{
    return frozenAxes_->getEpoch();
}

errc_t FrameWithEpoch::getEpoch(TimePoint &epoch) const
{
    return frozenAxes_->getEpoch(epoch);
}

void FrameWithEpoch::setEpoch(EventTime *epoch)
{
    return frozenAxes_->setEpoch(epoch);
}


void FrameWithEpoch::setSourceAxes(Axes *axes)
{
    sourceAxes_ = axes;
    frozenAxes_->setAxes(axes);
}

Axes *FrameWithEpoch::getSourceAxes() const
{
    return sourceAxes_.get();
}

Axes *FrameWithEpoch::getReferenceAxes() const
{
    return frozenAxes_->getReferenceAxes();
}

void FrameWithEpoch::setReferenceAxes(Axes *axes)
{
    frozenAxes_->setReferenceAxes(axes);
}

AST_NAMESPACE_END


