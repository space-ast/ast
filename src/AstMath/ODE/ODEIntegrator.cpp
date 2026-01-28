///
/// @file      ODEIntegrator.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-16
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

#include "ODEIntegrator.hpp"
#include "AstMath/ODEStateVectorCollector.hpp"

AST_NAMESPACE_BEGIN

ODEIntegrator::~ODEIntegrator()
{
    // if (workStepHandler)
    // {
    //     delete workStepHandler;
    // }
    if(innerStateObserver_)
    {
        delete innerStateObserver_;
        innerStateObserver_ = nullptr;
    }
}

err_t ODEIntegrator::initialize(ODE &ode)
{
    this->ode_ = &ode;
    this->initWorkStateObserver();
    return eNoError;
}

err_t ODEIntegrator::integrate(
    ODE &ode, double *y, double &t, double tf,
    std::vector<double> &xlist, std::vector<std::vector<double>> &ylist)
{
    ODEStateVectorCollector* collector = new ODEStateVectorCollector(ode.getDimension());
    this->addStateObserver(collector);
    err_t rc = this->integrate(ode, y, t, tf);
    xlist = std::move(collector->x());
    ylist = std::move(collector->y());
    this->removeStateObserver(collector);
    return rc;
}

void ODEIntegrator::addEventDetector(ODEEventDetector *detector)
{
    eventDetectorList_.addEventDetector(detector);
}

void ODEIntegrator::removeEventDetector(ODEEventDetector *detector)
{
    eventDetectorList_.removeEventDetector(detector);
}

void ODEIntegrator::addStateObserver(ODEStateObserver *observer)
{
    stateObserverList_.addStateObserver(observer);
}

void ODEIntegrator::removeStateObserver(ODEStateObserver *observer)
{
    stateObserverList_.removeStateObserver(observer);
}

void ODEIntegrator::initWorkStateObserver()
{
    if(!stateObserverList_.empty())
    {
        if(!eventDetectorList_.empty())
        {
            if(!innerStateObserver_)
            {
                innerStateObserver_ = new ODEInnerStateObserver(this);
            }
            workStateObserver_ = innerStateObserver_;
        }
        // eventDetectorList_ is empty
        else if(stateObserverList_.size() == 1)
        {
            workStateObserver_ = &stateObserverList_[0];
        }else{
            workStateObserver_ = &stateObserverList_;
        }
    }
    // stateObserverList_ is empty
    else if(eventDetectorList_.size() == 1)
    {
        workStateObserver_ = &eventDetectorList_[0];
    }else{
        workStateObserver_ = &eventDetectorList_;
    }
}

AST_NAMESPACE_END

