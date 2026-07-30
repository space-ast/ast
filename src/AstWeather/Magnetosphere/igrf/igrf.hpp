///
/// @file      igrf.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-19
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


/// @brief 利用 IGRF 地磁场模型计算磁壳参数 L 和磁场比值 B/B₀
///
/// IGRF（International Geomagnetic Reference Field，国际地磁参考场）是
/// 国际地磁学与高空物理学协会（IAGA）发布的标准地磁场模型。
/// 
/// 本函数将地理坐标转换为辐射带建模所需的磁坐标参数，
/// 是连接地理空间与辐射带模型（AE8/AP8）的桥梁。
///
/// @param[in]  lon     地理经度 [度（°）]
/// @param[in]  lat     地理纬度 [度（°）]
/// @param[in]  height  相对于地球表面的高度 [千米（km）]
/// @param[in]  year    小数年份（如 2021.1616），用于计算地磁场的长期变化（主磁场漂移）
/// @param[out] xl      McIlwain L 参数（磁壳参数），无量纲。
///                     表示磁力线在赤道面与地心的距离（以地球半径 R_E 为单位）。
///                     内辐射带典型值 ~1.2–3，外辐射带 ~3–10。
/// @param[out] bbx     B/B₀，即当地磁场总强度与同 L 值磁赤道处磁场强度的比值。
///                     该值 ≥ 1，是辐射带粒子弹跳运动的控制参数。
///
/// @note  函数内部会通过 WorkingDirectory 临时切换当前目录到 IGRF 数据目录，
///        因为底层 Fortran 例程通过相对路径读取数据文件。
///
/// @par 使用示例：
/// @code
/// double xl, bbx;
/// // 计算南大西洋异常区（45°W, 30°S, 500 km, 2021-03-01）的磁坐标
/// igrf(-45.0, -30.0, 500.0, 2021.1616438356164, xl, bbx);
/// // xl  ≈ 1.4138（L 值，位于内辐射带边缘）
/// // bbx ≈ 1.7853（磁镜比，粒子在此处接近弹跳镜像点）
/// @endcode
///
/// @see aep8(), get_flux()
AST_WEATHER_CAPI void igrf(double lon, double lat, double height, double year, double& xl, double& bbx);


/*! @} */

AST_NAMESPACE_END
