///
/// @file      EventTimeExplicit.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-14
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/EventTime.hpp"
#include "AstCore/TimePoint.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 显式事件时间
class AST_CORE_API EventTimeExplicit final: public EventTime
{
public:
    AST_OBJECT(EventTimeExplicit)
    AST_PROPERT(Time)
    static EventTimeExplicit* New(const TimePoint& time);
    static SharedPtr<EventTimeExplicit> MakeShared(const TimePoint& time);

    EventTimeExplicit() = default;
    explicit EventTimeExplicit(const TimePoint& time);
    ~EventTimeExplicit() override = default;
PROPERTIES:
    errc_t getTime(TimePoint& time) const override;
    void setTime(const TimePoint& time);
protected:
    TimePoint time_{};
};

/*! @} */

AST_NAMESPACE_END
