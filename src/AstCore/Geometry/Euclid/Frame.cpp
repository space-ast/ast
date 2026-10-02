///
/// @file      Frame.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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

#include "Frame.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/Resolve.hpp"
#include "AstMath/Transform.hpp"
#include "AstMath/KinematicTransform.hpp"
#include "AstMath/AccelerationTransform.hpp"

AST_NAMESPACE_BEGIN


std::string Frame::getRepresentation() const
{
    auto parent = getParentScope();
    if(parent)
        return parent->getRepresentation() + " " + getName();
    return getName();
}

bool Frame::equals(const Frame &other) const
{
    if(this == &other)
        return true;
    if(this->getOrigin() != other.getOrigin())
        return false;
    if(this->getAxes() != other.getAxes())
        return false;
    return true;
}

Frame* Frame::Resolve(StringView value)
{
    return aResolveFrame(value);
}


CelestialBody *Frame::getBody()
{
    if(auto origin = getOrigin())
    {
        return origin->toBody();
    }
    return nullptr;
}

double Frame::getGM()
{
    if(auto body = getBody())
        return body->getGM();
    return 0.0;
}

Frame *Frame::getParent() const
{
    if(auto origin = getOrigin())
        return origin->getFrame();
    return nullptr;
}

errc_t Frame::getTransform(const TimePoint &tp, Transform &transform) const
{
    auto parent = getParent();
    if(!parent)
        return eErrorInvalidParam;
    if(auto origin = getOrigin())
    {
        errc_t rc = origin->getPosIn(parent, tp, transform.getTranslation());
        if(rc != 0)
            return rc;
    }
    if(auto axes = getAxes())
    {
        errc_t rc = axes->getTransformFrom(parent->getAxes(), tp, transform.getRotation());
        if(rc != 0)
            return rc;
    }
    return eNoError;
}

errc_t Frame::getTransform(const TimePoint &tp, KinematicTransform &transform) const
{
    auto parent = getParent();
    if(!parent)
        return eErrorInvalidParam;
    if(auto origin = getOrigin())
    {
        errc_t rc = origin->getPosVelIn(parent, tp, transform.getTranslation(), transform.getVelocity());
        if(rc != 0)
            return rc;
    }
    if(auto axes = getAxes())
    {
        errc_t rc = axes->getTransformFrom(parent->getAxes(), tp, transform.getKinematicRotation());
        if(rc != 0)
            return rc;
    }
    return eNoError;
}

errc_t Frame::getTransform(const TimePoint &tp, AccelerationTransform &transform) const
{
    /*!
        默认实现：与运动学版本同构，平移项由原点的位置/速度/加速度给出，
        旋转项由本坐标系轴系相对父坐标系轴系的加速度旋转变换给出。
        平移加速度的解析解由 Point::getPosVelAcc 提供，没有解析解的点走其默认差分实现；
        旋转角加速度的解析解由 Axes::getTransform(AccelerationRotation) 提供，没有解析解的轴系走其默认差分实现。
    */
    auto parent = getParent();
    if(!parent)
        return eErrorInvalidParam;
    if(auto origin = getOrigin())
    {
        errc_t rc = origin->getPosVelAccIn(parent, tp, transform.translation(), transform.velocity(), transform.acceleration());
        if(rc != 0)
            return rc;
    }
    if(auto axes = getAxes())
    {
        errc_t rc = axes->getTransformFrom(parent->getAxes(), tp, transform.accelerationRotation());
        if(rc != 0)
            return rc;
    }
    return eNoError;
}

AST_NAMESPACE_END
