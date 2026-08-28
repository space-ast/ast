///
/// @file      EphemerisTwoBody.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-13
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

#include "EphemerisTwoBody.hpp"
#include "AstCore/TwoBody.hpp"
#include "AstCore/BuiltinFrame.hpp"

AST_NAMESPACE_BEGIN

EphemerisTwoBody::EphemerisTwoBody()
    : frame_{aFrameECI()}
{
    gm_ = frame_->getGM();
}

EphemerisTwoBody::EphemerisTwoBody(Frame *frame, double gm, const TimePoint &epoch, const CartState &initstate)
    : frame_{frame}
    , gm_{gm}
    , epoch_{epoch}
    , initstate_{initstate}
{
}

EphemerisTwoBody *EphemerisTwoBody::New(Frame *frame, const TimePoint &epoch, const CartState &initstate)
{
    // if(!frame){
    //     aError("frame is nullptr");
    //     return nullptr;
    // }
    return new EphemerisTwoBody(frame, frame?frame->getGM():0.0, epoch, initstate);
}

EphemerisTwoBody *EphemerisTwoBody::New(Frame *frame, double gm, const TimePoint &epoch, const CartState &initstate)
{
    return new EphemerisTwoBody(frame, gm, epoch, initstate);
}

Frame *EphemerisTwoBody::getFrame() const
{
    return frame_.get();
}

errc_t EphemerisTwoBody::getPos(const TimePoint &tp, Vector3d &pos) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initstate_.pos();
    Vector3d vel = initstate_.vel();
    return aTwoBodyProp(duration, gm_, pos, vel);
}

errc_t EphemerisTwoBody::getPosVel(const TimePoint &tp, Vector3d &pos, Vector3d &vel) const
{
    double duration = tp.durationFrom(epoch_);
    pos = initstate_.pos();
    vel = initstate_.vel();
    return aTwoBodyProp(duration, gm_, pos, vel);
}

errc_t EphemerisTwoBody::getInterval(TimeInterval &interval) const
{
    interval.setWhole();
    return eNoError;
}

AST_NAMESPACE_END

