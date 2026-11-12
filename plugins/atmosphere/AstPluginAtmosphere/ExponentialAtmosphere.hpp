///
/// @file      ExponentialAtmosphere.hpp
/// @brief     指数大气模型
/// @details   
/// @author    axel
/// @date      2026-10-14
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
#include "AstPluginAtmosphereGlobal.hpp"

AST_NAMESPACE_BEGIN

/// @brief 指数大气模型
/// @details 密度随高度按单一标高指数衰减：
/// @code
///   rho(h) = rho0 * exp(-(h - h0) / H)
/// @endcode
/// 它不是精确模型，只用于做粗略的量级估算。
class AST_PLUGIN_ATMOSPHERE_API ExponentialAtmosphere : public AtmosphereBase
{
public:
    /// @brief 构造函数
    /// @param frame 参考坐标系（固连系）
    /// @param bodyShape 天体形状，用于由位置反解高度
    /// @param density0 参考高度上的密度 [kg/m^3]
    /// @param height0 参考高度 [m]
    /// @param scaleHeight 标高 [m]
    ExponentialAtmosphere(Frame* frame, BodyShape* bodyShape, double density0 = 1.225, double height0 = 0.0, double scaleHeight = 8500.0);

    double getDensity(const TimePoint& tp, const Vector3d& posInBodyFixed) const override;

private:
    double density0_{1.225};      ///< 参考高度处的密度 [kg/m^3]
    double height0_{0.0};         ///< 参考高度 [m]
    double scaleHeight_{8500.0};  ///< 标高 [m]
};

AST_NAMESPACE_END
