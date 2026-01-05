/// @file      Time.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      4.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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
 

#include "Time.hpp"
#include <math.h>       // for floor
#include <algorithm>	// for std::min
#include "AstCore/MathOperator.hpp"

AST_NAMESPACE_BEGIN

void aTimeNormalize(Time &time)
{
	int rmin = (int)floor(time.second() / 60);
	time.second() -= rmin * 60;
	time.minute() += rmin;

	int rhour = (int)floor(time.minute() / 60.);
	time.minute() -= rhour * 60;
	time.hour() += rhour;
}

void aTimeFromTotalSecond(double totalSecond, Time &time)
{
	using std::min;
	using std::max;


	time.hour() = max(min((int)floor(totalSecond / 3600.), 23), 0);
	double remainSec = totalSecond - time.hour() * 3600;
	time.minute() = max(min((int)floor(remainSec / 60.), 59), 0);
	time.second() = totalSecond - (time.minute() * 60 + time.hour() * 3600);
}

AST_NAMESPACE_END
