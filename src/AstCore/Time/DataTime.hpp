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

AST_NAMESPACE_BEGIN


/// @brief 日期时间
class DateTime
{
public:
    const Date& date() const{return date_;}
    Date& date() {return date_;}
    const Time& time() const {return time_;}
    Time& time() {return time_;}

public:
    Date date_;     ///< 日期
    Time time_;     ///< 时间
};



AST_NAMESPACE_END
 
