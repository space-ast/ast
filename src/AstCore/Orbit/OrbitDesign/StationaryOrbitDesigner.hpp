///
/// @file      StationaryOrbitDesigner.hpp
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
#include "BaseOrbitDesigner.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Orbit
    @{
*/


/// @brief 静止轨道设计器
class AST_CORE_API StationaryOrbitDesigner final: public BaseOrbitDesigner
{
public:
    StationaryOrbitDesigner();
    StationaryOrbitDesigner(CelestialBody *body);
    ~StationaryOrbitDesigner() = default;

    using BaseOrbitDesigner::getOrbitState;
    
    err_t getOrbitState(ModOrbElem &orbElem) const override;

    /// @brief 获取星下点经度
    double getSubsatellitePoint() const { return subsatellitePoint_; }

    /// @brief 获取轨道倾角
    double getInclination() const { return inclination_; }

    /// @brief 设置星下点经度
    void setSubsatellitePoint(double subsatellitePoint) { subsatellitePoint_ = subsatellitePoint; }

    /// @brief 设置轨道倾角
    void setInclination(double inclination) { inclination_ = inclination; }
protected:
    double subsatellitePoint_{0};       ///< 星下点经度
    double inclination_{0};             ///< 轨道倾角
};

/*! @} */

AST_NAMESPACE_END
