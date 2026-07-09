///
/// @file      Vector.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-05-14
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

#include "Vector.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/Frame.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

errc_t Vector::getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec) const
{
    auto ownAxes = this->getAxes();
    if (ownAxes == targetAxes)
    {
        return getVector(tp, vec);
    }
    else
    {
        Vector3d vecOwn;
        errc_t rc = getVector(tp, vecOwn);
        if (rc) return rc;
        Rotation rotation;
        rc = aAxesTransform(ownAxes, targetAxes, tp, rotation);
        if (rc) return rc;
        rotation.transformVector(vecOwn, vec);
        return eNoError;
    }
}

errc_t Vector::getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    auto ownAxes = this->getAxes();
    if (ownAxes == targetAxes)
    {
        return getVector(tp, vec, vel);
    }
    else
    {
        Vector3d vecOwn, velOwn;
        errc_t rc = getVector(tp, vecOwn, velOwn);
        if (rc) return rc;
        KinematicRotation rotation;
        rc = aAxesTransform(ownAxes, targetAxes, tp, rotation);
        if (rc) return rc;
        rotation.transformVectorVelocity(vecOwn, velOwn, vec, vel);
        return eNoError;
    }
}

errc_t Vector::getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec) const
{
    if (targetFrame == nullptr)
        return eErrorNullPtr;
    auto targetAxes = targetFrame->getAxes();
    return getVectorIn(targetAxes, tp, vec);
}

errc_t Vector::getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    if (targetFrame == nullptr)
        return eErrorNullPtr;
    auto targetAxes = targetFrame->getAxes();
    return getVectorIn(targetAxes, tp, vec, vel);
}

AST_NAMESPACE_END
