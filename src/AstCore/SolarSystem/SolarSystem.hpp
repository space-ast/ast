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
#include "AstCore/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include <unordered_map>
#include <vector>
 
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
class AST_CORE_API SolarSystem: public ObjectNamed
{
public:
	SolarSystem() = default;
	~SolarSystem() = default;

	/// @brief 初始化太阳系
	void init();

	/// @brief 加载指定目录下的太阳系天体数据
	/// @warning 该方法会覆盖已加载的天体数据
	/// @param  dirpath     - 数据目录路径
	/// @retval             - 错误码
	errc_t load(StringView dirpath);

	/// @brief 通过PCK文件加载太阳系天体数据
	/// @warning 该方法会覆盖已加载的天体数据
	/// @param  filepath    - PCK文件路径
	/// @retval             - 错误码
	errc_t loadPCK(StringView filepath);

	/// @brief 加载默认的太阳系数据
	errc_t loadDefault();

	/// @brief 获取默认的太阳系数据目录
	static std::string defaultSolarSystemDir();

	/// @brief 获取太阳系数据目录路径
	const std::string& getDirpath() const { return dirpath_; }

	/// @brief 获取太阳系质心
	CelestialBody* getSolarSystemBarycenter() const { return solarSystemBarycenter_.get(); }
	
	/// @brief 获取地月质心
	CelestialBody* getEarthMoonBarycenter() const { return earthMoonBarycenter_.get(); }

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


	/// @brief 获取指定JPL索引的天体
	/// @param  index       - JPL索引
	/// @retval             - 天体指针
	CelestialBody* getBodyByJplIndex(int index) const;

	/// @brief 获取指定SPICE ID的天体
	/// @param  id          - SPICE ID
	/// @retval             - 天体指针
	CelestialBody* getBodyBySpiceId(int id) const;

	/// @brief 添加一个新的天体，如果name已存在，则返回nullptr
	/// @param  name        - 天体名称
	/// @retval             - 天体指针
	CelestialBody* addBody(StringView name);

	/// @brief 添加一个新的天体，如果相应名称的天体已存在，则返回nullptr
	/// @param  body        - 天体指针
	/// @retval             - 天体指针
	CelestialBody* addBody(HCelestialBody body);
	
	/// @brief 获取指定名称的天体，若不存在则添加
	/// @param  name        - 天体名称
	/// @retval             - 天体指针
	CelestialBody* getOrAddBody(StringView name);
		
protected:
	using BodyNameMap = std::unordered_map<std::string, CelestialBody*>;
	using BodyIndexMap = std::unordered_map<int, CelestialBody*>;
	using BodyVector = std::vector<SharedPtr<CelestialBody>>;

	
	SharedPtr<CelestialBody> solarSystemBarycenter_; ///< 太阳系质心
	SharedPtr<CelestialBody> earthMoonBarycenter_;	 ///< 地月质心
 
	SharedPtr<CelestialBody> mercury_;				 ///< 水星
	SharedPtr<CelestialBody> venus_;				 ///< 金星
    SharedPtr<CelestialBody> earth_;				 ///< 地球
    SharedPtr<CelestialBody> mars_;					 ///< 火星
	SharedPtr<CelestialBody> jupiter_;				 ///< 木星
	SharedPtr<CelestialBody> saturn_;				 ///< 土星
	SharedPtr<CelestialBody> uranus_;				 ///< 天王星
	SharedPtr<CelestialBody> neptune_;				 ///< 海王星
	SharedPtr<CelestialBody> pluto_;				 ///< 冥王星
    SharedPtr<CelestialBody> moon_;					 ///< 月球
	SharedPtr<CelestialBody> sun_;					 ///< 太阳
 
	BodyVector bodies_;							 	 ///< 太阳系天体集合
	mutable BodyNameMap  nameMap_;					 ///< 太阳系天体映射表，可能存在别名映射
	mutable BodyIndexMap jplIndexMap_;				 ///< 太阳系天体映射表，根据JPL索引映射
	mutable BodyIndexMap spiceIdMap_;				 ///< 太阳系天体映射表，根据SPICE ID映射
	bool			     isInit_{false};			 ///<  是否已初始化
	std::string          dirpath_;				 	 ///< 太阳系数据目录路径
};



/*! @} */

AST_NAMESPACE_END

AST_DECL_TYPE_ALIAS(SolarSystem)
