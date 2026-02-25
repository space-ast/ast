///
/// @file      StationaryOrbitDesigner.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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

#include "StationaryOrbitDesigner.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstCore/FrameTransform.hpp"
#include "AstCore/OrbitDesign.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Zeros.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

using namespace literals;

StationaryOrbitDesigner::StationaryOrbitDesigner()
    : StationaryOrbitDesigner(getDefaultCelestialBody())
{

}

StationaryOrbitDesigner::StationaryOrbitDesigner(CelestialBody *body) 
    : BaseOrbitDesigner(body)
    , subsatellitePoint_(-100_deg)
    , inclination_(0_deg)
{
    if(body && !body->isEarth())
    {
        aError("StationaryOrbitDesigner currently only support Earth.");
    }
}

err_t StationaryOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double gm = getGM();
    const double j2 = getJ2();
    const double rb = getBodyRadius();
    
    KinematicRotation rot;
    /// @todo 这里目前只能计算地球轨道，需要增加对其他天体的支持
    aICRFToECFTransform(orbitEpoch_, rot);
    double rotRate = rot.getRotationRate().norm();
    double period = kTwoPI / rotRate;
    double inc = inclination_;
    auto func = [gm, j2, rb, period, inc](double a) -> double
    {
        // double p1 = aSMajAxToPeriod(a, gm);
        const double ecc = 0.0;
        double p2 = aJ2Period(gm, j2, rb, a, ecc, inc);
        return p2 - period;
    };
    SolverStats stats{};
    double a_upper = aPeriodToSMajAx(period, gm);
    double a = brentq(func, 1, a_upper * 1.5, 1e-12, 1e-12, 100, stats);
    if(stats.error_num == 0){
        orbElem.rp_ = a;
        orbElem.e_ = 0.0;
        orbElem.i_ = inclination_;
        orbElem.raan_ = 0_deg;
        orbElem.argper_ = 0_deg;
        orbElem.trueA_ = 0_deg;
        return eNoError;
    }else{
        aError("StationaryOrbitDesigner failed to solve semimajor axis.");
        return -1;
    }
}

AST_NAMESPACE_END

