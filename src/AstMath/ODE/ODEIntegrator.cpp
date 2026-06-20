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
#include "AstMath/MathOperator.hpp"

AST_NAMESPACE_BEGIN

using namespace math;

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

errc_t ODEIntegrator::initialize(ODE &ode)
{
    this->ode_ = &ode;
    this->initWorkStateObserver();
    return eNoError;
}

errc_t ODEIntegrator::integrate(
    ODE &ode, double *y, double &t, double tf,
    std::vector<double> &xlist, std::vector<std::vector<double>> &ylist)
{
    ODEStateVectorCollector* collector = new ODEStateVectorCollector(ode.getDimension());
    this->addStateObserver(collector);
    errc_t rc = this->integrate(ode, y, t, tf);
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

void ODEIntegrator::clearEventDetectors()
{
    eventDetectorList_.clear();
}

void ODEIntegrator::clearStateObservers()
{
    stateObserverList_.clear();
}

void ODEIntegrator::addStateObserver(ODEStateObserver *observer)
{
    stateObserverList_.addStateObserver(observer);
}

void ODEIntegrator::removeStateObserver(ODEStateObserver *observer)
{
    stateObserverList_.removeStateObserver(observer);
}

errc_t ODEIntegrator::integrateOneFixedStep(ODE& ode, double absh, double* y, double& t, double tf)
{
    // 初始化积分器
    // this->init(ode);
    
    double step = tf - t;
    int tdir = sign(step);
    double stepabs = std::abs(step);
    if(stepabs < absh)
    {
        absh = stepabs;
    }
    double h = absh * tdir;
    errc_t err = this->singleStep(ode, y, t, h);
    if(err != eNoError)
    {
        return err;
    }
    t += h;
    return eNoError;
}


errc_t ODEIntegrator::integrateFixedStep(ODE& ode, double stepSize, double* y, double& t, double tf, int* pNumSteps)
{
    // 初始化积分器
    this->initialize(ode);
    int numSteps = 0;
    
    errc_t err = eNoError;
    if(stepSize <= 0)
    {
        stepSize = 60;
    }
    double t0 = t;
    double habs = std::min((stepSize), fabs(tf - t0));
    /// int ndim = ode.getDimension();
    int tdir = sign(tf - t0);
    // double step = tdir * habs;
    // int numSteps = static_cast<int>(std::ceil(fabs(tf - t0) / stepSize));
    // double t = t0;
    // std::copy_n(y0, ndim, yf);
    if(workStateObserver_)
    {
        if(workStateObserver_->onStateUpdate(y, t, this) == EODEAction::eStop)
        {
            return eNoError;
        }
    }
    while (tdir * (tf - t) > 0) {
        double h = tdir * std::min(habs, std::abs(tf - t));
        err = this->singleStep(ode, y, t, h);
        if (err != eNoError) {
            return err;
        }
        t += h;
        numSteps ++;
        if(pNumSteps)
            *pNumSteps = numSteps;
        if(workStateObserver_)
        {
            if(workStateObserver_->onStateUpdate(y, t, this) == EODEAction::eStop)
            {
                break;
            }
        }
    }
    return eNoError;
}


void ODEIntegrator::initWorkStateObserver()
{
    // 存在状态观察者
    if(!stateObserverList_.empty())
    {
        // 最复杂的情况：同时也存在事件检测器
        // 此时转发到内部状态观察者来同时处理状态观察者和事件检测器事件
        if(!eventDetectorList_.empty())
        {
            if(!innerStateObserver_)
            {
                innerStateObserver_ = new ODEInnerStateObserver(this);
            }
            workStateObserver_ = innerStateObserver_;
        }
        // 如果只存在状态观察者，则直接使用该状态观察者
        else if(stateObserverList_.size() == 1)
        {
            workStateObserver_ = &stateObserverList_[0];
        }else{
            workStateObserver_ = &stateObserverList_;
        }
    }
    // 如果只存在事件检测器，则直接使用该事件检测器
    else if(eventDetectorList_.size() == 1)
    {
        workStateObserver_ = &eventDetectorList_[0];
    }else{
        workStateObserver_ = &eventDetectorList_;
    }
}

AST_NAMESPACE_END

