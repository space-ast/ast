///
/// @file      TimeList.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-07-03
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

#include "TimeList.hpp"
#include "TimeInterval.hpp"

AST_NAMESPACE_BEGIN

TimeList TimeList::FromInterval(const TimeInterval& interval,
                                const TimePoint& epoch,
                                double step)
{
	TimeList result(epoch);
	errc_t rc = interval.discrete(epoch, step, result.seconds());
	A_UNUSED(rc);
	return result;
}

TimeList TimeList::FromInterval(const TimeInterval &interval, double step)
{
	TimeList result;
	errc_t rc = interval.discrete(step, result);
	A_UNUSED(rc);
	return result;
}

AST_NAMESPACE_END
