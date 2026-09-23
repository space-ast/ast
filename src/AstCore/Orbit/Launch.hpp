///
/// @file      Launch.hpp
/// @brief     发射弹道相关计算
/// @details   提供发射方位角（射向）与发射点纬度、目标轨道倾角之间的换算接口。
/// @author    axel
/// @date      2026-09-17
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
    @addtogroup Orbit
    @{
*/


/// @brief 由目标轨道倾角和发射点纬度计算发射方位角（射向）
/// @details 忽略地球自转对速度方向的影响，认为火箭在发射点沿当地水平面飞行，
///          此时目标轨道倾角 i、发射方位角 A 与发射点地理纬度 B 满足球面三角关系：
///
///              cos(i) = cos(B) * sin(A)
///
///          该方程一般有两个解，分别对应向北发射（发射点位于轨道升段）和向南发射（发射点位于轨道降段）
///          当 sin(A) = ±1（如赤道发射场入轨倾角为 0° 或 180°、任意纬度入轨倾角为 90°）时两个解重合
/// @param inclination  目标轨道倾角 [rad]，取值范围 [0, π]
/// @param latitude     发射点地理纬度 [rad]，取值范围 [-π/2, π/2]
/// @param azimuthNorth 向北发射的方位角 [rad]，自正北起顺时针为正（向东为正）
/// @param azimuthSouth 向南发射的方位角 [rad]，自正北起顺时针为正（向东为正）
/// @return 错误码，参数非法时返回 eErrorInvalidParam；
///         目标倾角低于发射点纬度（|cos(i)| > cos(B)）或发射点在极点附近时返回 eErrorOutOfRange
/// @see aLaunchAzimuthToInclination
AST_CORE_CAPI errc_t aInclinationToLaunchAzimuths(double inclination, double latitude, double& azimuthNorth, double& azimuthSouth);

/// @brief 由发射方位角和发射点纬度计算入轨轨道倾角
/// @details 为 aInclinationToLaunchAzimuths() 的逆运算，同样忽略地球自转的影响
/// @param azimuth 发射方位角 [rad]，自正北起顺时针为正（向东为正）
/// @param latitude 发射点地理纬度 [rad]
/// @return 轨道倾角 [rad]，取值范围 [0, π]；纬度非法时返回 NaN
/// @see aInclinationToLaunchAzimuths
AST_CORE_CAPI double aLaunchAzimuthToInclination(double azimuth, double latitude);

/*! @} */

AST_NAMESPACE_END
