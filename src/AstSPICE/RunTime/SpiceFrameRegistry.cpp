///
/// @file      SpiceFrameRegistry.cpp
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

#include "SpiceFrameRegistry.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

SpiceFrameRegistry::SpiceFrameRegistry(bool whetherInit)
{
    if (whetherInit)
    {
        init();
    }
}

SpiceFrameRegistry &SpiceFrameRegistry::Instance()
{
    static SpiceFrameRegistry instance(true);
    return instance;
}

err_t SpiceFrameRegistry::init()
{
    return eNoError;
}

PFrame SpiceFrameRegistry::findFrame(StringView name) const
{
    auto it = frameMap_.find(std::string(name));
    if (it == frameMap_.end())
    {
        return PFrame();
    }
    return it->second;
}

AST_NAMESPACE_END

