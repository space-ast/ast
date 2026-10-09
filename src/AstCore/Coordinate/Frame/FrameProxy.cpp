///
/// @file      FrameProxy.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-09
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

#include "FrameProxy.hpp"

AST_NAMESPACE_BEGIN

std::string FrameProxy::getRepresentation() const
{
    auto impl = this->impl();
    if(!impl)
        return Object::getRepresentation();
    return impl->getRepresentation();
}

Frame* FrameProxy::getParent() const
{
    auto impl = this->impl();
    if(!impl)
        return nullptr;
    return impl->getParent();
}

errc_t FrameProxy::getTransform(const TimePoint& tp, Transform& transform) const
{
    auto impl = this->impl();
    if(!impl)
        return eErrorNullPtr;
    return impl->getTransform(tp, transform);
}

errc_t FrameProxy::getTransform(const TimePoint& tp, KinematicTransform& transform) const
{
    auto impl = this->impl();
    if(!impl)
        return eErrorNullPtr;
    return impl->getTransform(tp, transform);
}

errc_t FrameProxy::getTransform(const TimePoint& tp, AccelerationTransform& transform) const
{
    auto impl = this->impl();
    if(!impl)
        return eErrorNullPtr;
    return impl->getTransform(tp, transform);
}

Axes* FrameProxy::getAxes() const
{
    auto impl = this->impl();
    if(!impl)
        return nullptr;
    return impl->getAxes();
}

Point* FrameProxy::getOrigin() const
{
    auto impl = this->impl();
    if(!impl)
        return nullptr;
    return impl->getOrigin();
}



AST_NAMESPACE_END
