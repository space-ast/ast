///
/// @file      VectorDisplacement.cpp
/// @brief     位移向量类实现
/// @details   
/// @author    axel
/// @date      2026-05-14
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///

#include "VectorDisplacement.hpp"
#include "AstCore/Frame.hpp"

AST_NAMESPACE_BEGIN

void VectorDisplacement::setOrigin(Point* origin)
{
    origin_ = origin;
}

Point* VectorDisplacement::getOrigin() const
{
    return origin_.get();
}

void VectorDisplacement::setDestination(Point* destination)
{
    destination_ = destination;
}

Point* VectorDisplacement::getDestination() const
{
    return destination_.get();
}

Axes* VectorDisplacement::getAxes() const
{
    Frame* frame = getReferenceFrame();
    if (frame)
    {
        return frame->getAxes();
    }
    return nullptr;
}


errc_t VectorDisplacement::getVector(const TimePoint& tp, Vector3d& vec) const
{
    if (!origin_ || !destination_)
    {
        return eErrorNullInput;
    }

    Frame* targetFrame = getReferenceFrame();
    if (!targetFrame)
    {
        return eErrorNullInput;
    }

    Vector3d posOrigin, posDest;
    errc_t err = origin_->getPosIn(targetFrame, tp, posOrigin);
    if (err != eNoError)
    {
        return err;
    }

    err = destination_->getPosIn(targetFrame, tp, posDest);
    if (err != eNoError)
    {
        return err;
    }

    vec = posDest - posOrigin;
    return eNoError;
}


errc_t VectorDisplacement::getVector(const TimePoint& tp, Vector3d& vec, Vector3d& vel) const
{
    if (!origin_ || !destination_)
    {
        return eErrorNullInput;
    }

    Frame* targetFrame = getReferenceFrame();
    if (!targetFrame)
    {
        return eErrorNullInput;
    }

    Vector3d posOrigin, velOrigin, posDest, velDest;
    errc_t err = origin_->getPosVelIn(targetFrame, tp, posOrigin, velOrigin);
    if (err != eNoError)
    {
        return err;
    }

    err = destination_->getPosVelIn(targetFrame, tp, posDest, velDest);
    if (err != eNoError)
    {
        return err;
    }

    vec = posDest - posOrigin;
    vel = velDest - velOrigin;
    return eNoError;
}


Frame* VectorDisplacement::getReferenceFrame() const
{
    if (auto origin = origin_.get())
    {
        return origin->getFrame();
    }
    return nullptr;
}

AST_NAMESPACE_END