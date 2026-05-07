///
/// @file      EventInterval.hpp
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include "AstCore/TimeInterval.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class EventInterval;
using PEventInterval = EventInterval*;
using HEventInterval = SharedPtr<EventInterval>;

/// @brief 事件时间段
class AST_CORE_API EventInterval: public ObjectNamed
{
public:
    /// @brief 获取时间段
    /// @param interval 
    /// @return 
    virtual errc_t getInterval(TimeInterval& interval) const = 0;
};

/*! @} */

AST_NAMESPACE_END
