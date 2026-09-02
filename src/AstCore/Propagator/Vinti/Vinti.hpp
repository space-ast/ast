///
/// @file      Vinti.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-09-01
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


/// @brief Vinti 解析解轨道预报（含 J2 / J3 扁率项）
/// @param duration  预报时间 [s]
/// @param gm        引力常数 
/// @param j2        J2 系数 
/// @param j3        J3 系数 
/// @param re        天体半径 [m]
/// @param[out] r    位置 [m]
/// @param[out] v    速度 [m/s]
/// @note  当 duration < 1e-15 时直接返回，r 与 v 保持不变。
AST_CORE_CAPI errc_t aVinti(double duration, double gm, double j2, double j3, double re, Vector3d& r, Vector3d& v);

/*! @} */

AST_NAMESPACE_END
