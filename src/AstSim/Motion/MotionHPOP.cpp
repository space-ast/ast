///
/// @file      MotionHPOP.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#include "MotionHPOP.hpp"
#include "AstCore/HPOP.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"
#include "AstMath/ODEIntegrator.hpp"
#include "AstSim/MotionProfileVisitor.hpp"

AST_NAMESPACE_BEGIN

PMotionHPOP MotionHPOP::New()
{
    return new MotionHPOP();
}

MotionHPOP::MotionHPOP() = default;
MotionHPOP::~MotionHPOP() = default;

errc_t MotionHPOP::makeEphemerisSpec(ScopedPtr<Ephemeris> &eph) const
{
    return makeEphemerisSimple(eph);
}

errc_t MotionHPOP::makeEphemerisSimple(ScopedPtr<Ephemeris> &eph) const
{
    errc_t rc;
    PropagationParams params{};
    rc = this->getPropagationParams(params);   AST_CHECK_ERRCODE(rc, "failed to get propagation params");
    auto propFrame = params.propagationFrame_;
    const TimePoint& epoch = params.epoch_;
    const CartState& cartState = params.stateInPropagationFrame_;
    TimeInterval interval{};
    rc = this->getInterval(interval);
    const TimePoint& startTime = interval.getStart();
    const TimePoint stopTime = interval.getStop();

    std::vector<double> times;
    std::vector<Vector3d> positions, velocities;
    
    HPOP hpop;
    hpop.setForceModel(this->forceModel_);
    hpop.setSpacecraftParam(this->spacecraftParam_);
    hpop.setIntegrator(integrator_);
    auto integrator = hpop.getIntegrator();  AST_CHECK_NULLPTR(integrator);
    rc = hpop.setPropagationFrame(propFrame); AST_CHECK_ERRCODE(rc, "failed to set propagation frame");

    CartState cartStateAtStartTime = cartState;

    // 先判断开始时间和轨道历元时间是否相同
    const double startOffset = startTime - epoch;
    if(startOffset != 0)
    {
        TimePoint startTimeTemp = startTime;
        rc = hpop.propagate(epoch, startTimeTemp, cartStateAtStartTime.pos(), cartStateAtStartTime.vel());
        AST_CHECK_ERRCODE(rc, "failed to propagate");
    }
    // 添加状态量观察者
    integrator->addStateObserver([&](double* y, double& x, ODEIntegrator* integrator){
        times.push_back(x);
        positions.push_back(Vector3d{y[0], y[1], y[2]});
        velocities.push_back(Vector3d{y[3], y[4], y[5]});
    });

    // 进行预报
    TimePoint stopTimeTemp = stopTime;
    rc = hpop.propagate(startTime, stopTimeTemp, cartStateAtStartTime.pos(), cartStateAtStartTime.vel());
    AST_CHECK_ERRCODE(rc, "failed to propagate");

    // 创建星历
    auto ephemLag = new EphemerisLagrangeVar();
    ephemLag->setTimes(times);
    ephemLag->setPositions(positions);
    ephemLag->setVelocities(velocities);
    ephemLag->setEpoch(startTime);  // 注意：这里将开始时间作为星历数据的历元时间
    ephemLag->setFrame(propFrame);
    eph = ephemLag;

    // 清除状态观察者
    integrator->clearStateObservers();
    
    return rc;
}

void MotionHPOP::setIntegrator(ODEIntegrator* integrator)
{
    integrator_ = integrator;
}

ODEIntegrator* MotionHPOP::getIntegrator() const
{
    return integrator_.get();
}



void MotionHPOP::accept(MotionProfileVisitor& visitor)
{
    visitor.visit(*this);
}

AST_NAMESPACE_END