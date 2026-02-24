///
/// @file      CriticallyInclinedSunSyncOrbitDesigner.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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
#include "AstCore/OrbitDesigner.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief      太阳同步临界倾角轨道设计器
class AST_CORE_API CriticallyInclinedSunSyncOrbitDesigner final: public OrbitDesigner
{
public:
    CriticallyInclinedSunSyncOrbitDesigner();
    CriticallyInclinedSunSyncOrbitDesigner(CelestialBody* body);
    ~CriticallyInclinedSunSyncOrbitDesigner() = default;

    using OrbitDesigner::getOrbitState;

    err_t getOrbitState(ModOrbElem& orbElem) const override;

    double getPerigeeAltitude() const { return perigeeAltitude_; }

    double getRAAN() const { return raan_; }

    void setPerigeeAltitude(double perigeeAltitude) { perigeeAltitude_ = perigeeAltitude; }

    void setRAAN(double raan) { raan_ = raan; }
protected:
    double perigeeAltitude_;     ///< 近地点高度
    double raan_;                ///< 升交点赤经
};

/*! @} */

AST_NAMESPACE_END
