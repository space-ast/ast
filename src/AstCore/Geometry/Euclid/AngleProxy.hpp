///
/// @file      AngleProxy.hpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-10-09
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
#include "Angle.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Geometry
    @{
*/

/// @brief 角度代理类
/// @details 将角度接口原样转发给被代理的角度对象，自身不产生任何计算。
class AST_CORE_API AngleProxy final: public Angle
{
public:
    AngleProxy() = default;
    AngleProxy(Angle* angle):impl_(angle){}
    ~AngleProxy() override = default;

public:
    void setImpl(Angle* impl){impl_ = impl;}
    Angle* impl() const {return impl_.get();}
public:
    errc_t getAngle(const TimePoint& tp, double& value) const override;
    errc_t getAngle(const TimePoint& tp, double& value, double& angVel) const override;
private:
    WeakPtr<Angle> impl_{};
};




/*! @} */

AST_NAMESPACE_END
