///
/// @file      MotionJ4Analytical.cpp
/// @brief     J4解析轨道动力学实现
/// @author    axel
/// @date      2026-04-14
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

#include "MotionJ4Analytical.hpp"
#include "AstSim/MotionProfileVisitor.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"
#include "AstCore/J4Analytical.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

MotionJ4Analytical *MotionJ4Analytical::New()
{
    return new MotionJ4Analytical();
}


errc_t MotionJ4Analytical::makeEphemerisSpec(ScopedPtr<Ephemeris> &eph) const
{
    return MotionJ4Analytical::makeEphemerisSimple(eph);
}

errc_t MotionJ4Analytical::makeEphemerisSimple(ScopedPtr<Ephemeris> &eph) const
{
    errc_t rc;
    PropagationParams params{};
    rc = this->getPropagationParams(params);   AST_CHECK_ERRCODE(rc, "failed to get propagation params");
    auto propFrame = params.propagationFrame_;
    const TimePoint& epoch = params.epoch_;
    const CartState& cartState = params.stateInPropagationFrame_;
    double gm = propFrame->getGM();   AST_CHECK_INVALID(gm <= 0);
    
    // 获取天体信息
    auto body = propFrame->getBody();
    AST_CHECK_INVALID(body == nullptr);
    double j2 = body->getJ2();
    double j4 = body->getJ4();
    double re = body->getRadius();

    std::vector<double> times;
    std::vector<Vector3d> positions, velocities;
    rc = this->discretizeInterval(epoch, stepSize_, times);   AST_CHECK_ERRCODE(rc, "failed to discretize interval");
    positions.resize(times.size());
    velocities.resize(times.size());
    for(size_t i = 0; i < times.size(); i++){
        double t = times[i];
        CartState state = cartState;
        // 使用 J4 传播函数
        aJ4AnalyticalProp(t, gm, j2, j4, re, state.pos(), state.vel());
        positions[i] = state.pos();
        velocities[i] = state.vel();
    }

    auto ephemLag = new EphemerisLagrangeVar();
    ephemLag->setTimes(times);
    ephemLag->setPositions(positions);
    ephemLag->setVelocities(velocities);
    ephemLag->setEpoch(epoch);
    ephemLag->setFrame(propFrame);
    eph = ephemLag;

    return eNoError;
}

void MotionJ4Analytical::accept(MotionProfileVisitor& visitor)
{
    visitor.visit(*this);
}

AST_NAMESPACE_END