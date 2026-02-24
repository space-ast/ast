///
/// @file      CriticallyInclinedOrbitDesigner.cpp
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

#include "CriticallyInclinedOrbitDesigner.hpp"
#include "AstCore/OrbitParam.hpp"
#include "AstUtil/Literals.hpp"

AST_NAMESPACE_BEGIN

using namespace literals;

CriticallyInclinedOrbitDesigner::CriticallyInclinedOrbitDesigner()
    : CriticallyInclinedOrbitDesigner(getDefaultCelestialBody())
{
}

CriticallyInclinedOrbitDesigner::CriticallyInclinedOrbitDesigner(CelestialBody* body)
    : OrbitDesigner(body)
    , direction_(ePosigrade)
    , apogeeAltitude_(12000_km)
    , perigeeAltitude_(400_km)
    //, longitudeOfAscendingNode_(-100_deg)
    , raan_(-100_deg)
{

}


err_t CriticallyInclinedOrbitDesigner::getOrbitState(ModOrbElem &orbElem) const
{
    double rp = perigeeAltitude_ + getBodyRadius();
    double ra = apogeeAltitude_ + getBodyRadius();
    orbElem.rp_ = rp;
    orbElem.e_ = aRadiiToEcc(rp, ra);
    if (direction_ == ePosigrade)
        orbElem.i_ = acos(1/ sqrt(5));
    else
        orbElem.i_ = kPI - acos(1/ sqrt(5));
    // orbElem.raan_ = longitudeOfAscendingNode_; // @todo 需要转换为raan
    orbElem.raan_ = raan_;
    orbElem.argper_ = 0_deg;
    orbElem.trueA_ = 0_deg;
    return eNoError;
}


AST_NAMESPACE_END

