///
/// @file      LatLonAlt.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

AST_NAMESPACE_BEGIN


/// @brief 纬度、经度、高度坐标
class LatLonAlt
{
public:
    /// @brief 获取纬度
    double latitude() const { return lat_; }
    /// @brief 获取经度
    double longitude() const { return lon_; }
    /// @brief 获取高度
    double altitude() const { return alt_; }
    /// @brief 设置纬度
    double& latitude() { return lat_; }
    /// @brief 设置经度
    double& longitude() { return lon_; }
    /// @brief 设置高度
    double& altitude() { return alt_; }
    /// @brief 设置纬度
    void setLatitude(double lat) { lat_ = lat; }
    /// @brief 设置经度
    void setLongitude(double lon) { lon_ = lon; }
    /// @brief 设置高度
    void setAltitude(double alt) { alt_ = alt; }

properties:
    angle_d  lat_;  ///< 纬度
    angle_d  lon_;  ///< 经度
    length_d alt_;  ///< 高度
};


AST_NAMESPACE_END
