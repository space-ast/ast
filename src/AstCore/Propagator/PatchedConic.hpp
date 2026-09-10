///
/// @file      PatchedConic.hpp
/// @brief     圆锥曲线拼接(双二体)预报
/// @details   以月球影响球为拼接面的圆锥曲线拼接预报。
/// @author    axel
/// @date      2026-09-10
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

/// @brief 计算卫星的影响球半径 (Laplace SOI)
/// @details r_SOI = a * (GM_satellite / GM_primary)^(2/5)
/// @param gmPrimary          主天体引力常数 [m^3/s^2]
/// @param gmSatellite        卫星引力常数 [m^3/s^2]
/// @param orbitSemiMajorAxis 卫星绕主天体的轨道半长轴 [m]
/// @return 影响球半径 [m]
/// @note  影响球是"卫星引力主导范围"的约定边界，不是真实物理边界，半长轴取长期均值最稳定；
///        若取某时刻的瞬时密切半长轴，结果会随该时刻的摄动相位波动，由调用方按需选择。
AST_CORE_API double aSOIRadius(double gmPrimary, double gmSatellite, double orbitSemiMajorAxis);


/// @brief     双二体模型前推预报（终端在卫星）
/// @details   以卫星影响球(Sphere Of Influence, SOI)为拼接面的圆锥曲线拼接预报。
///            出发时刻从主天体惯性系出发，主天体段按二体解析预报至穿越卫星SOI 球面，
///            在球面上将状态转换到卫星的天体惯性系，再以卫星二体预报到卫星近拱点。
/// @param[in]   primaryBody   主天体
/// @param[in]   secondaryBody 卫星
/// @param[in]   soiRadius     卫星影响球半径 [m]
/// @param[in]   initTime   出发时刻
/// @param[in]   initState  出发时刻的主天体惯性系状态 [m, m/s]
/// @param[out]  outTime    输出，终端(卫星近拱点)时刻
/// @param[out]  outState   输出，终端卫星惯性系状态 [m, m/s]
/// @return    错误码
/// @note      输出参考系为卫星的天体惯性系。预报方向为"主天体 -> 卫星"。
AST_CORE_API errc_t aPatchedConicPropToSecondary
(
    CelestialBody& primaryBody,
    CelestialBody& secondaryBody,
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
);


/// @brief 地月转移双二体模型前推预报
/// @param soiRadius 卫星影响球半径 [m]
/// @param initTime 出发时刻
/// @param initState 出发时刻的地心惯性系状态 [m, m/s]
/// @param outTime 输出，终端(近月点)时刻
/// @param outState 输出，终端月心惯性系状态 [m, m/s]
/// @return 错误码
/// @note 输出参考系为月球的天体惯性系
AST_CORE_API errc_t aPatchedConicPropEarthToMoon(
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
);


/// @brief     双二体模型卫星到主天体预报（终端在主天体）
/// @details   与 aPatchedConicPropToSecondary 流向相反、时间方向相同（均为时间递增）：
///            从卫星惯性系下的状态出发，正向传播至穿出卫星影响球，在球面上把状态
///            转换回主天体惯性系，再正向传播至主天体的下一个近拱点。
/// @param[in]   primaryBody   主天体
/// @param[in]   secondaryBody 卫星
/// @param[in]   soiRadius     卫星影响球半径 [m]
/// @param[in]   initTime      出发时刻（位于卫星段上）
/// @param[in]   initState     出发时刻的卫星惯性系状态 [m, m/s]，须位于影响球内
/// @param[out]  outTime       输出，终端(主天体近拱点)时刻
/// @param[out]  outState      输出，终端主天体惯性系状态 [m, m/s]
/// @return    错误码
AST_CORE_API errc_t aPatchedConicPropToPrimary
(
    CelestialBody& primaryBody,
    CelestialBody& secondaryBody,
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
);


/// @brief 地月转移双二体模型月球到地球预报
/// @param soiRadius 卫星影响球半径 [m]
/// @param initTime 出发时刻（位于月球段上）
/// @param initState 出发时刻的月心惯性系状态 [m, m/s]，须位于影响球内
/// @param outTime 输出，终端(地心近拱点/近地点)时刻
/// @param outState 输出，终端地心惯性系状态 [m, m/s]
/// @return 错误码
/// @note 输出参考系为地球的天体惯性系
AST_CORE_API errc_t aPatchedConicPropMoonToEarth(
    double           soiRadius,
    const TimePoint& initTime,
    const CartState& initState,
    TimePoint&       outTime,
    CartState&       outState
);

/*! @} */

AST_NAMESPACE_END
