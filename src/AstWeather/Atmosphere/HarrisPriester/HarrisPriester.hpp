///
/// @file      HarrisPriester.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-26
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

/// @brief 通过HarrisPriester模型计算大气密度
/// @param pos 位置 [m]
/// @param sunpos 太阳位置 [m]
/// @param altitude 海拔高度 [m]
/// @param f107 F10.7 太阳辐射通量
/// @param n 余弦指数 [2-6]
/// @return 大气密度 [kg/m³]
/// @warning 输入的位置向量和太阳位置向量必须在同一个坐标系下
AST_WEATHER_CAPI double aHarrisPriester(
    const Vector3d& pos,
    const Vector3d& sunpos,
    double altitude,
    double f107,
    int n
);

/*! @} */

AST_NAMESPACE_END
