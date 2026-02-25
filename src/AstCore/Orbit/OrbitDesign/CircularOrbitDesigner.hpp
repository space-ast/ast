///
/// @file      CircularOrbitDesigner.hpp
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

/// @brief 圆轨道设计器
class CircularOrbitDesigner final: public BaseOrbitDesigner
{
public:
    CircularOrbitDesigner();
    CircularOrbitDesigner(CelestialBody* body);
    ~CircularOrbitDesigner() = default;
public:
    
    using BaseOrbitDesigner::getOrbitState;

    err_t getOrbitState(ModOrbElem &orbElem) const override;

    /// @brief 获取轨道倾角
    double getInclination() const { return inclination_; }

    /// @brief 获取轨道高度
    double getAltitude() const { return altitude_; }

    /// @brief 获取升交点赤经
    double getRAAN() const { return raan_; }

    /// @brief 设置轨道倾角
    void setInclination(double inclination) { inclination_ = inclination; }

    /// @brief 设置轨道高度
    void setAltitude(double altitude) { altitude_ = altitude; }

    /// @brief 设置升交点赤经
    void setRAAN(double raan) { raan_ = raan; }

protected:
    double inclination_{0};    ///< 轨道倾角
    double altitude_{0};       ///< 轨道高度
    double raan_{0};           ///< 升交点赤经
};

/*! @} */

AST_NAMESPACE_END
