///
/// @file      LeapSecond.hpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      29.11.2025
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
#include <cstdio>
#include <vector>

AST_NAMESPACE_BEGIN


/*!
    @addtogroup Data
    @{
*/



class JulianDate;
class TimePoint;
class Date;

class AST_CORE_API LeapSecond
{
public:
    struct Entry
    {
        int mjd;
        int leapSecond;
    };
public:
    LeapSecond();
public:
	/// @brief  从ATK格式文件加载闰秒数据
	/// @param  filepath - ATK格式文件路径
	/// @retval          - 错误码
    err_t loadATK(const char* filepath);

	/// @brief  从HPIERS格式文件加载闰秒数据
	/// @details 见 <https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat>
	/// @param  filepath - HPIERS格式文件路径
	/// @retval          - 错误码
    err_t loadHPIERS(const char* filepath);

	/// @brief  从文件加载闰秒数据，按照默认格式进行加载
	/// @param  filepath - 文件路径
	/// @retval          - 错误码
    err_t load(const char* filepath);

    /// @brief 从默认文件加载闰秒数据
    /// @return 错误码
    err_t loadDefault();

	/// @brief  设置默认数据
	void setDefaultData();

	/// @brief  设置数据
	void setData(const std::vector<int>& mjd, const std::vector<int>& taiMinusUTC);
	
public:
	/// @brief  获取UTC秒数
	/// @param  jdUTC -  儒略日Julian Date (UTC)
	/// @retval       -  闰秒数
	double leapSecondUTC(ImpreciseJD jdUTC);
	double leapSecondUTCMJD(ImpreciseMJD mjdUTC);

	/// @brief  获取TAI秒数
	/// @param  jdTAI -  儒略日Julian Date (TAI)
	/// @retval       -  闰秒数
	double leapSecondTAI(ImpreciseJD jdTAI);
	double leapSecondTAIMJD(ImpreciseMJD mjdTAI);

	
    /// @brief  获取当天的UTC秒数
	/// @param  utcDate -  UTC日期
	/// @retval          -  当天的UTC秒数
	double getLodUTC(const Date& utcDate);

	/// @brief  获取当天的UTC秒数
	/// @param  mjdUTC -   简约儒略日Modified Julian Date (UTC)
	/// @retval          -  当天的UTC秒数
	double getLodUTCMJD(ImpreciseMJD mjdUTC);

	/// @brief  获取闰秒数据
	const std::vector<Entry>& data() const { return m_data; }
	std::vector<Entry>& data() { return m_data; }


protected:
    std::vector<Entry> m_data;
};


/*! @} */


AST_NAMESPACE_END
