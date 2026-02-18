///
/// @file      Magnetosphere.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-15
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
#include "AstWeather/aep8.hpp"

AST_NAMESPACE_BEGIN

/*!
    @ingroup Weather
    @defgroup Magnetosphere 磁场模型
    @brief  包括IGRF等磁场模型等。
*/


/*!
    @addtogroup Magnetosphere
    @{
*/


/// @brief 根据IGRF13模型计算磁场分量
/// @details  
/// igrf13.f文件内igrf13syn函数的C版本，保存函数接口与参数含义一致。
/// @see https://www.ncei.noaa.gov/products/international-geomagnetic-reference-field
/// @see https://www.ngdc.noaa.gov/IAGA/vmod/igrf13.f
/// @see https://ccmc.gsfc.nasa.gov/models/IGRF~13/
/// @see https://github.com/space-physics/igrf
/// @param[in] isv =0: 计算主磁场值（单位：nT）;
///                =1: 计算长期变化率（年变率，单位：nT/年）
/// @param[in] date 年份，公元纪年，如 2020.0。有效范围：1900.0 至 2030.0;
///                 若 date > 2025.0 会给出警告，表示精度可能降低（因预测模型仅到2025年）
/// @param[in] itype =1：大地坐标（海拔高度）;
///                  =2：地心坐标（地心距离）
/// @param[in] alt 当 itype=1 时：海拔高度，单位为 km（相对于海平面）;
///                当 itype=2 时：到地心的距离，单位为 km（必须 > 3485 km，即地球内核半径之外）
/// @param[in] colat 余纬度，单位为度，范围 0° 到 180°;
///                  定义为从北极起算的角度：colat = 90° - 纬度（北纬为正，南纬为负）
/// @param[in] elong 东经，单位为度，范围 0° 到 360°
/// @param[out] x 若 isv=0：主磁场北分量，单位 nT;
///               若 isv=1：北分量的年变率，单位 nT/年
/// @param[out] y 东向分量（单位同上）
/// @param[out] z 垂直分量（向下为正，单位同上）
/// @param[out] f 总强度（仅当 isv=0 时有意义，单位 nT）；若 isv=1 则返回无意义的值
/// @return 
AST_WEATHER_CAPI int igrf13syn(
    int isv, double date, int itype, double alt, double colat, double elong, 
    double &x, double &y, double &z, double &f
);



/*! @} */

AST_NAMESPACE_END
