///
/// @file      BaseOrbitDesigner.hpp
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
#include "AstCore/OrbitElement.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/SharedPtr.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup Orbit
    @{
*/

/// @brief 轨道设计器接口
class AST_CORE_API IOrbitDesigner
{
public:
    IOrbitDesigner() = default;
    ~IOrbitDesigner() = default;

    virtual err_t getOrbitEpoch(TimePoint& orbitEpoch) const = 0;

    /// @brief 获取轨道状态
    /// @param orbitEpoch 轨道历元（可选）
    /// @param orbElem 轨道根数
    virtual err_t getOrbitState(ModOrbElem &orbElem) const = 0;

    /// @brief 获取坐标系定义
    /// @param useCoordEpoch 是否使用坐标系历元
    /// @param coordEpoch 坐标系历元（可选）
    /// @param coordFrame 坐标系
    virtual err_t getCoordFrame(bool& useCoordEpoch, TimePoint& coordEpoch, SharedPtr<Frame>& coordFrame) const = 0;
};


/// @brief 轨道设计器基类
class AST_CORE_API BaseOrbitDesigner: public IOrbitDesigner
{
public:
    BaseOrbitDesigner();
    BaseOrbitDesigner(CelestialBody* body);
    ~BaseOrbitDesigner() = default;

    using IOrbitDesigner::getOrbitState;

    static CelestialBody* getDefaultCelestialBody();

    /// @brief 获取天体
    CelestialBody* getCelestialBody() const { return celestialBody_.get(); }

    /// @brief 获取引力参数
    double getGM() const { return celestialBody_?celestialBody_->getGM(): 0; }
    
    /// @brief 获取J2项
    double getJ2() const {return celestialBody_?celestialBody_->getJ2():0; }

    /// @brief 获取天体半径
    double getBodyRadius() const { return celestialBody_?celestialBody_->getRadius():0; }

    /// @brief 获取轨道历元
    err_t getOrbitEpoch(TimePoint& orbitEpoch) const final{ orbitEpoch = orbitEpoch_; return 0; };

    /// @brief 获取轨道状态
    /// @param cartState 笛卡尔状态
    err_t getOrbitState(CartState &cartState) const;

    /// @brief 获取轨道状态
    /// @param orbElem 轨道根数
    err_t getOrbitState(OrbElem &orbElem) const;

    err_t getCoordFrame(bool& useCoordEpoch, TimePoint& coordEpoch, SharedPtr<Frame>& coordFrame) const override;
protected:
    SharedPtr<CelestialBody> celestialBody_;    ///< 天体
    TimePoint orbitEpoch_{};    ///< 轨道历元
};


/*! @} */

AST_NAMESPACE_END
