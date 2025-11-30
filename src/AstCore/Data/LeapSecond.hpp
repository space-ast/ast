///
/// @file      LeapSecond.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include <stdio.h>
#include <vector>

AST_NAMESPACE_BEGIN

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
	LeapSecond(const char* filepath);
	LeapSecond(const std::vector<double>& mJulianDate, const std::vector<double>& taiMinusUTC) {
		setData(mJulianDate, taiMinusUTC);
	}
public:
	/// @brief  从ATK格式文件加载闰秒数据
	/// @param  filepath - ATK格式文件路径
	/// @retval          - 错误码
    err_t loadATK(const char* filepath);

	/// @brief  从HPIERS格式文件加载闰秒数据
	/// @details 见https://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat
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
	void setData(const std::vector<double>& mjd, const std::vector<double>& taiMinusUTC);
	
public:
	double getLeapSecondByUTC(double jdUTCp1, double jdUTCp2);
	double getLeapSecondByTAI(double jdTAIp1, double jdTAIp2);
	
    /// @brief  获取当天的UTC秒数
	/// @param  jdUTCp1 - 
	/// @param  jdUTCp2 - 
	/// @retval          - 
	double getSecDayByUTC(double jdUTCp1, double jdUTCp2);
	
    /// @brief 考虑闰秒下，通过计算当天积秒计算标准化后的日期变动值和新积秒
	/// @param year   - 年(UTC)
	/// @param month  - 月(UTC)
	/// @param day    - 日(UTC)
	/// @param sec    - 当天积秒
	/// @param dday   - 时间格式标准化后的日期变动值
	/// @param newsec - 当天的新积秒
	void getTimeCorrectionByUTC(int year, int month, int day, double sec, int& dday, double& newsec);
private:

    /// @brief  计算当天的UTC秒数、当天的闰秒、明天的闰秒
    /// @param  jdUTCp1    - 
    /// @param  jdUTCp2    - 
    /// @param  leap        - 
    /// @param  leapNextDay - 
    /// @retval             - 
	double getLeapSecDayByUTC(double jdUTCp1, double jdUTCp2, double& leap, double& leapNextDay);
protected:
    std::vector<Entry> m_data;
};


AST_NAMESPACE_END
