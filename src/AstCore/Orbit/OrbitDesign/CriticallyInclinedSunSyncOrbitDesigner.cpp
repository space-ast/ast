///
/// @file      CriticallyInclinedSunSyncOrbitDesigner.cpp
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

#include "CriticallyInclinedSunSyncOrbitDesigner.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Literals.hpp"
#include "AstMath/Zeros.hpp"

AST_NAMESPACE_BEGIN

using namespace literals;

CriticallyInclinedSunSyncOrbitDesigner::CriticallyInclinedSunSyncOrbitDesigner()
    : CriticallyInclinedSunSyncOrbitDesigner(getDefaultCelestialBody())
{

}

CriticallyInclinedSunSyncOrbitDesigner::CriticallyInclinedSunSyncOrbitDesigner(CelestialBody* body)
    : OrbitDesigner(body)
    , perigeeAltitude_(400_km)
    , raan_(-100_deg)
{
    if(body && !body->isEarth())
    {
        aError("CriticallyInclinedSunSyncOrbitDesigner only support Earth.");
    }
}

err_t CriticallyInclinedSunSyncOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double j2 = getJ2();
    const double gm = getGM();
    const double rb = getBodyRadius();

    const double rp = perigeeAltitude_ + rb;
    const double inc = kPI - acos(1/sqrt(5));

    orbElem.rp_ = rp;
    orbElem.i_ = inc;
    orbElem.raan_ = raan_;
    orbElem.argper_ = 0_deg;
    orbElem.trueA_ = 0_deg;
    // printf("gm=%.15g rb=%.15g j2=%.15g\n", gm, rb, j2);
    
    auto func = [rp, j2, gm, rb, inc](double ecc) -> double { 
        #if 1
        double a = aPeriRadToSMajAx(rp, ecc);
        double raanDot = -3./2. * j2 * sqrt(gm/ pow(rb, 3)) * pow(rb / a, 7./2.) * cos(inc) / pow(1 - ecc * ecc, 2);
        return raanDot - kEarthMeanMotion;
        #else
        double a = aPeriRadToSMajAx(rp, ecc);
        double raanDot = -9.9649 * pow(rb / a, 7./2.) * cos(inc) / pow(1 - ecc * ecc, 2);
        return raanDot - 0.9856;
        #endif
    };

    SolverStats stats{};
    double ecc = brentq(func, 0.0, 0.8, 1e-10, 1e-10, 100, stats);
    if(stats.error_num == 0){
        // double delta = func(ecc);
        // printf("delta: %.15g\n", delta);
        orbElem.e_ = ecc;
        return eNoError;
    }else{
        aError("failed to solve ecc with brentq, error_num: %d", stats.error_num);
        return stats.error_num;
    }
}

AST_NAMESPACE_END


