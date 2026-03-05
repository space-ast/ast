///
/// @file      InertialAxes.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Coordinate
    @{
*/

class Rotation;


/// @brief      J2000 到 B1950 的转换矩阵
AST_CORE_CAPI void aJ2000ToB1950Matrix(Matrix3d& m);


/// @brief      J2000 到 B1950 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToB1950Transform(Rotation& rotation);


/// @brief      B1950 到 J2000 的转换矩阵
AST_CORE_CAPI void aB1950ToJ2000Matrix(Matrix3d& m);


/// @brief      B1950 到 J2000 的坐标旋转变换
AST_CORE_CAPI void aB1950ToJ2000Transform(Rotation& rotation);


/// @brief      J2000 到 B1950 转换矩阵（SPICE 版本）
AST_CORE_CAPI void aJ2000ToB1950Matrix_SPICE(Matrix3d& m);


/// @brief      J2000 到 B1950 的坐标旋转变换（SPICE 版本）
AST_CORE_CAPI void aJ2000ToB1950Transform_SPICE(Rotation& rotation);


A_ALWAYS_INLINE void aJ2000ToB1950SpiceTransform(Rotation& rotation)
{
    aJ2000ToB1950Transform_SPICE(rotation);
}


/// @brief      B1950 到 J2000 转换矩阵（SPICE 版本）
AST_CORE_CAPI void aB1950ToJ2000Matrix_SPICE(Matrix3d& m);


/// @brief      B1950 到 J2000 的坐标旋转变换（SPICE 版本）
AST_CORE_CAPI void aB1950ToJ2000Transform_SPICE(Rotation& rotation);


/// @brief      B1950 到 FK4 的坐标旋转变换
AST_CORE_CAPI void aB1950ToFK4Transform(Rotation& rotation);


/*! @} */

AST_NAMESPACE_END
