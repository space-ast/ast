///
/// @file      GeocentricPoint.hpp
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


/// @brief 地心坐标
/// @details 地心坐标是指以天体中心为原点，以天体中心到空间中任意点的连线为坐标轴的坐标系统。
/// 地心坐标的纬度是从地表点到天体质心的连线与赤道平面的夹角
class GeocentricPoint: public GeoCoordinate
{
public:
    using GeoCoordinate::GeoCoordinate;
};



AST_NAMESPACE_END
