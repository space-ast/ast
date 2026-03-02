///
/// @file      SpiceZpr.h
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-02
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
    @addtogroup Spice
    @{
*/

// appndc

// appndd

// appndi


/// @brief 根据旋转轴和角度计算旋转矩阵(Axis and angle to rotation)
/// 输出的矩阵表示将向量绕轴旋转给定角度
/// @param[in] axis 旋转轴
/// @param[in] angle 旋转角度（弧度）
/// @param[out] r 输出旋转矩阵
AST_SPICE_CAPI void axisar(const Vector3d&  axis,
                           double           angle,
                           Matrix3d&        r);


// azlcpo


/// @brief 将极坐标转换为矩形坐标(AZ/EL to rectangular coordinates)
/// @param[in] range 范围（距离）
/// @param[in] az 方位角（弧度）
/// @param[in] el 俯仰角（弧度）
/// @param[in] azccw 如果为真，则方位角按逆时针方向计算，否则按顺时针方向计算
/// @param[in] elplsz 如果为真，则俯仰角按正值上增加，否则按负值上增加
/// @param[out] rectan 输出矩形坐标（x, y, z）
AST_SPICE_CAPI void azlrec(double     range,
                           double     az,
                           double     el,
                           bool       azccw,
                           bool       elplsz,
                           Vector3d&  rectan);


/// @brief 将纬度经度转换为矩形坐标(Latitudinal to rectangular coordinates)
/// @param[in] radius 半径
/// @param[in] lon 经度（弧度）
/// @param[in] lat 纬度（弧度）
/// @param[out] rectan 输出矩形坐标（x, y, z）
AST_SPICE_CAPI void latrec(double    radius,
                           double    lon,
                           double    lat,
                           Vector3d&  rectan);



/*! @} */

AST_NAMESPACE_END
