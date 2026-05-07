///
/// @file      AxesLinkTo.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-04
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

#include "AxesLinkTo.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

AxesLinkTo* AxesLinkTo::New(StringView objectType, StringView name)
{
    return new AxesLinkTo(objectType, name);
}

Axes* AxesLinkTo::getParent() const
{
    return nullptr;
}

errc_t AxesLinkTo::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    return -1;
}


errc_t AxesLinkTo::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    return -1;
}


Axes* AxesLinkTo::resolve(Object* scope) const
{
    if (scope)
    {
        if(!scope->isOfType(objectType_))
        {
            aError("failed to resolve axes '%s' for object type '%s'", 
                name().c_str(), 
                objectType_.c_str()
            );
            return nullptr;
        }
    }
    return aFindChild<Axes*>(scope, name());
}


AxesLinkTo::AxesLinkTo(StringView objectType, StringView name)
    : objectType_{objectType}
    //, name_{name}
{
    setName(name);
}

AST_NAMESPACE_END

