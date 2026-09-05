///
/// @file      EphemerisTwoBody.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Ephemeris.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 二体星历
class AST_CORE_API EphemerisTwoBody final: public Ephemeris
{
public:
    static EphemerisTwoBody* New(Frame* frame, const TimePoint& epoch, const CartState& initstate);
    static EphemerisTwoBody* New(Frame* frame, double gm, const TimePoint& epoch, const CartState& initstate);

    EphemerisTwoBody();
    EphemerisTwoBody(Frame* frame, double gm, const TimePoint& epoch, const CartState& initstate);
    ~EphemerisTwoBody() override = default;
    void setFrame(Frame* frame){ frame_ = frame; }
    void setGM(double gm){ gm_ = gm; }
    double getGM() const{ return gm_; }
    void setEpoch(const TimePoint& epoch){ epoch_ = epoch; }
    const TimePoint& getEpoch() const{ return epoch_; }
    void setInitialState(const CartState& initstate){ initstate_ = initstate; }
    void getInitialState(CartState& initstate) const{ initstate = initstate_; }
public:
    Frame* getFrame() const override;
    errc_t getPos(const TimePoint& tp, Vector3d& pos) const override;
    errc_t getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const override;
    errc_t getInterval(TimeInterval& interval) const override;
protected:
    SharedPtr<Frame> frame_{aFrameECI()};
    double           gm_{0.0};
    TimePoint        epoch_{};
    CartState        initstate_{};
};

/*! @} */

AST_NAMESPACE_END
