///
/// @file      Point.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-09
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

#include "Point.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"

AST_NAMESPACE_BEGIN

CelestialBody *Point::toBody() const
{
    /// @todo 这里需要优化动态类型转换的执行效率
    return dynamic_cast<CelestialBody*>(const_cast<Point*>(this));
}

errc_t Point::getInterval(TimeInterval &interval) const
{
    interval.setWhole();
    return eNoError;
}

/// @brief 用差分求加速度时的步长 [s]
/// @details 与 kAxesAccelerationDiffStep 的默认步长保持一致；
///          步长过小会放大速度的舍入误差，过大则增大截断误差。
static constexpr double kPointAccelerationDiffStep = 0.05;

errc_t Point::getPosVelAcc(const TimePoint &tp, Vector3d &pos, Vector3d &vel, Vector3d &acc) const
{
    /*!
        按 AccelerationTransform 的约定，加速度是速度的分量在坐标系下的坐标时间导数；
        getPosVel 给出的正是该坐标系下的速度分量，因此直接对速度分量做差商即可，
        不需要额外补偿坐标系自身的转动。
        这是给没有解析加速度的点兜底用的默认实现，有解析解的点应当重写本函数。
    */
    Vector3d pos0, vel0;
    errc_t rc = getPosVel(tp, pos0, vel0);
    if (A_UNLIKELY(rc != eNoError))
        return rc;

    const double h = kPointAccelerationDiffStep;
    Vector3d posPlus, velPlus, posMinus, velMinus;
    const errc_t rcPlus  = getPosVel(tp + h, posPlus, velPlus);
    const errc_t rcMinus = getPosVel(tp - h, posMinus, velMinus);

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
    pos = pos0;
    vel = vel0;
    acc = accValue;
    return eNoError;
}

errc_t Point::getPosIn(Frame *frame, const TimePoint &tp, Vector3d &pos) const
{
    if(frame == nullptr)
        return eErrorNullPtr;
    return getPosIn(*frame, tp, pos);
}

errc_t Point::getPosIn(Frame& frame, const TimePoint& tp, Vector3d& pos) const
{
    auto parent = this->getFrame();
    if(parent == nullptr)
    {
        return eErrorNullPtr;
    }
    else if(parent == &frame)
    {
        return getPos(tp, pos);
    }
    else
    {
        Vector3d posInParent;
        errc_t rc = getPos(tp, posInParent);
        if(rc) return rc;
        Transform transform;
        rc = aFrameTransform(*parent, frame, tp, transform);
        if(rc) return rc;
        transform.transformPosition(posInParent, pos);
        return eNoError;
    }
}

errc_t Point::getPosVelIn(Frame *frame, const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    if(frame == nullptr)
        return eErrorNullPtr;
    return getPosVelIn(*frame, tp, pos, vel);
}

errc_t Point::getPosVelIn(Frame& frame, const TimePoint& tp, Vector3d& pos, Vector3d& vel) const
{
    auto parent = this->getFrame();
    if(parent == nullptr)
    {
        return eErrorNullPtr;
    }
    else if(parent == &frame)
    {
        return getPosVel(tp, pos, vel);
    }
    else
    {
        Vector3d posInParent, velInParent;
        errc_t rc = getPosVel(tp, posInParent, velInParent);
        if(rc) return rc;
        KinematicTransform transform;
        rc = aFrameTransform(*parent, frame, tp, transform);
        if(rc) return rc;
        transform.transformPositionVelocity(posInParent, velInParent, pos, vel);
        return eNoError;
    }
}

errc_t Point::getPosIn(Frame *frame, const TimePointRange &range, std::vector<Vector3d> &posList) const
{
    // 空范围：先清空输出，避免复用的容器残留上一次调用的数据
    posList.resize(range.size());
    if(range.size() == 0) return eNoError;
    errc_t rc = eNoError;
    for(size_t i=0; i < range.size()-1; i++)
    {
        rc |= getPosIn(frame, range.start() + i * range.step(), posList[i]);
    }
    rc |= getPosIn(frame, range.stop(), posList.back());
    return rc;
}

errc_t Point::getPosVelIn(Frame *frame, const TimePointRange &range, std::vector<Vector3d> &posList, std::vector<Vector3d> &velList) const
{
    // 空范围：先清空输出，避免复用的容器残留上一次调用的数据
    posList.resize(range.size());
    velList.resize(range.size());
    if(range.size() == 0) return eNoError;
    errc_t rc = eNoError;
    for(size_t i=0; i < range.size()-1; i++)
    {
        rc |= getPosVelIn(frame, range.start() + i * range.step(), posList[i], velList[i]);
    }
    rc |= getPosVelIn(frame, range.stop(), posList.back(), velList.back());
    return rc;
}

AST_NAMESPACE_END


