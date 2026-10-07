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
#include "AstMath/AccelerationRotation.hpp"

AST_NAMESPACE_BEGIN

/// @brief 用差分求加速度时的步长 [s]
/// @details 与 Point::getPosVelAcc 的默认步长保持一致；
///          步长过小会放大速度的舍入误差，过大则增大截断误差。
static constexpr double kVectorAccelerationDiffStep = 0.05;

errc_t Vector::getVector(const TimePoint &tp, Vector3d &vec, Vector3d &vel, Vector3d &acc) const
{
    /*!
        加速度是速度分量在向量自身参考坐标系下的坐标时间导数，getVector(tp, vec, vel)
        给出的正是该坐标系下的速度分量，因此直接对速度分量做差商即可，
        不需要额外补偿坐标系自身的转动。
        这是给没有解析加速度的向量兜底用的默认实现，有解析解的向量应当重写本函数。
    */
    Vector3d vec0, vel0;
    errc_t rc = getVector(tp, vec0, vel0);
    if (A_UNLIKELY(rc != eNoError))
        return rc;

    const double h = kVectorAccelerationDiffStep;
    Vector3d vecPlus, velPlus, vecMinus, velMinus;
    const errc_t rcPlus  = getVector(tp + h, vecPlus, velPlus);
    const errc_t rcMinus = getVector(tp - h, vecMinus, velMinus);

    Vector3d accValue;
    if (rcPlus == eNoError && rcMinus == eNoError)
    {
        // 中心差分
        accValue = (velPlus - velMinus) / (2.0 * h);
    }
    else if (rcPlus == eNoError)
    {
        // 前向差分(时间点靠近可用数据的起点)
        accValue = (velPlus - vel0) / h;
    }
    else if (rcMinus == eNoError)
    {
        // 后向差分(时间点靠近可用数据的终点)
        accValue = (vel0 - velMinus) / h;
    }
    else
    {
        // 两侧都取不到，返回原始错误码，而不是悄悄给一个零加速度
        return rcPlus;
    }

    // 全部成功后再写回，避免失败时留下半成品输出
    vec = vec0;
    vel = vel0;
    acc = accValue;
    return eNoError;
}

errc_t Vector::getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec) const
{
    if (targetAxes == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetAxes, tp, vec);
}

errc_t Vector::getVectorIn(Axes &targetAxes, const TimePoint &tp, Vector3d &vec) const
{
    auto ownAxes = this->getAxes();
    if (ownAxes == nullptr)
        return eErrorNullPtr;
    if (ownAxes == &targetAxes)
    {
        return getVector(tp, vec);
    }
    else
    {
        Vector3d vecOwn;
        errc_t rc = getVector(tp, vecOwn);
        if (rc) return rc;
        Rotation rotation;
        rc = aAxesTransform(*ownAxes, targetAxes, tp, rotation);
        if (rc) return rc;
        rotation.transformVector(vecOwn, vec);
        return eNoError;
    }
}

errc_t Vector::getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    if (targetAxes == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetAxes, tp, vec, vel);
}

errc_t Vector::getVectorIn(Axes &targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    auto ownAxes = this->getAxes();
    if (ownAxes == nullptr)
        return eErrorNullPtr;
    if (ownAxes == &targetAxes)
    {
        return getVector(tp, vec, vel);
    }
    else
    {
        Vector3d vecOwn, velOwn;
        errc_t rc = getVector(tp, vecOwn, velOwn);
        if (rc) return rc;
        KinematicRotation rotation;
        rc = aAxesTransform(*ownAxes, targetAxes, tp, rotation);
        if (rc) return rc;
        rotation.transformVectorVelocity(vecOwn, velOwn, vec, vel);
        return eNoError;
    }
}

errc_t Vector::getVectorIn(Axes *targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel, Vector3d &acc) const
{
    if (targetAxes == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetAxes, tp, vec, vel, acc);
}

errc_t Vector::getVectorIn(Axes &targetAxes, const TimePoint &tp, Vector3d &vec, Vector3d &vel, Vector3d &acc) const
{
    auto ownAxes = this->getAxes();
    if (ownAxes == nullptr)
        return eErrorNullPtr;
    if (ownAxes == &targetAxes)
    {
        return getVector(tp, vec, vel, acc);
    }
    else
    {
        Vector3d vecOwn, velOwn, accOwn;
        errc_t rc = getVector(tp, vecOwn, velOwn, accOwn);
        if (rc) return rc;
        AccelerationRotation rotation;
        rc = aAxesTransform(*ownAxes, targetAxes, tp, rotation);
        if (rc) return rc;
        rotation.transformVecVelAcc(vecOwn, velOwn, accOwn, vec, vel, acc);
        return eNoError;
    }
}

errc_t Vector::getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec) const
{
    if (targetFrame == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetFrame, tp, vec);
}

errc_t Vector::getVectorIn(Frame &targetFrame, const TimePoint &tp, Vector3d &vec) const
{
    auto targetAxes = targetFrame.getAxes();
    return getVectorIn(targetAxes, tp, vec);
}

errc_t Vector::getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    if (targetFrame == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetFrame, tp, vec, vel);
}

errc_t Vector::getVectorIn(Frame &targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel) const
{
    auto targetAxes = targetFrame.getAxes();
    return getVectorIn(targetAxes, tp, vec, vel);
}

errc_t Vector::getVectorIn(Frame *targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel, Vector3d &acc) const
{
    if (targetFrame == nullptr)
        return eErrorNullPtr;
    return getVectorIn(*targetFrame, tp, vec, vel, acc);
}

errc_t Vector::getVectorIn(Frame &targetFrame, const TimePoint &tp, Vector3d &vec, Vector3d &vel, Vector3d &acc) const
{
    auto targetAxes = targetFrame.getAxes();
    return getVectorIn(targetAxes, tp, vec, vel, acc);
}

AST_NAMESPACE_END
