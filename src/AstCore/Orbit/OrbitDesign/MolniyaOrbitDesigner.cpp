///
/// @file      MolniyaOrbitDesigner.cpp
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

#include "MolniyaOrbitDesigner.hpp"
#include "AstCore/OrbitalPrecession.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstMath/Zeros.hpp"
#include "AstUtil/Literals.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

using namespace literals;

MolniyaOrbitDesigner::MolniyaOrbitDesigner()
    : MolniyaOrbitDesigner(getDefaultCelestialBody())
{
}

MolniyaOrbitDesigner::MolniyaOrbitDesigner(CelestialBody *body)
    : BaseOrbitDesigner(body)
    , apogeeLongitude_(-100_deg)
    , perigeeAltitude_(500_km)
    , argumentOfPerigee_(270_deg)
{
    
}

err_t MolniyaOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    const double gm = getGM();
    const double rb = getBodyRadius();
    const double j2 = getJ2();
    /// @todo 这里目前只支持地球，需要增加对其他天体的支持
    const double bodyAngleVel = kEarthAngVel;

    const double inc = acos(1/sqrt(5));
    const double rp = perigeeAltitude_ + getBodyRadius();

    auto func = [gm, j2, rb, inc, rp, bodyAngleVel](double a) -> double
    {
        const double ecc = (a - rp) / (a);
        const double expectDeltaLon = kPI;
        double period = aJ2Period(gm, j2, rb, a, ecc, inc);                 // 交点周期
        double RAANAngle = aRAANRate(gm, j2, rb, a, ecc, inc) * period;     // 每个交点周期内的RAAN变化率
        
        double deltaLon = bodyAngleVel * period - RAANAngle;
        return deltaLon - expectDeltaLon;
    };
    SolverStats stats{};
    double a = brentq(func, rb, rb * 100, 1e-10, 1e-10, 100, stats);
    if(stats.error_num == 0){
        double ecc = (a - rp) / a;
        orbElem.rp_ = rp;
        orbElem.e_ = ecc;
        orbElem.i_ = inc;
        orbElem.raan_ = apogeeLongitude_;  // @fixme 
        orbElem.argper_ = argumentOfPerigee_;
        orbElem.trueA_ = 0_deg;
        return eNoError;
    }else{
        aError("failed to find the molniya orbit with the given parameters, errnum: %d", stats.error_num);
        return -1;
    }
}

AST_NAMESPACE_END

