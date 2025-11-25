/// @file      GlobalContext.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      25.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

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

#include "GlobalContext.hpp"
#include <assert.h>

AST_NAMESPACE_BEGIN


// 线程本地存储的当前全局上下文指针
A_THREAD_LOCAL GlobalContext* t_currentGlobalContext = nullptr;


GlobalContext* aGlobalContext_GetCurrent()
{
	assert(t_currentGlobalContext && "Current GlobalContext is null!");
    return t_currentGlobalContext;
}

void aGlobalContext_SetCurrent(GlobalContext* context)
{
    t_currentGlobalContext = context;
}

GlobalContext* aGlobalContext_New()
{
    return new GlobalContext{};
}

  
AST_NAMESPACE_END
 


