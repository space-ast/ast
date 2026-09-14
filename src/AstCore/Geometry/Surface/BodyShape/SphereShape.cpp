///
/// @file      SphereShape.cpp
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

#include "SphereShape.hpp"
#include "AstCore/LocalHorizonalFrame.hpp"

AST_NAMESPACE_BEGIN

SphereShape::SphereShape(double radius)
    : radius_(radius)
{
}

void SphereShape::transform(const Vector3d& cartesian, GeodeticPoint& detic) const
{
    aBodyFixedToGeodetic(cartesian, detic, radius_);
}

void SphereShape::transform(const GeodeticPoint& detic, Vector3d& cartesian) const
{
    aGeodeticToBodyFixed(detic, cartesian, radius_);
}

errc_t SphereShape::transform(const Vector3d& pos, const Vector3d& vel,
                              GeodeticPoint& detic, LatLonAlt& rate) const
{
    // 圆球是零扁率的扁球体: e²=0 时 N=M=radius, 位置与速度的实现均精确退化
    return aBodyFixedToGeodetic(pos, vel, detic, rate, radius_, 0.0);
}

errc_t SphereShape::transform(const GeodeticPoint& detic, const LatLonAlt& rate,
                              Vector3d& pos, Vector3d& vel) const
{
    return aGeodeticToBodyFixed(detic, rate, pos, vel, radius_, 0.0);
}


AST_NAMESPACE_END

