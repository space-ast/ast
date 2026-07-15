///
/// @file      DataContext.hpp
/// @brief     数据上下文及其相关接口
/// @details   ~
/// @author    axel
/// @date      18.11.2025
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
#include "AstCore/ScopedPtr.hpp"
#include "AstCore/SolarSystem.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/LeapSecond.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/SpaceWeather.hpp"
#include "AstCore/IAUXYS.hpp"
#include "AstCore/IAUXYSPrecomputed.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/StartupConfig.hpp"

#include <string>
 
AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/



class SolarSystem;
class EOP;
class LeapSecond;


/// @brief 数据上下文
/// @details 
/// 数据上下文包含了太阳系、地球指向、闰秒、空间天气、JPL DE星历和IAU XYS预计算数据。
class DataContext
{
public:

	/// @brief 太阳系数据
	const SolarSystem* solarSystem() const{return solarSystem_;}
    SolarSystem* solarSystem() {return solarSystem_;}

	/// @brief 地球指向数据
	const EOP* eop() const{return &eop_;}
    EOP* eop() {return &eop_;}

	/// @brief 闰秒数据
	const LeapSecond* leapSecond() const{return &leapSecond_;}
    LeapSecond* leapSecond() {return &leapSecond_;}

	/// @brief 空间天气数据
	// const SpaceWeather* spaceWeather() const{return &spaceWeather_;}
    // SpaceWeather* spaceWeather() {return &spaceWeather_;}

	/// @brief 数据目录
    std::string& dataDir() {return dataDir_;}

	/// @brief 设置数据目录
	void setDataDir(StringView dir){dataDir_ = std::string(dir);}

	/// @brief JPL DE星历数据
    const JplDe* jplDe() const {return &jplDe_;}
    JplDe* jplDe() {return &jplDe_;}

	/// @brief IAU XYS预计算数据
	const IAUXYSPrecomputed* iauXYSPrecomputed() const {return &iauXYSPrecomputed_;}
    IAUXYSPrecomputed* iauXYSPrecomputed() {return &iauXYSPrecomputed_;}

	/// @brief 参考历元
    TimePoint& epoch() {return epoch_;}
	/// @brief 设置参考历元
	void setEpoch(TimePoint tp){epoch_ = tp;}

	/// @brief 配置
	const StartupConfig* config() const {return &config_;}
	StartupConfig* config() {return &config_;}

	/// @brief 是否初始化
	bool isInitialized() const {return !dataDir_.empty();}
protected:
	SharedPtr<SolarSystem>  solarSystem_{new SolarSystem("SolarSystem")};	///< 太阳系数据
	EOP		    			eop_;											///< 地球指向数据
	LeapSecond  			leapSecond_;									///< 闰秒数据
	// SpaceWeather          spaceWeather_;									///< 空间天气数据
    JplDe                   jplDe_;											///< JPL DE星历数据
	IAUXYSPrecomputed       iauXYSPrecomputed_;						///< IAU XYS预计算数据 @todo: 这个考虑更改为静态数据
    std::string     		dataDir_;										///< 数据目录
	TimePoint				epoch_{};										///< 参考历元
	StartupConfig           config_;										///< 配置
};

constexpr size_t kSizeOfDataContext = sizeof(DataContext);

/*! @} */

 
AST_NAMESPACE_END
 


