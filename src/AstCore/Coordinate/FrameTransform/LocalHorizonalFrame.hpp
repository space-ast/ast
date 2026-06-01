///
/// @file      LocalHorizonalFrame.hpp
/// @brief     局部水平坐标转换
/// @details   提供大地坐标到NED坐标、ENU坐标转换函数
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
#include "AstCore/GeodeticPoint.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup
    @{
*/

/// @brief 大地坐标(天体固连系)转NED坐标系的旋转变换
AST_CORE_CAPI void aGeodeticToNEDTransform(const GeodeticPoint& origin, Rotation& rot);

/// @brief 大地坐标(天体固连系)转ENU坐标系的旋转变换
AST_CORE_CAPI void aGeodeticToENUTransform(const GeodeticPoint& origin, Rotation& rot);

/// @brief 大地坐标转NED坐标
/// @param point 大地坐标
/// @param origin 局部NED系原点
/// @param ned 输出的NED坐标
AST_CORE_CAPI void aGeodeticToNED(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& ned, BodyShape* bodyShape);

/// @brief NED坐标转大地坐标
/// @param ned NED坐标
/// @param origin 局部NED系原点
/// @param point 输出的大地坐标
AST_CORE_CAPI void aNEDToGeodetic(const Vector3d& ned, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape);

/// @brief 大地坐标转ENU坐标
/// @param point 大地坐标
/// @param origin 局部ENU系原点
/// @param enu 输出的ENU坐标
AST_CORE_CAPI void aGeodeticToENU(const GeodeticPoint& point, const GeodeticPoint& origin, Vector3d& enu, BodyShape* bodyShape);

/// @brief ENU坐标转大地坐标
/// @param enu ENU坐标
/// @param origin 局部ENU系原点
/// @param point 输出的大地坐标
AST_CORE_CAPI void aENUToGeodetic(const Vector3d& enu, const GeodeticPoint& origin, GeodeticPoint& point, BodyShape* bodyShape);


// ----------------------------------
// MATLAB Navigation Toolbox 同名函数
// ----------------------------------


A_ALWAYS_INLINE Vector3d lla2ned(const GeodeticPoint& lla, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    Vector3d ned;
    aGeodeticToNED(lla, lla0, ned, bodyShape);
    return ned;
}

A_ALWAYS_INLINE GeodeticPoint ned2lla(const Vector3d& ned, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    GeodeticPoint target;
    aNEDToGeodetic(ned, lla0, target, bodyShape);
    return target;
}

A_ALWAYS_INLINE Vector3d lla2enu(const GeodeticPoint& lla, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    Vector3d enu;
    aGeodeticToENU(lla, lla0, enu, bodyShape);
    return enu;
}

A_ALWAYS_INLINE GeodeticPoint enu2lla(const Vector3d& enu, const GeodeticPoint& lla0, BodyShape* bodyShape)
{
    GeodeticPoint target;
    aENUToGeodetic(enu, lla0, target, bodyShape);
    return target;
}


/*! @} */

AST_NAMESPACE_END
