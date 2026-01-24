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
#include "AstCore/IAUXYS.hpp"
#include "AstCore/IAUXYSPrecomputed.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
 
AST_NAMESPACE_BEGIN
 

class SolarSystem;
class EOP;
class LeapSecond;


/// @brief 数据上下文
class DataContext
{
public:

	/// @brief 太阳系数据
	const SolarSystem* solarSystem() const{return &m_solarSystem;}
    SolarSystem* solarSystem() {return &m_solarSystem;}

	/// @brief 地球指向数据
	const EOP* eop() const{return &m_eop;}
    EOP* eop() {return &m_eop;}

	/// @brief 闰秒数据
	const LeapSecond* leapSecond() const{return &m_leapSecond;}
    LeapSecond* leapSecond() {return &m_leapSecond;}

	/// @brief 数据目录
    std::string& dataDir() {return m_dataDir;}
    
	/// @brief 设置数据目录
	void setDataDir(StringView dir){m_dataDir = std::string(dir);}

	/// @brief JPL DE星历数据
    const JplDe* jplDe() const {return &m_jplDe;}
    JplDe* jplDe() {return &m_jplDe;}

	/// @brief IAU XYS数据
	const IAUXYS* iauXYS() const {return &m_iauXYS;}
    IAUXYS* iauXYS() {return &m_iauXYS;}

	/// @brief IAU XYS预计算数据
	const IAUXYSPrecomputed* iauXYSPrecomputed() const {return &m_iauXYSPrecomputed;}
    IAUXYSPrecomputed* iauXYSPrecomputed() {return &m_iauXYSPrecomputed;}
	
protected:
	SolarSystem  			m_solarSystem;
	EOP		    			m_eop;	
	LeapSecond  			m_leapSecond;
    JplDe                   m_jplDe;
	IAUXYS                  m_iauXYS;				///< IAU XYS数据 @todo: 这个考虑更改为静态数据
	IAUXYSPrecomputed       m_iauXYSPrecomputed;	///< IAU XYS预计算数据 @todo: 这个考虑更改为静态数据
    std::string     		m_dataDir;				
};


constexpr size_t kSizeOfDataContext = sizeof(DataContext);
 
AST_NAMESPACE_END
 


