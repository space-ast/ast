///
/// @file      MotionSPICE.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-26
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "MotionSPICE.hpp"
#include "AstSim/MotionProfileVisitor.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstCore/EphemerisLagrangeVar.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

errc_t MotionSPICE::makeEphemerisSpec(ScopedPtr<Ephemeris> &eph) const
{
    auto ephemSPK = new BodyEphemerisSPK();
    ephemSPK->setSpiceIndex(getSpiceIndex());
    ephemSPK->openSPKFile(getSpiceFile());
    eph = ephemSPK;
    return eNoError;
}

errc_t MotionSPICE::makeEphemerisSimple(ScopedPtr<Ephemeris> &eph) const
{
    TimeInterval interval{};
    errc_t rc = this->getInterval(interval);
    if(rc)
    {
        aError("failed to get interval");
        return rc;
    }
    if(!spk_.isOpen())
    {
        aError("failed to open spice file: '%s'", getSpiceFile().c_str());
        return rc;
    }
    std::vector<double> times;
    std::vector<Vector3d> positions;
    std::vector<Vector3d> velocities;

    const TimePoint& epoch = interval.getStart();
    auto range = interval.discretize(epoch, getStepSize());
    size_t n = range.size();
    times.reserve(n);
    positions.reserve(n);
    velocities.reserve(n);

    for(auto second: range)
    {
        TimePoint tp = epoch + second;
        Vector3d pos, vel;
        rc = spk_.getPosVelICRF(tp, getSpiceIndex(), ESpiceId::eSolarSystemBarycenter, pos, vel);
        if(rc)
        {
            aError("failed to get ephemeris at time %f", second);
            return rc;
        }
        times.push_back(second);
        positions.push_back(pos);
        velocities.push_back(vel);
    }

    Frame* frame = aFrameICRF();  //@todo 有些SPK并不是参考自ICRF，需要根据实际情况选择正确的参考帧
    auto ephemLag = new EphemerisLagrangeVar(frame);
    ephemLag->setEpoch(epoch);
    ephemLag->setTimes(std::move(times));
    ephemLag->setPositions(std::move(positions));
    ephemLag->setVelocities(std::move(velocities));

    eph = ephemLag;
    return eNoError;
}

void MotionSPICE::accept(MotionProfileVisitor &visitor)
{
    visitor.visit(*this);
}

errc_t MotionSPICE::setSpiceFile(StringView spiceFile)
{
    return this->spk_.open(spiceFile);
}

MotionSPICE* MotionSPICE::New()
{
    return new MotionSPICE();
}

AST_NAMESPACE_END