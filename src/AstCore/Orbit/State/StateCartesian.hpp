///
/// @file      StateCartesian.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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
#include "AstCore/State.hpp"
#include "AstMath/Vector.hpp"
#include "AstCore/OrbitElement.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/
class StateCartesian;
using PStateCartesian = StateCartesian*;
using HStateCartesian = SharedPtr<StateCartesian>;

/// @brief 笛卡尔状态(直角坐标)
class AST_CORE_API StateCartesian final: public State
{
public:
    AST_OBJECT(StateCartesian)
    AST_PROPERT(x)
    AST_PROPERT(y)
    AST_PROPERT(z)
    AST_PROPERT(vx)
    AST_PROPERT(vy)
    AST_PROPERT(vz)
    static PStateCartesian New();
    static HStateCartesian MakeShared();
    static PStateCartesian New(const CartState& state);
    static HStateCartesian MakeShared(const CartState& state);
    
    StateCartesian() = default;
    StateCartesian(const CartState& state);
    StateCartesian(const State& state);
    ~StateCartesian() override = default;
public:
    EStateType getStateType() const override { return EStateType::eCartesian; }
    errc_t getState(CartState& state) const override;
    errc_t getState(ModOrbElem& orbElem) const override;
    errc_t setState(const CartState& state) override;
    errc_t setState(const ModOrbElem& orbElem) override;
PROPERTIES:
    length_d x() const { return cartState_.x(); }
    length_d y() const { return cartState_.y(); }
    length_d z() const { return cartState_.z(); }
    speed_d vx() const { return cartState_.vx(); }
    speed_d vy() const { return cartState_.vy(); }
    speed_d vz() const { return cartState_.vz(); }
    void setX(length_d x){ cartState_.x() = x; }
    void setY(length_d y){ cartState_.y() = y; }
    void setZ(length_d z){ cartState_.z() = z; }
    void setVx(speed_d vx){ cartState_.vx() = vx; }
    void setVy(speed_d vy){ cartState_.vy() = vy; }
    void setVz(speed_d vz){ cartState_.vz() = vz; }
protected:
    CartState cartState_{};
};



/*! @} */

AST_NAMESPACE_END