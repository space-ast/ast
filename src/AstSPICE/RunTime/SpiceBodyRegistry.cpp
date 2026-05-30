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

SpiceBodyRegistry::SpiceBodyRegistry()
    : solarSystem_(new SolarSystem())
{
    
}


SpiceBodyRegistry::SpiceBodyRegistry(bool whetherInit)
    : SpiceBodyRegistry()
{
    if (whetherInit)
    {
        init();
    }
}

SpiceBodyRegistry::~SpiceBodyRegistry() = default;

PBody SpiceBodyRegistry::findBody(StringView name) const
{
    return solarSystem_->getBody(name);
}



PBody SpiceBodyRegistry::findBody(int id) const
{
    return solarSystem_->getBodyBySpiceId(id);
}


errc_t SpiceBodyRegistry::init()
{
    return solarSystem_->loadDefault();
}

AST_NAMESPACE_END
