///
/// @file      BurnImpulsive.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-18
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/Burn.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 脉冲点火
class AST_CORE_API BurnImpulsive: public Burn
{
public:
    AST_OBJECT(BurnImpulsive)
    AST_PROPERT(x)
    AST_PROPERT(y)
    AST_PROPERT(z)
    AST_PROPERT(axes)
    BurnImpulsive() = default;
    ~BurnImpulsive() = default;
    
PROPERTIES:
    double x() const {return impulse_.x();}
    void setX(double value) {impulse_.x() = value;}

    double y() const {return impulse_.y();}
    void setY(double value) {impulse_.y() = value;}
    
    double z() const {return impulse_.z();}
    void setZ(double value) {impulse_.z() = value;}

    Axes* axes() const {return axes_.get();}
    void setAxes(Axes* value) {axes_ = value;}
public:
    const Vector3d& impulse() const {return impulse_;}
    void setImpulse(const Vector3d& value) {impulse_ = value;}
private:
    WeakPtr<Axes> axes_;
    Vector3d impulse_;
};



/*! @} */

AST_NAMESPACE_END
