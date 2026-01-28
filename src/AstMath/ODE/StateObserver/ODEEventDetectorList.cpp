///
/// @file      ODEEventsHandler.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-27
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

#include "ODEEventDetectorList.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>      // for memcpy


AST_NAMESPACE_BEGIN

ODEEventDetectorList::~ODEEventDetectorList()
{
    for(auto observer : eventObservers_)
    {
        delete observer;
    }
}


EODEAction ODEEventDetectorList::onStateUpdate(double* y, double& x, ODEIntegrator* integrator)
{
    ODEEventObserver* firstObservered = nullptr;    ///< 第一个观测到的事件
    double firstEventTime = std::numeric_limits<double>::max();
    for(auto observer : eventObservers_)
    {
        double lastTime = observer->lastTime_;
        bool eventOccurred = observer->isEventOccurred(y, x);
        if(eventOccurred)
        {
            // printf("size = %d\n", eventObservers_.size());
            // printf("detected event by signbit change, x = %f\n", x);
            // printf("goal = %lf\n", observer->detector_->getGoal());
            err_t err = observer->findEventTime(lastTime, x, observer->eventTime_, integrator);
            if(err == eNoError && observer->eventTime_ < firstEventTime)
            {
                firstEventTime = observer->eventTime_;
                firstObservered = observer;
            }
        }
    }
    // 第一个观测到的事件
    if(firstObservered != nullptr)
    {
        auto ode = integrator->getODE();
        int ndim = ode->getDimension();
        double t0 = integrator->timeAtStepStart();
        double step = firstEventTime - t0;
        memcpy(y, integrator->stateAtStepStart(), ndim * sizeof(double));
        integrator->singleStep(*ode, y, t0, step);
        x = firstEventTime;
        return EODEAction::eStop;
    }
    return EODEAction::eContinue;
}

void ODEEventDetectorList::addEventDetector(ODEEventDetector *detector)
{
    eventObservers_.emplace_back(new ODEEventObserver(detector));
}

void ODEEventDetectorList::removeEventDetector(ODEEventDetector *detector)
{
    auto it = std::find_if(eventObservers_.begin(), eventObservers_.end(), [detector](ODEEventObserver* obs) { return obs->getEventDetector() == detector; });
    if(it != eventObservers_.end())
    {
        delete *it;
        eventObservers_.erase(it);
    }
}

AST_NAMESPACE_END


