///
/// @file      SunSynchronousOrbitDesigner.cpp
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

#include "SunSynchronousOrbitDesigner.hpp"
#include "AstCore/OrbitDesign.hpp"
#include "AstCore/OrbitalPrecession.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"
#include "AstMath/Zeros.hpp"

AST_NAMESPACE_BEGIN

SunSynchronousOrbitDesigner::SunSynchronousOrbitDesigner()
    : SunSynchronousOrbitDesigner(getDefaultCelestialBody())
{
}

SunSynchronousOrbitDesigner::SunSynchronousOrbitDesigner(CelestialBody *body)
    : BaseOrbitDesigner(body)
    , geometryDefinition_(eAltitude)
    , nodeDefinition_(eAscendingNode)
    , inclination_(1.6935736087532673_rad)
    , altitude_(400_km)
    , localTimeOfAscendingNode_(0)
    , localTimeOfDescendingNode_(kPI)
{
    if(body && !body->isEarth())
    {
        aError("SunSynchronousOrbitDesigner only support Earth.");
    }
}

err_t SunSynchronousOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double rb = getBodyRadius();

    double a = getAltitude() + rb;
    double e = 0;
    double inc = getInclination();

    orbElem.rp_ = a;
    orbElem.i_ = inc;
    orbElem.e_ = e;

    if(nodeDefinition_ == eAscendingNode)
    {
        orbElem.raan_ = localTimeOfAscendingNode_;      // @fixme 
    }else{
        orbElem.raan_ = localTimeOfDescendingNode_;     // @fixme 
    }
    orbElem.argper_ = 0;
    orbElem.trueA_ = 0;
    return eNoError;
}

double SunSynchronousOrbitDesigner::calcInclination(double alt) const
{
    const double rb = getBodyRadius();
    const double gm = getGM();
    const double j2 = getJ2();

    const double a = alt + rb;
    auto func = [gm, j2, rb, a](double inc) -> double 
    {
        const double ecc = 0;
        // const double meanMotion = kEarthMeanMotion;
        // 这里应该使用恒星年
        const double meanMotion = kTwoPI / (kEarthSiderealYear * kSecondsPerDay);
        return aRAANRate(gm, j2, rb, a, ecc, inc) - meanMotion;
    };
    SolverStats stats;
    double inc = brentq(func, kHalfPI, kPI, 1e-12, 1e-14, 100, stats);
    if(stats.error_num == 0){
        return inc;
    }else{
        aError("failed to solve inclination for sun synchronous orbit.");
        return 0.0;
    }
}

double SunSynchronousOrbitDesigner::calcInclination() const
{
    return calcInclination(altitude_);
}

double SunSynchronousOrbitDesigner::calcAltitude(double inc) const
{
    const double rb = getBodyRadius();
    const double gm = getGM();
    const double j2 = getJ2();

    auto func = [gm, j2, rb, inc](double alt) -> double 
    {
        const double a = alt + rb;
        const double ecc = 0;
        // const double meanMotion = kEarthMeanMotion;
        // 这里应该使用恒星年
        const double meanMotion = kTwoPI / (kEarthSiderealYear * kSecondsPerDay);
        return aRAANRate(gm, j2, rb, a, ecc, inc) - meanMotion;
    };
    SolverStats stats;
    double alt = brentq(func, 0, 10000_km, 1e-12, 1e-14, 100, stats);
    if(stats.error_num == 0){
        return alt;
    }else{
        aError("failed to solve altitude for sun synchronous orbit.");
        return 0.0;
    }
}

double SunSynchronousOrbitDesigner::calcAltitude() const
{
    return calcAltitude(inclination_);
}

double SunSynchronousOrbitDesigner::getInclination() const
{
    return inclination_;
}

void SunSynchronousOrbitDesigner::setInclination(double inc)
{
    geometryDefinition_ = eInclination;
    inclination_ = inc;
    altitude_ = calcAltitude(inc);    
}

double SunSynchronousOrbitDesigner::getAltitude() const
{
    return altitude_;
}

void SunSynchronousOrbitDesigner::setAltitude(double alt)
{
    geometryDefinition_ = eAltitude;
    altitude_ = alt;
    inclination_ = calcInclination(alt);
}

AST_NAMESPACE_END


