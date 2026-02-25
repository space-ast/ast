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
    const double inc = acos(1/sqrt(5));
    const double rp = perigeeAltitude_ + getBodyRadius();
    const double ecc = 0;
    aError("not implemented");

    orbElem.rp_ = rp;
    orbElem.e_ = ecc;
    orbElem.i_ = inc;
    orbElem.raan_ = apogeeLongitude_;
    orbElem.argper_ = argumentOfPerigee_;
    orbElem.trueA_ = 0_deg;
    return -1;
}

AST_NAMESPACE_END

