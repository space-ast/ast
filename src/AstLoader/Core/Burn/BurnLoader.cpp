///
/// @file      BurnLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-18
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "BurnLoader.hpp"
#include "AstCore/Burn.hpp"
#include "AstCore/BurnFinite.hpp"
#include "AstCore/BurnImpulsive.hpp"
#include "AstCore/BurnCollocation.hpp"
#include "AstCore/Resolve.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"

AST_NAMESPACE_BEGIN


errc_t aLoadBurnFinite(const Value &value, BurnFinite &burn)
{
    const std::string type = value["Type"];
    if(type != "Maneuver:Finite")
    {
        aError("invalid type, expect 'Maneuver:Finite'");
        return eErrorInvalidParam;
    }
    // @todo 加载有限推力点火

    return eNoError;
}

errc_t aLoadBurnCollocation(const Value &value, BurnCollocation &burn)
{
    const std::string type = value["Type"];
    if(type != "Maneuver:Collocation")
    {
        aError("invalid type, expect 'Maneuver:Collocation'");
        return eErrorInvalidParam;
    }
    aError("unsupported type 'Maneuver:Collocation'");
    return eErrorUnsupported;
}

errc_t aLoadBurnImpulsive(const Value &value, BurnImpulsive &burn)
{
    {
        const std::string type = value["Type"];
        if(type != "Maneuver:Impulsive")
        {
            aError("invalid type, expect 'Maneuver:Impulsive'");
            return eErrorInvalidParam;
        }
    }
    {
        std::string thrustAxes = value["ThrustAxes"];
        Axes* axes = aResolveAxes(thrustAxes);
        burn.setAxes(axes);
    }
    {
        auto& cartesian = value["Cartesian"];
        if(!cartesian.isNull())
        {
            burn.setX(cartesian["X"]);
            burn.setY(cartesian["Y"]);
            burn.setZ(cartesian["Z"]);
        }
    }
    return eNoError;
}

errc_t aLoadBurn(const Value &value, SharedPtr<Burn> &burn)
{
    const std::string type = value["Type"];
    if(type == "Maneuver:Collocation")
    {
        auto collocation = new BurnCollocation();
        burn = collocation;
        return aLoadBurnCollocation(value, *collocation);
    }
    else if(type == "Maneuver:Impulsive")
    {
        auto impulse = new BurnImpulsive();
        burn = impulse;
        return aLoadBurnImpulsive(value, *impulse);
    }
    else if(type == "Maneuver:Finite")
    {
        auto finite = new BurnFinite();
        burn = finite;
        return aLoadBurnFinite(value, *finite);
    }
    else
    {
        aError("unsupported burn type '%s'", type.c_str());
        return eErrorInvalidParam;
    }
}


AST_NAMESPACE_END
