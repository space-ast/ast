///
/// @file      AstScriptAPI.cpp
/// @brief     ~
/// @details   ~
/// @author    jinke18
/// @date      2025-12-19
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

#include "AstScriptAPI.hpp"
#include "AstScript/ValBool.hpp"
#include "AstScript/ValInt.hpp"
#include "AstScript/ValDouble.hpp"
#include "AstScript/ValString.hpp"
#include "AstScript/OpBin.hpp"

AST_NAMESPACE_BEGIN

Expr *aParseExpr(StringView script)
{
    // @todo 实现表达式解析逻辑
    return nullptr;
}


Expr *aNewOpBin(OpBinType op, Expr *left, Expr *right)
{
    if(!left || !right){
        return nullptr;
    }
    return new OpBin(op, left, right);
}


Value* aNewValueString(StringView value)
{
    return new ValString(value);
}
Value* aNewValueInt(int value)
{
    return new ValInt(value);
}
Value* aNewValueBool(bool value)
{
    return new ValBool(value);
}
Value* aNewValueDouble(double value)
{
    return new ValDouble(value);
}


AST_NAMESPACE_END
