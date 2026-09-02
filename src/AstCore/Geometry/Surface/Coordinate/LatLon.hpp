///
/// @file      LatLon.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-08-28
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

/// @brief 纬度经度坐标
class LatLon
{
public:
    LatLon() = default;
    LatLon(double lat, double lon) : lat_(lat), lon_(lon) {}

    double latitude() const { return lat_; }
    double longitude() const { return lon_; }
    double& latitude() { return lat_; }
    double& longitude() { return lon_; }
    void setLatitude(double lat) { lat_ = lat; }
    void setLongitude(double lon) { lon_ = lon; }
PROPERTIES:
    angle_d  lat_;  ///< 纬度
    angle_d  lon_;  ///< 经度
};

using LatLng = LatLon;

/*! @} */

AST_NAMESPACE_END
