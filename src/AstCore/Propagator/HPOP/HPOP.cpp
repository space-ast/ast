///
/// @file      HPOP.cpp
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

#include "HPOP.hpp"
#include "HPOPEquation.hpp"
#include "AstMath/ODE.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

HPOP::~HPOP()
{
    if (equation_)
        delete equation_;
    if (integrator_)
        delete integrator_;
}

err_t HPOP::setForceModel(const HPOPForceModel& forcemodel)
{
    if(!equation_){
        equation_ = new HPOPEquation();
    }
    forcemodel_ = forcemodel;
    return equation_->setForceModel(forcemodel);
}

ODEIntegrator *HPOP::getIntegrator() const
{
    if(A_UNLIKELY(!integrator_)){
        // const_cast<HPOP*>
        (this)->integrator_ = new RKF78();
    }
    return integrator_;
}

err_t HPOP::initialize()
{
    if (!equation_){
        equation_ = new HPOPEquation(forcemodel_);
    }
    if (!integrator_){
        integrator_ = new RKF78();
    }
    // err |= integrator_->initialize(equation_);
    return 0;
}




err_t HPOP::propagate(const TimePoint &startTime, TimePoint &targetTime, Vector3d &position, Vector3d &velocity)
{
    err_t err = this->initialize();
    if (err)
        return err;
    int dim = equation_->getDimension();
    if (dim != 6){
        aError("dimension of equation is not 6");
        return -1;
    }
    equation_->setEpoch(startTime);
    array6d y = {position.x(), position.y(), position.z(), velocity.x(), velocity.y(), velocity.z()};
    double duration = targetTime - startTime;
    double t = 0;
    err = integrator_->integrate(*equation_,  y.data(), t, duration);
    if(t != duration && !err){
        targetTime = startTime + t;
    }
    position = {y[0], y[1], y[2]};
    velocity = {y[3], y[4], y[5]};
    return err;
}

AST_NAMESPACE_END

