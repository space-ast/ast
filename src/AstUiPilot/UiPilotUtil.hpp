///
/// @file      UiPilotUtil.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-12
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include <QMetaObject>
#include <QApplication>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


// @brief 添加一个排队执行的回调函数
// @param func 要排队执行的函数
template<typename Func>
void addQueued(Func &&func) {
    // 使用 Qt::QueuedConnection 确保任务在事件循环中排队执行
    QMetaObject::invokeMethod(qApp, std::forward<Func>(func), Qt::QueuedConnection);
}

/*! @} */

AST_NAMESPACE_END
