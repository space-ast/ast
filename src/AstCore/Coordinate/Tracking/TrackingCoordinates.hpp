///
/// @file      TrackingCoordinates.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-01
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 测量/跟踪坐标，包含方位角、仰角和距离
class TrackingCoordinates 
{
public:
    /// @brief 获取方位角
    double azimuth() const { return azimuth_; }
    double& azimuth() { return azimuth_; }
    /// @brief 获取仰角
    double elevation() const { return elevation_; }
    double& elevation() { return elevation_; }
    /// @brief 获取距离
    double range() const { return range_; }
    double& range() { return range_; }
    /// @brief 设置方位角
    void setAzimuth(double az) { azimuth_ = az; }
    /// @brief 设置仰角
    void setElevation(double el) { elevation_ = el; }
    /// @brief 设置距离
    void setRange(double r) { range_ = r; }
public:
    double azimuth_;            ///< 方位角
    double elevation_;          ///< 仰角
    double range_;              ///< 距离
};

using AER = TrackingCoordinates; ///< 别名: AER坐标

/*! @} */

AST_NAMESPACE_END

