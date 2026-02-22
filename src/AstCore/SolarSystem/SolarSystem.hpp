///
/// @file      SolarSystem.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      15.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。
 
#pragma once
 
#include "AstGlobal.h"
 
AST_NAMESPACE_BEGIN


/*!
	@ingroup Core
	@defgroup SolarSystem 太阳系
	@brief 提供太阳系及行星相关计算函数与类
*/


/*!
	@addtogroup SolarSystem
	@{
*/

class CelestialBody;
class SolarSystem;

/// @brief 获取地球
AST_CORE_CAPI CelestialBody* aGetEarth(SolarSystem* ss);

/// @brief 获取月球
AST_CORE_CAPI CelestialBody* aGetMoon(SolarSystem* ss);

/// @brief 获取火星
AST_CORE_CAPI CelestialBody* aGetMars(SolarSystem* ss);


/// @brief 太阳系
class AST_CORE_API SolarSystem
{
public:
	SolarSystem();
	/// @brief 获取地球
	CelestialBody* getEarth();
	/// @brief 获取月球
	CelestialBody* getMoon();
	/// @brief 获取火星
	CelestialBody* getMars();
};

/*! @} */

AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(SolarSystem)
