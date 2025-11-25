///
/// @file      GlobalContext.hpp
/// @brief     全局上下文及其相关接口
/// @details   ~
/// @author    jinke18
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
#include <string>
 
AST_NAMESPACE_BEGIN
 

class SolarSystem;
class EOPData;
class LeapSecondData;


/// @brief 全局上下文
class GlobalContext
{
public:
	/// @brief 太阳系数据
	SolarSystem* solarSystem() const{return m_solarSystem;}

	/// @brief 地球指向数据
	EOPData*	 eopData() const{return m_eopData;}

	/// @brief 闰秒数据
	LeapSecondData* leapSecondData() const{return m_leapSecondData;}

	/// @brief 数据目录
    std::string& dataDir() {return m_dataDir;}
    
	/// @brief 设置数据目录
	void setDataDir(const std::string& dir){m_dataDir = dir;}

protected:
	SolarSystem*    m_solarSystem{ nullptr };
	EOPData*	    m_eopData{nullptr};
	LeapSecondData* m_leapSecondData{nullptr};
    std::string     m_dataDir;
};



 
AST_NAMESPACE_END
 


