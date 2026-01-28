///
/// @file      ODEFixedStepIntegrator.cpp
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

#include "ODEFixedStepIntegrator.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstMath/ODEStateObserver.hpp"
#include <limits>
#include <cmath>
#include <algorithm>

AST_NAMESPACE_BEGIN

using namespace math;

ODEFixedStepIntegrator::Workspace::Workspace()
    : numSteps_(0)
    , largestStepSize_(0)
    , smallestStepSize_(std::numeric_limits<double>::max())
    , dimension_(0)
    , stage_(0)
    , KArr_(nullptr)
    , absErrPerLen_(nullptr)
    , ymid_(nullptr)
    // , y_(nullptr)
    // , ynew_(nullptr)
    // , ystep_(nullptr)
    , nextAbsStepSize_(0)
{

}

void ODEFixedStepIntegrator::Workspace::reset(int dimension, int stage)
{
    if(dimension > this->dimension_ || stage > this->stage_)
    {
        clear();
        // 重置多步法中间结果数组
        KArr_ = new double*[stage];
        for(int i = 0; i < stage; i++)
        {
            KArr_[i] = new double[dimension];
        }
        absErrPerLen_ = new double[dimension];
        ymid_ = new double[dimension];
        // y_ = new double[dimension];
        // ynew_ = new double[dimension];
        // ystep_ = new double[dimension];
    }
    // 重置统计数据
    numSteps_ = 0;
    largestStepSize_ = 0;
    smallestStepSize_ = std::numeric_limits<double>::max();
    dimension_ = dimension;
    stage_ = stage;
}

void ODEFixedStepIntegrator::Workspace::clear()
{
    if(KArr_ != nullptr)
    {
        // 清空中间结果数组
        for(int i = 0; i < stage_; i++)
        {
            delete[] KArr_[i];
        }
        delete[] KArr_;
    }
    if(absErrPerLen_ != nullptr)
    {
        delete[] absErrPerLen_;
    }
    if(ymid_ != nullptr)
    {
        delete[] ymid_;
    }
    // if(y_ != nullptr)
    // {
    //     delete[] y_;
    // }
    // if(ynew_ != nullptr)
    // {
    //     delete[] ynew_;
    // }
    // if(ystep_ != nullptr)
    // {
    //     delete[] ystep_;
    // }
}


ODEFixedStepIntegrator::Workspace::~Workspace()
{
    clear();
}

ODEFixedStepIntegrator::ODEFixedStepIntegrator()
    : stepSize_(60)
{

}

ODEFixedStepIntegrator::~ODEFixedStepIntegrator()
{
    if(stateAtStepStart_)
    {
        delete[] stateAtStepStart_;
    }
    if(stateAtStepEnd_)
    {
        delete[] stateAtStepEnd_;
    }
    if(stateTemp_)
    {
        delete[] stateTemp_;
    }
}

int ODEFixedStepIntegrator::getNumSteps() const
{
    return this->getWorkspace().numSteps_;
}

err_t ODEFixedStepIntegrator::integrate(ODE& ode, double* y, double& t, double tf)
{
    // 初始化积分器
    this->initialize(ode);
    auto& wrk = this->getWorkspace();
    
    err_t err = eNoError;
    double stepSize = this->stepSize_;
    if(stepSize <= 0)
    {
        stepSize = 60;
    }
    double t0 = t;
    double habs = std::min(fabs(stepSize), fabs(tf - t0));
    int ndim = ode.getDimension();
    int tdir = sign(tf - t0);
    double step = tdir * habs;
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
        wrk.numSteps_++;
        if(workStateObserver_)
        {
            if(workStateObserver_->onStateUpdate(y, t, this) == EODEAction::eStop)
            {
                return eNoError;
            }
        }
    }
    return eNoError;
}

err_t ODEFixedStepIntegrator::integrateStep(ODE &ode, double *y, double &t, double tf)
{
    // 初始化积分器
    // this->init(ode);
    
    double absh = this->stepSize_;
    double step = tf - t;
    int tdir = sign(step);
    double stepabs = abs(step);
    if(stepabs < absh)
    {
        absh = stepabs;
    }
    double h = absh * tdir;
    err_t err = this->singleStep(ode, y, t, h);
    if(err != eNoError)
    {
        return err;
    }
    t += h;
    return eNoError;
}

void ODEFixedStepIntegrator::resetWorkspace(int dimension, int stage)
{
    if(dimension > this->getWorkspace().dimension_)
    {
        if(stateAtStepStart_)
        {
            delete[] stateAtStepStart_;
        }
        if(stateAtStepEnd_)
        {
            delete[] stateAtStepEnd_;
        }
        if(stateTemp_)
        {
            delete[] stateTemp_;
        }
        stateAtStepStart_ = new double[dimension];
        stateAtStepEnd_ = new double[dimension];
        stateTemp_ = new double[dimension];
    }
    this->getWorkspace().reset(dimension, stage);
}

AST_NAMESPACE_END