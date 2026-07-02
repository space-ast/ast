///
/// @file      JacchiaRoberts.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-26
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
#include "AstCore/AtmosphereBase.hpp"
#include "AstCore/SunPosition.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief Jacchia Roberts大气模型
class AST_CORE_API JacchiaRoberts final: public AtmosphereBase
{
public:
    JacchiaRoberts(Frame* frame, BodyShape* bodyShape, CelestialBody* sun, double f107Daily, double f107Average, double kp);
    
    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;

    void setSunPosition(ESunPosition sunPosition){sunPosition_ = sunPosition;}
    ESunPosition getSunPosition() const {return sunPosition_;}
private:
    CelestialBody* sun_{nullptr};
    double F107Daily_{0.0};
    double F107Average_{0.0};
    double kp_{0};
    ESunPosition sunPosition_{ESunPosition::eTrue};
};

/*! @} */

AST_NAMESPACE_END
