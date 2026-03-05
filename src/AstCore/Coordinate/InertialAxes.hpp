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


/// @brief      J2000 到 B1950 的坐标旋转变换（Vallado 版本）
AST_CORE_CAPI void aJ2000ToB1950Transform_Vallado(Rotation& rotation);


/// @brief      J2000 到 B1950 的坐标旋转变换（SPICE 版本）
AST_CORE_CAPI void aJ2000ToB1950Transform_SPICE(Rotation& rotation);


/// @brief      B1950 到 FK4 的坐标旋转变换
AST_CORE_CAPI void aB1950ToFK4Transform(Rotation& rotation);


/// @brief      B1950 到 DE-118 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE118Transform(Rotation& rotation);


/// @brief      B1950 到 DE-96 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE96Transform(Rotation& rotation);


/// @brief      B1950 到 DE-102 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE102Transform(Rotation& rotation);


/// @brief      B1950 到 DE-108 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE108Transform(Rotation& rotation);


/// @brief      B1950 到 DE-111 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE111Transform(Rotation& rotation);


/// @brief      B1950 到 DE-114 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE114Transform(Rotation& rotation);


/// @brief      B1950 到 DE-122 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE122Transform(Rotation& rotation);


/// @brief      B1950 到 DE-125 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE125Transform(Rotation& rotation);


/// @brief      B1950 到 DE-130 的坐标旋转变换
AST_CORE_CAPI void aB1950ToDE130Transform(Rotation& rotation);


/// @brief      FK4 到 Galactic 的坐标旋转变换
AST_CORE_CAPI void aFK4ToGalacticTransform(Rotation& rotation);


/// @brief      J2000 到 DE-200 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToDE200Transform(Rotation& rotation);


/// @brief      J2000 到 DE-202 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToDE202Transform(Rotation& rotation);


/// @brief      J2000 到 MarsIAU 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToMarsIAUTransform(Rotation& rotation);


/// @brief      J2000 到 EclipJ2000 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToEclipJ2000Transform(Rotation& rotation);


/// @brief      B1950 到 EclipB1950 的坐标旋转变换
AST_CORE_CAPI void aB1950ToEclipB1950Transform(Rotation& rotation);


/// @brief      J2000 到 DE-140 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToDE140Transform(Rotation& rotation);


/// @brief      J2000 到 DE-142 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToDE142Transform(Rotation& rotation);


/// @brief      J2000 到 DE-143 的坐标旋转变换
AST_CORE_CAPI void aJ2000ToDE143Transform(Rotation& rotation);

/*! @} */

AST_NAMESPACE_END
