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
#include "AstMath/Vector.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"

AST_NAMESPACE_BEGIN

CelestialBody *Point::toBody() const
{
    /// @todo 这里需要优化动态类型转换的执行效率
    return dynamic_cast<CelestialBody*>(const_cast<Point*>(this));
}

errc_t Point::getPosIn(Frame *frame, const TimePoint &tp, Vector3d &pos) const
{
    auto parent = this->getFrame();
    if(parent == frame)
    {
        return getPos(tp, pos);
    }
    else
    {
        Vector3d posInParent;
        errc_t rc = getPos(tp, posInParent);
        if(rc) return rc;
        Transform transform;
        rc = aFrameTransform(parent, frame, tp, transform);
        if(rc) return rc;
        transform.transformPosition(posInParent, pos);
        return eNoError;
    }
}

errc_t Point::getPosVelIn(Frame *frame, const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    auto parent = this->getFrame();
    if(parent == frame)
    {
        return getPosVel(tp, pos, vel);
    }
    else
    {
        Vector3d posInParent, velInParent;
        errc_t rc = getPosVel(tp, posInParent, velInParent);
        if(rc) return rc;
        KinematicTransform transform;
        rc = aFrameTransform(parent, frame, tp, transform);
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


