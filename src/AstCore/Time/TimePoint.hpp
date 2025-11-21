///
/// @file      TimePoint.hpp
/// @brief     绝对时间点
/// @details   ~
/// @author    jinke18
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
 
#include "AstGlobal.hpp"
#include "AstCore/Constants.h"      // for kJ2000XXXX
#include <stdint.h>                 // for int64_t

AST_NAMESPACE_BEGIN

class DateTime;

class LongDuration;
class ShortDuration;
class DaySecDuration;

class TAIScale;
class TDBScale;
class TTScale;
class UTCScale;


/// @brief 绝对时间点
/// @tparam TimeScale 时间尺度，TT TAI TDB UTC等
/// @tparam Duration  相对于时间尺度参考历元的时长
template<typename TimeScale, typename Duration>
class TimePoint
{
public:

public:
    Duration    duration_;
};


// typedef TimePoint<TDBScale, LongDuration> TDBTime;
// typedef TimePoint<TTScale, LongDuration> TTTime;
// typedef TimePoint<UTCScale, LongDuration> UTCTime;
typedef TimePoint<TAIScale, LongDuration> TAITime;

typedef TAITime AbsTime;
typedef TAITime AstTime;

  
AST_NAMESPACE_END
