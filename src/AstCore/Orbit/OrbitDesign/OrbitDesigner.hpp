///
/// @file      OrbitDesigner.hpp
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

    /// @brief 获取轨道状态
    /// @param orbitEpoch 轨道历元（可选）
    /// @param orbElem 轨道根数
    virtual err_t getOrbitState(TimePoint& orbitEpoch, ModOrbElem &orbElem) const = 0;

    /// @brief 获取坐标系定义
    /// @param useCoordEpoch 是否使用坐标系历元
    /// @param coordEpoch 坐标系历元（可选）
    /// @param coordFrame 坐标系
    virtual err_t getCoordFrame(bool& useCoordEpoch, TimePoint& coordEpoch, SharedPtr<Frame>& coordFrame) const = 0;
};

/// @brief 轨道设计器
class AST_CORE_API OrbitDesigner: public IOrbitDesigner
{
public:
    OrbitDesigner() = default;
    OrbitDesigner(CelestialBody* cb);
    ~OrbitDesigner() = default;
protected:
    SharedPtr<CelestialBody> centralBody_;  ///< 中心天体
};

/*! @} */

AST_NAMESPACE_END
