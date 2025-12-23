///
/// @file      Symbol.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-21
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
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Symbol.hpp"
#include "AstScript/ScriptContext.hpp"
#include "AstScript/SymbolScope.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

Value *Symbol::eval() const
{
    auto expr = exec();
    if(A_UNLIKELY(!expr))
    {
        // aError("symbol %s is not found", name_.c_str());
        return nullptr;
    }
    return expr->eval();
}

Expr *Symbol::exec() const
{
    auto expr = aScriptContext_ResolveSymbol(this);
    if(A_UNLIKELY(!expr))
    {
        aError("symbol %s is not found", name_.c_str());
        return nullptr;
    }
    return expr->exec();  // @fixme: 这里是否需要递归执行？
}

err_t Symbol::setValue(Value *value)
{
    auto expr = exec();
    if(A_UNLIKELY(!expr))
    {
        aError("symbol %s is not found", name_.c_str());
        return err_t();
    }
    return expr->setValue(value);
}

AST_NAMESPACE_END

