///
/// @file      AttitudeSunRelated.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-05
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

#include "AttitudeSunRelated.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/RunTime.hpp"

AST_NAMESPACE_BEGIN


AttitudeSunRelated::AttitudeSunRelated()
    : sun_{aGetSun()}
{
}

errc_t AttitudeSunRelated::getSunPosLocal(const TimePoint& tp, Vector3d& sunPos) const
{
    auto frame = this->frame();
    auto sun = this->sun();
    if(!frame || !sun) return eErrorNullPtr;
    return sun->getPosIn(frame, tp, sunPos);
}

errc_t AttitudeSunRelated::getSunPosVelLocal(const TimePoint& tp, Vector3d& sunPos, Vector3d& sunVel) const
{
    auto frame = this->frame();
    auto sun = this->sun();
    if(!frame || !sun) return eErrorNullPtr;
    return sun->getPosVelIn(frame, tp, sunPos, sunVel);
}

AST_NAMESPACE_END
