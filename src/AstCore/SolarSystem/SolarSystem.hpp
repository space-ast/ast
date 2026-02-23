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
#include "AstCore/CelestialBody.hpp"
#include <unordered_map>
 
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



/// @brief 太阳系
/// @details 太阳系行星及行星卫星的集合，
/// 包含水星、金星、地球、火星、木星、土星、天王星、海王星、冥王星、月球和太阳等。
class AST_CORE_API SolarSystem
{
public:
	SolarSystem() = default;
	~SolarSystem() = default;

	void init();

	err_t load(StringView dirpath);
	err_t loadDefault();
	
	/// @brief 获取水星
	CelestialBody* getMercury() const { return mercury_.get(); }
	
	/// @brief 获取金星
	CelestialBody* getVenus() const { return venus_.get(); }

	/// @brief 获取地球
	CelestialBody* getEarth() const { return earth_.get(); }

	/// @brief 获取火星
	CelestialBody* getMars() const { return mars_.get(); }

	/// @brief 获取木星
	CelestialBody* getJupiter() const { return jupiter_.get(); }

	/// @brief 获取土星
	CelestialBody* getSaturn() const { return saturn_.get(); }

	/// @brief 获取天王星	
	CelestialBody* getUranus() const { return uranus_.get(); }

	/// @brief 获取海王星	
	CelestialBody* getNeptune() const { return neptune_.get(); }
	
	/// @brief 获取冥王星
	CelestialBody* getPluto() const { return pluto_.get(); }

	/// @brief 获取月球
	CelestialBody* getMoon() const { return moon_.get(); }

	/// @brief 获取火星
	CelestialBody* getSun() const { return sun_.get(); }

	/// @brief 获取指定名称的天体
	/// @param  name        - 天体名称
	/// @retval             - 天体指针
	CelestialBody* getBody(StringView name) const;

	/// @brief 添加一个新的天体
	/// @param  name        - 天体名称
	/// @retval             - 天体指针
	CelestialBody* addBody(StringView name);
	
	/// @brief 获取指定名称的天体，若不存在则添加
	/// @param  name        - 天体名称
	/// @retval             - 天体指针
	CelestialBody* getOrAddBody(StringView name);
		
protected:
	using CelestialBodyMap = std::unordered_map<std::string, SharedPtr<CelestialBody>>;

	SharedPtr<CelestialBody> mercury_;			///< 水星
	SharedPtr<CelestialBody> venus_;			///< 金星
    SharedPtr<CelestialBody> earth_;			///< 地球
    SharedPtr<CelestialBody> mars_;				///< 火星
	SharedPtr<CelestialBody> jupiter_;			///< 木星
	SharedPtr<CelestialBody> saturn_;			///< 土星
	SharedPtr<CelestialBody> uranus_;			///< 天王星
	SharedPtr<CelestialBody> neptune_;			///< 海王星
	SharedPtr<CelestialBody> pluto_;			///< 冥王星
    SharedPtr<CelestialBody> moon_;				///< 月球
	SharedPtr<CelestialBody> sun_;				///< 太阳
	
	CelestialBodyMap bodies_;					///< 太阳系天体映射表
};

/*! @} */

AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(SolarSystem)
