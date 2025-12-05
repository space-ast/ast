///
/// @file      TimeSystem.hpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      21.11.2025
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
#include "AstCore/Constants.h"
 
AST_NAMESPACE_BEGIN

class JulianDate;
class DateTime;

template<typename Time>
A_ALWAYS_INLINE Time aTTToTAI(const Time& tmTT)
{
    return tmTT - kTTMinusTAI;
}


/// @brief 转换 TT 时间到 TAI 时间
/// @param tmTT TT 时间
/// @param tmTAI TAI 时间
template<typename Time>
A_ALWAYS_INLINE void aTTToTAI(const Time& tmTT, Time& tmTAI)
{
    tmTAI = aTTToTAI(tmTT);
}

template<typename Time>
A_ALWAYS_INLINE Time aTAIToTT(const Time& tmTAI)
{
    return tmTAI + kTTMinusTAI;
}


/// @brief 转换 TAI 时间到 TT 时间
/// @param tmTAI TAI 时间
/// @param tmTT TT 时间
template<typename Time>
A_ALWAYS_INLINE void aTAIToTT(const Time& tmTAI, Time& tmTT)
{
    tmTT = aTAIToTT(tmTAI);
}



/// @brief 转换 TAI 时间到 UTC 时间
/// @param jdTAI TAI 时间
/// @param jdUTC UTC 时间
AST_CORE_CAPI void aTAIToUTC(const JulianDate& jdTAI, JulianDate& jdUTC);
AST_CORE_API void aTAIToUTC(const DateTime& dtTAI, DateTime& dtUTC);

/// @brief 转换 UTC 时间到 TAI 时间
/// @param jdUTC UTC 时间
/// @param jdTAI TAI 时间
AST_CORE_CAPI void aUTCToTAI(const JulianDate& jdUTC, JulianDate& jdTAI);
AST_CORE_API void aUTCToTAI(const DateTime& dtUTC, DateTime& dtTAI);

/// @brief 转换 UTC 时间到 TT 时间
/// @param jdUTC UTC 时间
/// @param jdTT TT 时间
AST_CORE_CAPI void aUTCToTT(const JulianDate& jdUTC, JulianDate& jdTT);
AST_CORE_API void aUTCToTT(const DateTime& dtUTC, DateTime& dtTT);

/// @brief 转换 TT 时间到 UTC 时间
/// @param jdTT TT 时间
/// @param jdUTC UTC 时间
AST_CORE_CAPI void aTTToUTC(const JulianDate& jdTT, JulianDate& jdUTC);
AST_CORE_API void aTTToUTC(const DateTime& dtTT, DateTime& dtUTC);


AST_NAMESPACE_END
 
