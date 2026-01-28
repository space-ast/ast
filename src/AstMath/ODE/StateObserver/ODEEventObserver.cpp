///
/// @file      ODEEventWatcher.cpp
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

#include "ODEEventObserver.hpp"
#include "AstMath/NLE.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstUtil/Logger.hpp"
#include <cstdio>
#include <cmath>
#include <cstring>      // for memcpy

AST_NAMESPACE_BEGIN

ODEEventObserver::ODEEventObserver(ODEEventDetector *detector)
    : detector_(detector)
{
}

ODEEventObserver::~ODEEventObserver()
{
    if(detector_)
    {
        delete detector_;
        detector_ = nullptr;
    }
}

EODEAction ODEEventObserver::onStateUpdate(double *y, double &x, ODEIntegrator* integrator)
{
    double lastTime = this->lastTime_;  // 记录上一个时间，用于精确求解事件时间
    bool eventOccurred = isEventOccurred(y, x);
    if(eventOccurred)
    {
        double eventTime = x;
        err_t err = findEventTime(lastTime, x, eventTime, integrator);
        if(err != eNoError)
        {
            // pass
            // aError("failed to find event time, error = %d", err);
        }else{
            memcpy(y, integrator->stateTemp(), integrator->getODE()->getDimension() * sizeof(double));
            x = eventTime;
        }
        return EODEAction::eStop;
    }
    return EODEAction::eContinue;
}

bool ODEEventObserver::isEventOccurred(double *y, double &x) 
{
    if(this->repeatCount_ < 0)
    {
        this->repeatCount_ = 0;
        this->lastDifference_ = detector_->getDifference(y, x);
        this->lastTime_ = x;
        return false;
    }else{
        auto direction = detector_->getDirection();
        double difference = detector_->getDifference(y, x);
        bool lastSign = std::signbit(lastDifference_);
        bool currentSign = std::signbit(difference);
        bool occurred;
        // 处理事件的穿越方向
        if(direction == ODEEventDetector::eBoth){
            occurred = lastSign ^ currentSign;
        }else{
            static_assert(ODEEventDetector::eDecrease < 0);
            static_assert(ODEEventDetector::eIncrease > 0);
            // 正向预报的上升触发相当于反向预报的下降触发
            bool timeSign = std::signbit((x - lastTime_) * direction);
            occurred = (lastSign^timeSign) && !(currentSign^timeSign);
        }
        // 记录当前状态，用于下一次判断
        lastDifference_ = difference;
        lastTime_ = x;
        if(occurred) 
            this->repeatCount_ ++ ;
        // 处理事件的重复次数
        return this->repeatCount_ >= detector_->getRepeatCount();
    }
}

err_t ODEEventObserver::findEventTime(double x1, double x2, double& result, ODEIntegrator *integrator)
{
    // aDebug("detected event by signbit change");
    auto detector = detector_;
    BrentSolver solver(detector->getThreshold()); // 这个求解器性能最佳
    // RidderSolver solver;
    // SecantSolver solver;
    // BisectionSolver solver;
    int ndim = integrator->getODE()->getDimension();
    /// @fixme 
    /// 这里额外计算了两次端点的开关函数，这个计算量是可以减少的
    err_t err =  solver.solveFunc(
        [detector, integrator, ndim](double t) -> double 
        {
            double t0 = integrator->timeAtStepStart();
            memcpy(integrator->stateTemp(), integrator->stateAtStepStart(), ndim * sizeof(double));
            integrator->singleStep(*integrator->getODE(), integrator->stateTemp(), t0, t - t0);
            return detector->getDifference(integrator->stateTemp(), t); 
        }, 
        x1, x2, this->eventTime_
    );
    if(err != eNoError)
    {
        aError("failed to solve event time, error = %d", err);
    }else{
        result = eventTime_;
    }
    return err;
}

AST_NAMESPACE_END

