///
/// @file      AttitudeAlignConstrain.cpp
/// @brief     对齐/约束姿态(Aligned and Constrained)
/// @details
/// @author    axel
/// @date      2026-09-28
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

#include "AttitudeAlignConstrain.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/AttitudeUtil.hpp"
#include "AstMath/Vector.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

Axes* AttitudeAlignConstrain::getParent() const
{
    auto alignedVector = alignedVector_.get();
    if(alignedVector == nullptr)
        return nullptr;
    return alignedVector->getAxes();
}

errc_t AttitudeAlignConstrain::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    auto axes = this->getParent();
    auto alignedVector = alignedVector_.get();
    auto constrainedVector = constrainedVector_.get();
    if(alignedVector == nullptr || constrainedVector == nullptr || axes == nullptr)
        return eErrorNullPtr;
    Vector3d aligned, constrained;
    errc_t rc;
    rc = alignedVector->getVectorIn(*axes, tp, aligned);
    if(rc != eNoError) return rc;
    rc = constrainedVector->getVectorIn(*axes, tp, constrained);
    if(rc != eNoError) return rc;
    return aAlignConstrainTransform(
        alignedAxis_, aligned, 
        constrainedAxis_, constrained, 
        rotation
    );
}

errc_t AttitudeAlignConstrain::getTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    auto axes = this->getParent();
    auto alignedVector = alignedVector_.get();
    auto constrainedVector = constrainedVector_.get();
    if(alignedVector == nullptr || constrainedVector == nullptr || axes == nullptr)
        return eErrorNullPtr;
    Vector3d aligned, alignedVelocity, constrained, constrainedVelocity;
    errc_t rc;
    rc = alignedVector->getVectorIn(*axes, tp, aligned, alignedVelocity);
    if(rc != eNoError) return rc;
    rc = constrainedVector->getVectorIn(*axes, tp, constrained, constrainedVelocity);
    if(rc != eNoError) return rc;
    return aAlignConstrainTransform(
        alignedAxis_, aligned, alignedVelocity, 
        constrainedAxis_, constrained, constrainedVelocity, 
        rotation
    );
}
   
AST_NAMESPACE_END