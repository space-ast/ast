///
/// @file      RepeatingSunSyncOrbitDesigner.cpp
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

#include "RepeatingSunSyncOrbitDesigner.hpp"
#include "AstCore/OrbitalPrecession.hpp"
#include "AstCore/OrbitDesign.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstMath/Zeros.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

using namespace literals;

RepeatingSunSyncOrbitDesigner::RepeatingSunSyncOrbitDesigner()
    : RepeatingSunSyncOrbitDesigner(getDefaultCelestialBody())
{
}

RepeatingSunSyncOrbitDesigner::RepeatingSunSyncOrbitDesigner(CelestialBody *body)
    :BaseOrbitDesigner(body)
{
}

err_t RepeatingSunSyncOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double gm = getGM();
    const double rb = getBodyRadius();
    const double j2 = getJ2();
    /// @todo 这里目前只支持地球，需要增加对其他天体的支持
    const double bodyAngleVel = kEarthAngVel;
    const double ecc = 0;

    const int N = numberOfRevsRepeat_;
    double revsPerDay = approxRevsPerDay_;
    if(revsPerDay <= 0)
        revsPerDay = 1;

    int D = round(N / revsPerDay);  // 四舍五入取整(D的单位是升交日)
    if(D<=0)
        D = 1;
    const double expectDeltaLon = D * kTwoPI / N;                 // 期望每一圈的经度变化角度
    double approxMeanMotion = bodyAngleVel * kTwoPI / expectDeltaLon;
    double approxA = aMeanMotnToSMajAx(approxMeanMotion, gm);
    const double bodyMeanMotion = kTwoPI / (kEarthSiderealYear * kSecondsPerDay);
    auto func = [gm, j2, rb, ecc, expectDeltaLon, bodyMeanMotion, bodyAngleVel](double a) -> double
    {
        double inc = aSunSynchronousInclination(gm, j2, rb, bodyMeanMotion, a, ecc);
        double period = aJ2Period(gm, j2, rb, a, ecc, inc);                 // 交点周期
        double RAANAngle = aRAANRate(gm, j2, rb, a, ecc, inc) * period;     // 每个交点周期内的RAAN变化率
        
        double deltaLon = bodyAngleVel * period - RAANAngle;
        return deltaLon - expectDeltaLon;
    };
    SolverStats stats{};
    double a = brentq(func, 0.5 * approxA, 1.5 * approxA, 1e-10, 1e-10, 100, stats);
    if(stats.error_num == 0){
        orbElem.rp_ = a;
        orbElem.e_ = ecc;
        orbElem.i_ = aSunSynchronousInclination(gm, j2, rb, bodyMeanMotion, a, ecc);
        orbElem.raan_ = longitudeOfAscendingNode_;
        orbElem.argper_ = 0;
        orbElem.trueA_ = 0;
        return 0;
    }else{
        aError("failed to find the repeating orbit with the given parameters, errnum: %d", stats.error_num);
        return -1;
    }

    return eNoError;
}

err_t RepeatingSunSyncOrbitDesigner::setApproxAltitude(double alt)
{
    /// @todo 这里的逻辑与 RepeatingOrbitDesigner::setApproxRevsPerDay(double revs)相同，需要避免重复
    if(alt < 0)
    {
        aError("invalid altitude value: %f", alt);
        return eErrorInvalidParam;
    }

    double rb = getBodyRadius();
    double gm = getGM();
    // double j2 = getJ2();
    double a = alt + rb;
    positionType_ = eAltitude;
    approxAltitude_ = alt;
    // const double ecc = 0;
    // const double inc = inclination_;
    // (void) j2; (void) ecc; (void) inc;
    // double meanMotion = aMeanArgLatRate(gm, j2, rb, a, ecc, inc);
    double meanMotion = aSMajAxToMeanMotn(a, gm);
    approxRevsPerDay_ = meanMotion * kSecondsPerDay / kTwoPI;
    return eNoError;
}

err_t RepeatingSunSyncOrbitDesigner::setApproxRevsPerDay(double revsPerDay)
{
    if(revsPerDay < 0)
    {
        aError("invalid revsPerDay value: %f", revsPerDay);
        return eErrorInvalidParam;
    }
    double gm = getGM();
    double rb = getBodyRadius();
    double meanMotion = revsPerDay * kTwoPI / kSecondsPerDay;
    double a = aMeanMotnToSMajAx(meanMotion, gm);
    double alt = a - rb;
    if(alt < 0)
    {
        aError("invalid revsPerDay value: %f, alt: %f", revsPerDay, alt);
        return eErrorInvalidParam;
    }
    positionType_ = eRevsPerDay;
    approxRevsPerDay_ = revsPerDay;
    approxAltitude_ = alt;
    return eNoError;
}

AST_NAMESPACE_END

