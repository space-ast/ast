///
/// @file      GeodeticPoint.hpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
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
#include "GeoCoordinate.hpp"

AST_NAMESPACE_BEGIN


/// @brief 大地坐标
/// @details 大地坐标是指以天体表面为参考椭球，以大地纬度、经度、高度为坐标的坐标系统。
/// 大地坐标的纬度是地表某点的法线（垂直于地球表面）与赤道平面的夹角
class GeodeticPoint: public GeoCoordinate
{
public:
    using GeoCoordinate::GeoCoordinate;

};

AST_NAMESPACE_END
