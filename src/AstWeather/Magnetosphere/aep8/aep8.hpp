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
/// @param e  需要计算通量的能量值，单位为 MeV(兆电子伏特)
/// @param l （McIlwain L 参数），表示磁壳参数。程序内部会将其限制在 ≤15.6
/// @param bb0  B/B₀，即当地磁场强度与磁赤道磁场强度的比值。若输入小于 1，程序强制设为 1
/// @param iname 1: e-min, 2: e-max, 3: p-min, 4: p-max
///              'e': 电子, 'p': 质子.
/// @return 粒子通量，单位为 cm^-2 s^-1
AST_WEATHER_CAPI double aep8(double e, double l, double bb0, int iname);



/*! @} */

AST_NAMESPACE_END
