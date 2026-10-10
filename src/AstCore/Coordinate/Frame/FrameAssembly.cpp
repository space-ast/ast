///
/// @file      FrameAssembly.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-11
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

#include "FrameAssembly.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"


AST_NAMESPACE_BEGIN

PFrameAssembly FrameAssembly::New(Point *origin, Axes *axes)
{
    return new FrameAssembly(origin, axes);
}

HFrameAssembly FrameAssembly::MakeShared(Point *origin, Axes *axes)
{
    return new FrameAssembly(origin, axes);
}

FrameAssembly::FrameAssembly(Point *origin, Axes *axes)
    : Frame()
    , origin_(origin)
    , axes_(axes)
{
}

Frame *FrameAssembly::getParent() const
{
    if(origin_)
        return origin_->getFrame();
    return nullptr;
}

errc_t FrameAssembly::getTransform(const TimePoint &tp, Transform &transform) const
{
    errc_t rc;
    auto origin = this->getOrigin();
    auto axes = this->getAxes();
    if(!origin || !axes)
        return eErrorNullPtr;
    auto parent = origin->getFrame();
    if(!parent)
        return eErrorNullPtr;
    
    rc = origin->getPos(tp, transform.getTranslation());
    if(rc != 0) return rc;

    rc = axes->getTransformFrom(parent->getAxes(), tp, transform.getRotation());
    if(rc != 0) return rc;
    
    return eNoError;
}

errc_t FrameAssembly::getTransform(const TimePoint &tp, KinematicTransform &transform) const
{
    errc_t rc;
    auto origin = this->getOrigin();
    auto axes = this->getAxes();
    if(!origin || !axes)
        return eErrorNullPtr;
    auto parent = origin->getFrame();
    if(!parent)
        return eErrorNullPtr;

    rc = origin->getPosVel(tp, transform.getTranslation(), transform.getVelocity());
    if(rc != 0) return rc;

    rc = axes->getTransformFrom(parent->getAxes(), tp, transform.getKinematicRotation());
    if(rc != 0) return rc;

    return eNoError;
}

AST_NAMESPACE_END


