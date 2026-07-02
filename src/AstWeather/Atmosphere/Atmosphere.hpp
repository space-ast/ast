///
/// @file      Atmosphere.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-22
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
    @ingroup Weather
    @defgroup Atmosphere 大气模型
    @brief  包括MSIS系列大气模型、USSA1976大气模型等。
*/


/*!
    @addtogroup Atmosphere
    @{
*/


/// @brief USSA1976大气模型
/// @param alt 高度 [m]
/// @return 大气密度 [kg/m³]
AST_WEATHER_CAPI double aUSSA1976(double alt);


/// @brief JaciachiaRoberts大气模型
/// @details 函数的注释原文请参考 val_atm_jr.cpp 和 val_atm_jr.h
/// @param height 地理高度 [m]
/// @param space_craft TOD GCI 坐标系下的卫星位置 [m]
/// @param sun TOD GCI 坐标系下的太阳向量 [m] 
/// @param a1_time 儒略日减去2430000（即相对于1941-01-05 12:00:00的天数）[A.1 时间系统]
/// @param tkp 地磁指数 [Kp]
/// @param xtemp 外大气层温度[K]
/// @return 大气密度 [kg/m³]
AST_WEATHER_CAPI double aJacchiaRoberts(
    double height, const Vector3d& space_craft, const Vector3d& sun, 
    double a1_time, double tkp, double xtemp
);


/*! @} */

AST_NAMESPACE_END
