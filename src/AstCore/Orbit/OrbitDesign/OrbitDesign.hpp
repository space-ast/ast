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

/// @brief 计算轨道升交点赤经漂移速率
/// @param gm 天体的引力参数
/// @param j2 天体的J2项
/// @param rb 天体的半径
/// @param a 轨道的半长轴
/// @param ecc 轨道的偏心率
/// @param inc 轨道的倾角
/// @return 轨道升交点赤经漂移速率
AST_CORE_CAPI double aRAANRate(double gm, double j2, double rb, double a, double ecc, double inc);

/// @brief 计算轨道近点幅角变化率
/// @param gm 天体的引力参数
/// @param j2 天体的J2项
/// @param rb 天体的半径
/// @param a 轨道的半长轴
/// @param ecc 轨道的偏心率
/// @param inc 轨道的倾角
/// @return 轨道近点幅角变化率
AST_CORE_CAPI double aArgPerRate(double gm, double j2, double rb, double a, double ecc, double inc);


/// @brief 计算轨道平近点角变化率
/// @param gm 天体的引力参数
/// @param j2 天体的J2项
/// @param rb 天体的半径
/// @param a 轨道的半长轴
/// @param ecc 轨道的偏心率
/// @param inc 轨道的倾角
/// @return 轨道平近点角变化率
AST_CORE_CAPI double aMeanAnomalyRate(double gm, double j2, double rb, double a, double ecc, double inc);


/// @brief 计算轨道在J2长期项影响下的轨道周期/交点周期
/// @param gm 天体的引力参数
/// @param j2 天体的J2项
/// @param rb 天体的半径
/// @param a 轨道的半长轴
/// @param ecc 轨道的偏心率
/// @param inc 轨道的倾角
/// @return J2长期项影响下的轨道周期/交点周期
AST_CORE_CAPI double aJ2Period(double gm, double j2, double rb, double a, double ecc, double inc);

/*! @} */

AST_NAMESPACE_END
