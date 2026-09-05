///
/// @file      EphemerisVinti.hpp
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


/// @brief Vinti 解析解星历（含 J2 / J3 扁率项）
/// @details 内部调用 aVinti() 进行闭式轨道预报，
///          需要引力常数 gm、J2/J3 系数与天体赤道半径 re。
class AST_CORE_API EphemerisVinti final: public Ephemeris
{
public:
    static EphemerisVinti* New(Frame* frame, const TimePoint& epoch, const CartState& initstate);
    static EphemerisVinti* New(Frame* frame, double gm, double j2, double j3, double re, const TimePoint& epoch, const CartState& initstate);

    EphemerisVinti();
    EphemerisVinti(Frame* frame, double gm, double j2, double j3, double re, const TimePoint& epoch, const CartState& initstate);
    ~EphemerisVinti() override = default;
    void setFrame(Frame* frame){ frame_ = frame; }
    void setGM(double gm){ gm_ = gm; }
    void setJ2(double j2){ j2_ = j2; }
    void setJ3(double j3){ j3_ = j3; }
    void setRe(double re){ re_ = re; }
    double getGM() const{ return gm_; }
    double getJ2() const{ return j2_; }
    double getJ3() const{ return j3_; }
    double getRe() const{ return re_; }
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
    double           gm_{0.0};     ///< 引力常数 [m³/s²]
    double           j2_{0.0};     ///< J2 带谐系数
    double           j3_{0.0};     ///< J3 带谐系数
    double           re_{0.0};     ///< 天体赤道半径 [m]
    TimePoint        epoch_{};
    CartState        initstate_{};
};

/*! @} */

AST_NAMESPACE_END
