///
/// @file      EphemerisVinti.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-05
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

#include "EphemerisVinti.hpp"
#include "AstCore/Vinti.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

EphemerisVinti::EphemerisVinti()
    : frame_{aFrameECI()}
{
    if(auto body = frame_->getBody())
    {
        gm_ = body->getGM();
        j2_ = body->getJ2();
        j3_ = body->getJ3();
        re_ = body->getGravityRefDistance();
        if(re_ <= 0.0)
            re_ = body->getRadius();
    }
}

EphemerisVinti::EphemerisVinti(Frame *frame, double gm, double j2, double j3, double re, const TimePoint &epoch, const CartState &initstate)
    : frame_{frame}
    , gm_{gm}
    , j2_{j2}
    , j3_{j3}
    , re_{re}
    , epoch_{epoch}
    , initstate_{initstate}
{
}

EphemerisVinti *EphemerisVinti::New(Frame *frame, const TimePoint &epoch, const CartState &initstate)
{
    double gm = 0.0, j2 = 0.0, j3 = 0.0, re = 0.0;
    if(frame && frame->getBody())
    {
        auto body = frame->getBody();
        gm = body->getGM();
        j2 = body->getJ2();
        j3 = body->getJ3();
        re = body->getGravityRefDistance();
        if(re <= 0.0)
            re = body->getRadius();
    }
    return new EphemerisVinti(frame, gm, j2, j3, re, epoch, initstate);
}

EphemerisVinti *EphemerisVinti::New(Frame *frame, double gm, double j2, double j3, double re, const TimePoint &epoch, const CartState &initstate)
{
    return new EphemerisVinti(frame, gm, j2, j3, re, epoch, initstate);
}

Frame *EphemerisVinti::getFrame() const
{
    return frame_.get();
}

errc_t EphemerisVinti::getPos(const TimePoint &tp, Vector3d &pos) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initstate_.pos();
    Vector3d vel = initstate_.vel();
    return aVinti(duration, gm_, j2_, j3_, re_, pos, vel);
}

errc_t EphemerisVinti::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initstate_.pos();
    vel = initstate_.vel();
    return aVinti(duration, gm_, j2_, j3_, re_, pos, vel);
}

errc_t EphemerisVinti::getInterval(TimeInterval &interval) const
{
    interval.setWhole();
    return eNoError;
}

AST_NAMESPACE_END
