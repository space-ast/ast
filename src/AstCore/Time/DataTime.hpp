///
/// @file      DataTime.hpp
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
#include "Date.hpp"
#include "Time.hpp"


struct tm;

AST_NAMESPACE_BEGIN


class DateTime;

/// @brief 获取当前本地日期时间
AST_CORE_CAPI void aCurrentDateTimeLocal(DateTime& dt);

/// @brief 获取当前UTC日期时间
AST_CORE_CAPI void aCurrentDateTimeUTC(DateTime& dt);



/// @brief 日期时间
class DateTime
{
public:
    /// @brief 根据系统时间创建日期时间对象
    AST_CORE_API
    static DateTime FromTm(const tm* time);

    /// @brief 根据time_t创建本地时间的日期时间对象
    AST_CORE_API
    static DateTime FromTimeTLocal(time_t time);

    /// @brief 根据time_t创建UTC时间的日期时间对象
    AST_CORE_API
    static DateTime FromTimeTUTC(time_t time);

public:
    const Date& date() const{return date_;}
    Date& date() {return date_;}
    const Time& time() const {return time_;}
    Time& time() {return time_;}

public:
    Date date_;     ///< 日期
    Time time_;     ///< 时间
};

A_ALWAYS_INLINE
DateTime aCurrentDateTimeLocal()
{
    DateTime dt;
    aCurrentDateTimeLocal(dt);
    return dt;
}

A_ALWAYS_INLINE
DateTime aCurrentDateTimeUTC()
{
    DateTime dt;
    aCurrentDateTimeUTC(dt);
    return dt;
}


AST_NAMESPACE_END
 
