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
 
#include "AstGlobal.h"
#include "Duration.hpp"             // for LongDuration
#include "AstCore/Constants.h"      // for kJ2000XXXX
#include "AstCore/Constants.h"
#include "AstCore/JulianDate.hpp"    
#include "AstCore/DateTime.hpp"
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

class TimePoint;

/// @brief 将时间点转换为儒略日数（UTC）
AST_CORE_API void aTimePointToUTC(const TimePoint& time, JulianDate& jdUTC);

/// @brief 将时间点转换为儒略日数（地球时TT）
AST_CORE_API void aTimePointToTT(const TimePoint& time, JulianDate& jdTT);

/// @brief 将时间点转换为儒略日数（质心动力学时TDB）
AST_CORE_API void aTimePointToTDB(const TimePoint& time, JulianDate& jdTDB);

/// @brief 将时间点转换为儒略日数（原子时 TAI）
AST_CORE_API void aTimePointToTAI(const TimePoint& time, JulianDate& jdTAI);

/// @brief 将时间点转换为日期时间（UTC）
AST_CORE_API void aTimePointToUTC(const TimePoint& time, DateTime& dttmUTC);

/// @brief 将时间点转换为日期时间（地球时TT）
AST_CORE_API void aTimePointToTT(const TimePoint& time, DateTime& dttmTT);

/// @brief 将时间点转换为日期时间（质心动力学时TDB）
AST_CORE_API void aTimePointToTDB(const TimePoint& time, DateTime& dttmTDB);

/// @brief 将时间点转换为日期时间（原子时 TAI）
AST_CORE_API void aTimePointToTAI(const TimePoint& time, DateTime& dttmTAI);

/// @brief 绝对时间点
class TimePoint
{
public:
    AST_CORE_API
    static TimePoint FromUTC(const DateTime& dttmUTC);

    AST_CORE_API 
    static TimePoint FromTAI(const JulianDate& jdTAI);
protected:
    static TimePoint FromIntegerFractional(int64_t integer, double fractional);
public:
    /// @brief 时间点的整数秒数部分
    int64_t integerPart() const { return duration_.integer_; }

    /// @brief 时间点的小数秒数部分
    double fractionalPart() const { return duration_.fractional_; }
public:
    /// @brief 将时间点转换为儒略日数（地球时TT）
    void toTT(JulianDate& jdTT) const{
        aTimePointToTT(*this, jdTT);
    }
    /// @brief 将时间点转换为儒略日数（地球时TT）
    JulianDate toTT() const{
        JulianDate jdTT;
        toTT(jdTT);
        return jdTT;
    }
    /// @brief 将时间点转换为儒略日数（质心动力学时TDB）
    void toTDB(JulianDate& jdTDB) const{
        aTimePointToTDB(*this, jdTDB);
    }
    /// @brief 将时间点转换为儒略日数（质心动力学时TDB）
    JulianDate toTDB() const{
        JulianDate jdTDB;
        toTDB(jdTDB);
        return jdTDB;
    }
    /// @brief 将时间点转换为儒略日数（原子时 TAI）
    void toTAI(JulianDate& jdTAI) const{
        aTimePointToTAI(*this, jdTAI);
    }
    /// @brief 将时间点转换为儒略日数（原子时 TAI）
    JulianDate toTAI() const{
        JulianDate jdTAI;
        toTAI(jdTAI);
        return jdTAI;
    }

public:
    LongDuration    duration_;
};


// typedef TimePoint<TDBScale, LongDuration> TDBTime;
// typedef TimePoint<TTScale, LongDuration> TTTime;
// typedef TimePoint<UTCScale, LongDuration> UTCTime;
// typedef TimePoint<TAIScale, LongDuration> TAITime;

// typedef TAITime AbsTime;
// typedef TAITime AstTime;

  
AST_NAMESPACE_END
