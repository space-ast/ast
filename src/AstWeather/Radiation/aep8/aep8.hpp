///
/// @file      aep8.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-18
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
    @addtogroup Magnetosphere
    @{
*/


/// @brief 计算特定 L 值和磁场强度下的粒子通量
/// @param e  需要计算通量的能量值 [MeV(兆电子伏特)]
/// @param l （McIlwain L 参数），表示磁壳参数。程序内部会将其限制在 ≤15.6
/// @param bb0  B/B₀，即当地磁场强度与磁赤道磁场强度的比值。若输入小于 1，程序强制设为 1
/// @param iname   粒子种类与太阳活动水平：
///                - 1: e-min（电子，太阳活动极小年）
///                - 2: e-max（电子，太阳活动极大年）
///                - 3: p-min（质子，太阳活动极小年）
///                - 4: p-max（质子，太阳活动极大年）
/// @return 粒子通量 [cm^-2 s^-1]
AST_WEATHER_CAPI double aep8(double e, double l, double bb0, int iname);


/// @brief 计算特定位置和时间处辐射带捕获粒子的通量
/// @details
/// "辐射带捕获粒子"指的是被地球磁场捕获并束缚在范艾伦辐射带（Van Allen Belts）中的高能带电粒子（主要是电子和质子）
/// 请参见：范艾伦辐射带（Van Allen Belts）
///
/// 该函数先通过 IGRF 地磁场模型将地理坐标（经度、纬度、高度、年份）转换为
/// McIlwain L 参数（磁壳参数）和 B/B₀（当地磁场强度与磁赤道磁场强度之比），
/// 再调用 AEP8 辐射带模型计算粒子积分通量。
///
/// @param lon     地理经度 [度（°）]
/// @param lat     地理纬度 [度（°）]
/// @param height  相对于地球表面的高度 [千米（km）]
/// @param energy  能量阈值，计算通量时只统计能量 ≥ 该值的粒子 [MeV（兆电子伏特）]
/// @param year    小数年份（如 2021.1616），用于考虑地球磁场的长期漂移
/// @param iname   粒子种类与太阳活动水平：
///                - 1: e-min（电子，太阳活动极小年）
///                - 2: e-max（电子，太阳活动极大年）
///                - 3: p-min（质子，太阳活动极小年）
///                - 4: p-max（质子，太阳活动极大年）
/// @return 粒子积分通量 [cm⁻²·s⁻¹]
///
/// @par 使用示例：
/// @code
/// // 计算南大西洋异常区（45°W, 30°S, 500 km高度）处
/// // 能量 ≥20 MeV 的质子在太阳活动极大年时的通量
/// double flux = get_flux(
///     -45.0,              // 经度: 45°W
///     -30.0,              // 纬度: 30°S
///     500.0,              // 高度: 500 km
///     20.0,               // 能量: 20 MeV
///     2021.1616438356164, // 时间: 2021-03-01
///     4                   // p-max
/// );
/// // flux ≈ 2642.5 cm⁻²·s⁻¹
/// @endcode
///
/// @see aep8(), igrf()
AST_WEATHER_CAPI double get_flux(
    double lon, double lat, double height, double energy, double year, int iname
);

/*! @} */

AST_NAMESPACE_END
