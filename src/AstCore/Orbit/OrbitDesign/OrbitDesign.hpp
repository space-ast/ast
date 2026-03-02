///
/// @file      OrbitDesign.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-25
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
    @addtogroup Orbit
    @{
*/

/// @brief 计算太阳同步轨道的倾角
/// @param[in] gm 天体引力系数
/// @param[in] j2 天体J2项
/// @param[in] rb 天体半径
/// @param[in] bodyMeanMotion 天体平均公转角速度
/// @param[in] a 轨道半长轴
/// @param[in] ecc 轨道偏心率
/// @param[out] inc 轨道倾角，如果计算失败，则为NaN
/// @return err_t 错误码
err_t aSunSynchronousInclination(double gm, double j2, double rb, double bodyMeanMotion, double a, double ecc, double& inc);

/// @brief 计算太阳同步轨道的半长轴
/// @param[in] gm 天体引力系数
/// @param[in] j2 天体J2项
/// @param[in] rb 天体半径
/// @param[in] bodyMeanMotion 天体平均公转角速度
/// @param[in] inc 轨道倾角
/// @param[in] ecc 轨道偏心率
/// @param[out] semiMajorAxis 轨道半长轴，如果计算失败，则为NaN
/// @return err_t 错误码
err_t aSunSynchronousSemiMajorAxis(double gm, double j2, double rb, double bodyMeanMotion, double inc, double ecc, double& semiMajorAxis);


/// @brief 计算太阳同步轨道的倾角
/// @param[in] gm 天体引力系数
/// @param[in] j2 天体J2项
/// @param[in] rb 天体半径
/// @param[in] bodyMeanMotion 天体平均公转角速度
/// @param[in] a 轨道半长轴
/// @param[in] ecc 轨道偏心率
/// @return double 轨道倾角，如果计算失败，则为NaN
A_ALWAYS_INLINE double aSunSynchronousInclination(double gm, double j2, double rb, double bodyMeanMotion, double a, double ecc)
{
    double inc;
    aSunSynchronousInclination(gm, j2, rb, bodyMeanMotion, a, ecc, inc);
    return inc;
}

/// @brief 计算太阳同步轨道的半长轴
/// @param[in] gm 天体引力系数
/// @param[in] j2 天体J2项
/// @param[in] rb 天体半径
/// @param[in] bodyMeanMotion 天体平均公转角速度
/// @param[in] inc 轨道倾角
/// @param[in] ecc 轨道偏心率
/// @return double 轨道半长轴，如果计算失败，则为NaN
A_ALWAYS_INLINE double aSunSynchronousSemiMajorAxis(double gm, double j2, double rb, double bodyMeanMotion, double inc, double ecc)
{
    double semiMajorAxis;
    aSunSynchronousSemiMajorAxis(gm, j2, rb, bodyMeanMotion, inc, ecc, semiMajorAxis);
    return semiMajorAxis;
}

/*! @} */

AST_NAMESPACE_END
