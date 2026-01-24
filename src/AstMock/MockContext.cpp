///
/// @file      MockContext.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-24
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

#include "MockContext.hpp"
#include "AstCore/EOP.hpp"
#include "AstCore/DataContext.hpp"

AST_NAMESPACE_BEGIN

EOP eop;

EOP* aMockGetEOP_Extern()
{
    return &eop;
}

EOP* aMockGetEOP_Static()
{
    static EOP eop;
    return &eop;
}

static EOP* eop_lazy{nullptr};

EOP* aMockGetEOP_Lazy()
{
    if(!eop_lazy)
    {
        eop_lazy = new EOP();
    }
    return eop_lazy;
}

DataContext context;

EOP* aMockGetEOP_ContextExtern()
{
    return context.eop();
}

EOP* aMockGetEOP_ContextStatic()
{
    static DataContext context;
    return context.eop();
}

A_THREAD_LOCAL DataContext context_tl;

EOP* aMockGetEOP_ContextExternTL()
{
    return context_tl.eop();
}

EOP* aMockGetEOP_ContextStaticTL()
{
    static DataContext context;
    return context.eop();
}


AST_NAMESPACE_END
