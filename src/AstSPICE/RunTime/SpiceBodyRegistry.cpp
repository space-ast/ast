///
/// @file      SpiceBodyRegistry.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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

#include "SpiceBodyRegistry.hpp"
#include "AstUtil/StringView.hpp"
#include "AstCore/SolarSystem.hpp"


AST_NAMESPACE_BEGIN

SpiceBodyRegistry &SpiceBodyRegistry::Instance()
{
    static SpiceBodyRegistry instance(true);
    return instance;
}

SpiceBodyRegistry::SpiceBodyRegistry(bool whetherInit)
{
    if (whetherInit)
    {
        init();
    }
}


PBody SpiceBodyRegistry::findBody(StringView name) const
{
    auto it = bodyMap_.find(name.to_string());
    if (it == bodyMap_.end())
    {
        return PBody();
    }
    return it->second;
}

PBody SpiceBodyRegistry::findBody(int id) const
{
    auto it = bodyIDMap_.find(id);
    if (it == bodyIDMap_.end())
    {
        return PBody();
    }
    return it->second;
}

enum ESpiceId
{
    eMercury    = 199,
    eVenus      = 299,
    eEarth      = 399,
    eMoon       = 301,
    eMars       = 499,
    eJupiter    = 599,
    eSaturn     = 699,
    eUranus     = 799,
    eNeptune    = 899,
    ePluto      = 999
};

err_t SpiceBodyRegistry::init()
{
    /// @todo 在SPICE这里使用另一套太阳系数据会比较合适
    aInitialize();
    auto ss = aDataContext_GetSolarSystem();

    bodyIDMap_[eMercury] = ss->getMercury();
    bodyIDMap_[eVenus]   = ss->getVenus();
    bodyIDMap_[eEarth]   = ss->getEarth();
    bodyIDMap_[eMoon]    = ss->getMoon();
    bodyIDMap_[eJupiter] = ss->getJupiter();
    bodyIDMap_[eSaturn]  = ss->getSaturn();
    bodyIDMap_[eUranus]  = ss->getUranus();
    bodyIDMap_[eNeptune] = ss->getNeptune();
    bodyIDMap_[ePluto]   = ss->getPluto();

    return eNoError;
}

AST_NAMESPACE_END
