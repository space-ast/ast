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
#include <cstring>

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
    auto detector = detector_;
    double difference = detector_->getDifference(y, x);
    bool lastSign = std::signbit(lastDifference_);
    bool currentSign = std::signbit(difference);
    if(lastSign ^ currentSign)
    {
        // aDebug("detected event by signbit change");
        BrentSolver solver; // 这个求解器性能最佳
        // RidderSolver solver;
        // SecantSolver solver;
        // BisectionSolver solver;
        double result;
        int ndim = integrator->getODE()->getDimension();
        err_t err = solver.solveFunc(
            [detector, integrator, ndim](double t) -> double 
            {
                double t0 = integrator->timeAtStepStart();
                memcpy(integrator->stateAtStepEnd(), integrator->stateAtStepStart(), ndim * sizeof(double));
                integrator->singleStep(*integrator->getODE(), integrator->stateAtStepEnd(), t0, t - t0);
                return detector->getDifference(integrator->stateAtStepEnd(), t); 
            }, 
            lastTime_, x, result
        );
        if(err != eNoError)
        {
            aError("failed to solve event time, error = %d", err);
            return EODEAction::eStop;
        }else{
            auto& stats = solver.getStats();
            // aDebug("funcalls = %d, iterations = %d, result = %g", stats.funcalls, stats.iterations, result);
        }
        x = result;
        return EODEAction::eStop;
    }
    lastDifference_ = difference;
    lastTime_ = x;

    // aDebug("t = %f, value = %f, difference = %f\n", x, detector_->getValue(y, x), difference);
    return EODEAction::eContinue;
}


AST_NAMESPACE_END

