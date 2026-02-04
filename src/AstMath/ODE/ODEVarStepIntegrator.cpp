///
/// @file      ODEVarStepIntegrator.cpp
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

#include "ODEVarStepIntegrator.hpp"
#include "AstMath/ODEStateObserver.hpp"
#include "AstMath/MathOperator.hpp"
#include "AstUtil/Logger.hpp"
#include <cmath>
#include <algorithm>

AST_NAMESPACE_BEGIN
using namespace math;

ODEVarStepIntegrator::ODEVarStepIntegrator()
    : useMinStep_{false}
    , useMaxStep_{false}
    , useFixedStepSize_{false}
    , warnOnMinStep_{true}
    , maxStepAttempts_{50}
    , minStepSize_{1}
    , maxStepSize_{86400}
    , maxAbsErr_{1e-10}
    , maxRelErr_{1e-13}
    , minStepScaleFactor_{0.5}
    , maxStepScaleFactor_{2.0}
    , safetyCoeffLow_{0.8}
    , safetyCoeffHigh_{0.9}
    , errCtrPowthLow_{0.25}
    , errCtrPowthHigh_{0.1}
{

}

ODEVarStepIntegrator::~ODEVarStepIntegrator()
{
    
}

/// @brief 获取积分过程中统计到的最大步长
double ODEVarStepIntegrator::getLargestStepSize() const
{
    return this->getWorkspace().largestStepSize_;
}

/// @brief 获取积分过程中统计到的最小步长
double ODEVarStepIntegrator::getSmallestStepSize() const
{
    return this->getWorkspace().smallestStepSize_;
}


err_t ODEVarStepIntegrator::integrate(ODE &ode, double* y, double& t, double tf)
{
    this->initialize(ode);
    auto& wrk = this->getWorkspace();
    double absh, h, hmin, hmax;
    double tnew;
    double t0 = t;
    if(this->useMinStep_){
        hmin = this->minStepSize_;
    }else{
        hmin = 16 * eps(t0);
    }
    if(this->useMaxStep_){
        hmax = this->maxStepSize_;
    }else{
        hmax = tf - t0;
    }

    absh = std::abs(this->getStepSize());
    // t = t0;
    int tdir = sign(tf - t);
    bool final = false;
    int numAttempts = 0;
    const double* y0 = y;
    double* yf = y;
    std::copy_n(y0, wrk.dimension_, this->stateAtStepStart_);
    this->timeAtStepStart_ = t0;
    if(workStateObserver_){
        if(workStateObserver_->onStateUpdate(this->stateAtStepStart_, t, this) == EODEAction::eStop)
        {
            return eNoError;
        }
    }
    while(1)
    {
        if(!this->useMinStep_){
            hmin = 16 * eps(t);
        }
        absh = clamp(absh, hmin, hmax);
        if(!(1.1 * absh < std::abs(tf - t)))
        {
            h = tf - t;
            absh = std::abs(h);
            tnew = tf;
            final = true;
        }else{
            h = absh * tdir;
            tnew = t + h;
        }
        std::copy_n(this->stateAtStepStart_, wrk.dimension_, this->stateAtStepEnd_);
        this->timeAtStepEnd_ = tnew;
        if(err_t rc = this->singleStep(ode, this->stateAtStepEnd_, t, h))
        {
            return rc;
        }
        bool isOK = this->isErrorMeet(absh, this->stateAtStepStart_, this->stateAtStepEnd_);
        if(isOK)
        {
            if(workStateObserver_){
                if(workStateObserver_->onStateUpdate(this->stateAtStepEnd_, tnew, this) == EODEAction::eStop)
                {
                    break;
                }
            }
            wrk.numSteps_ ++;
            wrk.largestStepSize_ = std::max(wrk.largestStepSize_, absh);
            wrk.smallestStepSize_ = std::min(wrk.smallestStepSize_, absh);
            if(final){
                break;
            }
            numAttempts = 0;
        }else{
            final = false;
            numAttempts ++;
            if(numAttempts >= this->maxStepAttempts_)
            {
                // @fixme! 这里是否直接停止积分？
                aWarning("Max iteration reached.");
            }else{
                continue;
            }
        }
        std::swap(this->stateAtStepStart_, this->stateAtStepEnd_);
        this->timeAtStepStart_ = tnew;
        t = tnew;
    }
    std::copy_n(this->stateAtStepEnd(), wrk.dimension_, yf);
    t = tnew;
    return eNoError;
}

err_t ODEVarStepIntegrator::integrateStep(ODE &ode, double* y, double &t, double tf)
{
    auto& wrk = this->getWorkspace();
    double& absh = wrk.nextAbsStepSize_;
    double step = tf - t;
    int tdir = sign(step);
    double stepabs = std::abs(step);
    if(stepabs < absh)
    {
        absh = stepabs;
    }
    // err_t err;
    bool isOK = false;
    int numAttempts = 0;
    const double* y0 = y;
    // double* yf = y;
    double h;
    do{
        h = absh * tdir;
        if(err_t rc = this->singleStep(ode, y, t, h))
        {
            return rc;
        }
        isOK = this->isErrorMeet(absh, y0, y);
        if(this->useMinStep_){
            absh = std::max(absh, this->minStepSize_);
        }
        if(this->useMaxStep_){
            absh = std::min(absh, this->maxStepSize_);
        }
        if(numAttempts ++ >= this->maxStepAttempts_)
        {
            aWarning("Max iteration reached.");
            return EError::eErrorMaxIter;
        }
    }while(!isOK);
    t += h;
    return eNoError;
}

bool ODEVarStepIntegrator::isErrorMeet(double &absh, const double *y, const double *ynew)
{
    using std::max;
    using std::abs;

    double maxErrRatio;
    double err;
    auto& wrk = this->getWorkspace();
    int dim = std::min(6, wrk.dimension_);  // @fixme!
    double threshold = this->maxAbsErr_ / this->maxRelErr_;
    double rtol = this->maxRelErr_;

    // 计算 maxErrRatio

    // L2范数误差控制
    #if 0
    {
        double normynew = norm(ynew, dim);
        double errwt = max(normynew, threshold);
        err = absh * (norm(wrk.absErrPerLen_, dim) / errwt);
        maxErrRatio = err / rtol;
    }
    #endif

    // L∞范数误差控制
    {
        double maxTemp = 0;
        for (int i = 0; i < dim; i++)
        {
            double temp = std::abs(wrk.absErrPerLen_[i]) / std::max(std::max(std::abs(ynew[i]), std::abs(y[i])), threshold);
            if(temp > maxTemp)
            {
                maxTemp = temp;
            }
        }
        err = absh * maxTemp;
        maxErrRatio = err / rtol;
    }

    // 计算下一个步长，判断是否满足误差要求
    bool isOK;
    {
        if(maxErrRatio > 1)
        {
            double scale = this->safetyCoeffLow_ * pow(maxErrRatio, -this->errCtrPowthLow_);
            scale = std::max(scale, this->minStepScaleFactor_);
            absh *= scale;
            isOK = false;
        }else{
            double scale = this->safetyCoeffHigh_ * pow(maxErrRatio, -this->errCtrPowthHigh_);
            scale = std::min(scale, this->maxStepScaleFactor_);
            absh *= scale;
            isOK = true;
        }
    }
    return isOK;
}

AST_NAMESPACE_END