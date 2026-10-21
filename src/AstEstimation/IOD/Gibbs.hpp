///
/// @file      Gibbs.hpp
/// @brief     Gibbs 初轨确定
/// @details   由三个连续时刻的位置矢量求解中间时刻的速度
/// @author    axel
/// @date      2026-10-21
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

/// @brief Gibbs 初轨确定
/// @details 一种经典的几何-矢量方法，利用三个连续时刻的位置矢量（不需要速度和时间间隔）
///          求解第二个位置处的速度，与 r2 一起构成该时刻的完整状态。
///          三个位置须位于同一二体轨道（即共面于中心天体），并按时间先后排列
/// @param r1 第一个观测时刻的位置矢量（单位 m）
/// @param r2 第二个观测时刻的位置矢量（单位 m）
/// @param r3 第三个观测时刻的位置矢量（单位 m）
/// @param gm 中心天体引力常数 GM（单位 m³/s²）
/// @param v2 输出：r2 处的速度矢量（惯性系，单位 m/s）
/// @return 错误码
/// @note 该方法只用位置信息，适合观测时刻间隔未知的场合；对观测噪声敏感，
///       且要求三点严格共面。位置顺序颠倒会得到 -v2（等价于按反向运动求解）。
AST_ESTIMATION_API errc_t aGibbs(
    const Vector3d& r1,
    const Vector3d& r2,
    const Vector3d& r3,
    double gm,
    Vector3d& v2
);

/*! @} */

AST_NAMESPACE_END
