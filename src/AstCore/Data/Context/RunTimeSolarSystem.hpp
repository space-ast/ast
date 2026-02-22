///
/// @file      RunTimeSolarSystem.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-22
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
    @addtogroup Data
    @{
*/

class SolarSystem;
class CelestialBody;

/// @brief 获取太阳系
AST_CORE_CAPI SolarSystem* aGetSolarSystem();

/// @brief 获取指定名称的天体
AST_CORE_CAPI CelestialBody* aGetBody(StringView name);

/// @brief 获取水星
AST_CORE_CAPI CelestialBody* aGetMercury();

/// @brief 获取金星
AST_CORE_CAPI CelestialBody* aGetVenus();

/// @brief 获取地球
AST_CORE_CAPI CelestialBody* aGetEarth();

/// @brief 获取火星
AST_CORE_CAPI CelestialBody* aGetMars();

/// @brief 获取木星
AST_CORE_CAPI CelestialBody* aGetJupiter();

/// @brief 获取土星
AST_CORE_CAPI CelestialBody* aGetSaturn();

/// @brief 获取月球
AST_CORE_CAPI CelestialBody* aGetMoon();



/*! @} */

AST_NAMESPACE_END


