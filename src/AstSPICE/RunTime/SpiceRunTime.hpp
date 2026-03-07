///
/// @file      SpiceRunTime.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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
class Axes;
class CelestialBody;
class TimePoint;

/// @brief 查找指定名称的参考系统轴
/// @param name 参考系统名称
/// @return 指向 Axes 实例的指针，如果未找到则为 nullptr
AST_SPICE_CAPI Axes* aSpiceFindAxes(StringView name);

/// @brief 查找指定名称的天体
/// @param name 天体名称
/// @return 指向 CelestialBody 实例的指针，如果未找到则为 nullptr
AST_SPICE_API CelestialBody* aSpiceFindBody(StringView name);

/// @brief 查找指定 ID 的天体
/// @param id 天体 ID
/// @return 指向 CelestialBody 实例的指针，如果未找到则为 nullptr
AST_SPICE_API CelestialBody* aSpiceFindBody(int id);


/// @brief 将 SPICE 时间转换为 TimePoint
/// @param et SPICE 时间（秒）
/// @return 对应的 TimePoint 实例
AST_SPICE_API TimePoint aSpiceEtToTimePoint(double et);


/*! @} */

AST_NAMESPACE_END
