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
    , dimension_(0)
    , stage_(0)
    , largestStepSize_(0)
    , smallestStepSize_(std::numeric_limits<double>::max())
    , KArr_(nullptr)
    , absErrPerLen_(nullptr)
    , ymid_(nullptr)
    // , y_(nullptr)
    // , ynew_(nullptr)
    // , ystep_(nullptr)
    , nextAbsStepSize_(60) // 这里初始为60，避免初始值为0时的无限循环
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
    : workspace_(), stepSize_(60)
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

errc_t ODEFixedStepIntegrator::integrate(ODE& ode, double* y, double& t, double tf)
{
    auto& wrk = this->getWorkspace();
    return this->integrateFixedStep(ode, this->stepSize_, y, t, tf, &wrk.numSteps_);
}

errc_t ODEFixedStepIntegrator::integrateOneStep(ODE &ode, double *y, double &t, double tf)
{
    return this->integrateOneFixedStep(ode, this->stepSize_, y, t, tf);
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