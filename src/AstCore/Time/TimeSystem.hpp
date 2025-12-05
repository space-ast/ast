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
#include "AstCore/DateTime.hpp"
#include "AstCore/JulianDate.hpp"
 
AST_NAMESPACE_BEGIN

class JulianDate;
class DateTime;


/// @brief 从原子时(TAI) 转换为 地球时(TT)
/// @param tmTAI 原子时(TAI)
/// @param tmTT 地球时(TT)
template<typename Time>
A_ALWAYS_INLINE void aTAIToTT(const Time& tmTAI, Time& tmTT)
{
    tmTT = aTAIToTT(tmTAI);
}
template<typename Time>
A_ALWAYS_INLINE Time aTAIToTT(const Time& tmTAI)
{
    return tmTAI + kTTMinusTAI;
}


/// @brief 从地球时(TT) 转换为 原子时(TAI)
/// @param tmTT 地球时(TT) 
/// @param tmTAI 原子时(TAI)
template<typename Time>
A_ALWAYS_INLINE void aTTToTAI(const Time& tmTT, Time& tmTAI)
{
    tmTAI = aTTToTAI(tmTT);
}
template<typename Time>
A_ALWAYS_INLINE Time aTTToTAI(const Time& tmTT)
{
    return tmTT - kTTMinusTAI;
}

/// @brief 从原子时(TAI) 转换为 GPS时
/// @param tmTAI 原子时(TAI)
/// @param tmGPS GPS时
template<typename Time>
A_ALWAYS_INLINE void aTAIToGPS(const Time& tmTAI, Time& tmGPS)
{
    tmGPS = tmTAI + kGPSMinusTAI;
}
template<typename Time>
A_ALWAYS_INLINE Time aTAIToGPS(const Time& tmTAI)
{
    return tmTAI + kGPSMinusTAI;
}

/// @brief 从GPS时 转换为 原子时(TAI)
/// @param tmGPS GPS时
/// @param tmTAI 原子时(TAI)
template<typename Time>
A_ALWAYS_INLINE void aGPSToTAI(const Time& tmGPS, Time& tmTAI)
{
    tmTAI = tmGPS - kGPSMinusTAI;
}
template<typename Time>
A_ALWAYS_INLINE Time aGPSToTAI(const Time& tmGPS)
{
    return tmGPS - kGPSMinusTAI;
}



/// @brief 从原子时(TAI) 转换为 协调世界时(UTC)
/// @param jdTAI 原子时(TAI)
/// @param jdUTC 协调世界时(UTC)
AST_CORE_API void aTAIToUTC(const JulianDate& jdTAI, JulianDate& jdUTC);
AST_CORE_API void aTAIToUTC(const DateTime& dttmTAI, DateTime& dttmUTC);
A_ALWAYS_INLINE JulianDate aTAIToUTC(const JulianDate& jdTAI)
{
    JulianDate jdUTC;
    aTAIToUTC(jdTAI, jdUTC);
    return jdUTC;
}
A_ALWAYS_INLINE DateTime aTAIToUTC(const DateTime& dttmTAI)
{
    DateTime dttmUTC;
    aTAIToUTC(dttmTAI, dttmUTC);
    return dttmUTC;
}




/// @brief 从协调世界时(UTC) 转换为 原子时(TAI)
/// @param jdUTC 协调世界时(UTC)
/// @param jdTAI 原子时(TAI)
AST_CORE_API void aUTCToTAI(const JulianDate& jdUTC, JulianDate& jdTAI);
AST_CORE_API void aUTCToTAI(const DateTime& dttmUTC, DateTime& dttmTAI);

A_ALWAYS_INLINE JulianDate aUTCToTAI(const JulianDate& jdUTC)
{
    JulianDate jdTAI;
    aUTCToTAI(jdUTC, jdTAI);
    return jdTAI;
}
A_ALWAYS_INLINE DateTime aUTCToTAI(const DateTime& dttmUTC)
{
    DateTime dttmTAI;
    aUTCToTAI(dttmUTC, dttmTAI);
    return dttmTAI;
}

/// @brief 从协调世界时(UTC) 转换为 地球时(TT)
/// @param jdUTC 协调世界时(UTC)
/// @param jdTT 地球时(TT)
AST_CORE_API void aUTCToTT(const JulianDate& jdUTC, JulianDate& jdTT);
AST_CORE_API void aUTCToTT(const DateTime& dttmUTC, DateTime& dttmTT);

A_ALWAYS_INLINE JulianDate aUTCToTT(const JulianDate& jdUTC)
{
    JulianDate jdTT;
    aUTCToTT(jdUTC, jdTT);
    return jdTT;
}
A_ALWAYS_INLINE DateTime aUTCToTT(const DateTime& dttmUTC)
{
    DateTime dttmTT;
    aUTCToTT(dttmUTC, dttmTT);
    return dttmTT;
}

/// @brief 从地球时(TT) 转换为 协调世界时(UTC)
/// @param jdTT 地球时(TT)
/// @param jdUTC 协调世界时(UTC)
AST_CORE_API void aTTToUTC(const JulianDate& jdTT, JulianDate& jdUTC);
AST_CORE_API void aTTToUTC(const DateTime& dttmTT, DateTime& dttmUTC);

A_ALWAYS_INLINE JulianDate aTTToUTC(const JulianDate& jdTT)
{
    JulianDate jdUTC;
    aTTToUTC(jdTT, jdUTC);
    return jdUTC;
}
A_ALWAYS_INLINE DateTime aTTToUTC(const DateTime& dttmTT)
{
    DateTime dttmUTC;
    aTTToUTC(dttmTT, dttmUTC);
    return dttmUTC;
}

/// @brief 从地球时(TT) 转换为质心动力学时(TDB)
/// @param jdTT 地球时(TT)
/// @param jdTDB 质心动力学时(TDB)
AST_CORE_API void aTTToTDB(const JulianDate& jdTT, JulianDate& jdTDB);
AST_CORE_API void aTTToTDB(const DateTime& dttmTT, DateTime& dttmTDB);
A_ALWAYS_INLINE JulianDate aTTToTDB(const JulianDate& jdTT)
{
    JulianDate jdTDB;
    aTTToTDB(jdTT, jdTDB);
    return jdTDB;
}
A_ALWAYS_INLINE DateTime aTTToTDB(const DateTime& dttmTT)
{
    DateTime dttmTDB;
    aTTToTDB(dttmTT, dttmTDB);
    return dttmTDB;
}


/// @brief 计算 质心动力学时(TDB) 时间与 地球时(TT) 时间的差值
/// @param jdTT 地球时(TT)
/// @return 质心动力学时(TDB) 时间与 地球时(TT) 时间的差值
AST_CORE_CAPI double aTDBMinusTT(const JulianDate& jdTT);


/// @brief 计算儒略世纪数，以 J2000.0 为基准
/// @param jd 儒略日
/// @return 儒略世纪数
AST_CORE_CAPI double aJulianCenturyFromJ2000(const JulianDate& jd);


AST_NAMESPACE_END
 
