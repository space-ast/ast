///
/// @file      AttitudeSunRelated.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AttitudeTrajectoryRelated.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 太阳相关姿态
class AST_CORE_API AttitudeSunRelated : public AttitudeTrajectoryRelated
{
public:
    AttitudeSunRelated();
    ~AttitudeSunRelated() = default;
public:
    void setSun(Body* sun){sun_ = sun;}
    Body* sun() const { return sun_.get(); }
protected:
    errc_t getSunPosLocal(const TimePoint& tp, Vector3d& sunPos) const;
    errc_t getSunPosVelLocal(const TimePoint& tp, Vector3d& sunPos, Vector3d& sunVel) const;
private:
    WeakPtr<CelestialBody> sun_{};  ///< 太阳(光源天体)
};


/*! @} */

AST_NAMESPACE_END
