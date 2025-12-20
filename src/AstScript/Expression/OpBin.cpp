///
/// @file      OpBin.cpp
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

#include "OpBin.hpp"

AST_NAMESPACE_BEGIN

static const char* OpBinTypeStr[] = {
    "+",    // eAdd
    "-",    // eSub
    "*",    // eMul
    "/",    // eDiv
    "^",    // ePow
    "%",    // eMod

    "&&",   // eAnd
    "||",   // eOr
    "|" ,    // eBitOr
    "&",    // eBitAnd
    aText("\u22BB"),    // eBitXor 位异或运算符 ⊻
    "<<",   // eBitLeftShift (逻辑/算术左移)
    ">>",   // eBitRightShift (算术右移)
    ">>>",  // eBitURightShift (逻辑右移)

    ".*",   // eElemMul
    "./",   // eElemDiv
    ".^",   // eElemPow
    ".%",   // eElemMod
    ".&",   // eElemAnd
    ".|",   // eElemOr

    "<",    // eLt
    "<=",   // eLe
    "==",   // eEq
    ">",    // eGt
    ">=",   // eGe
    "!=",   // eNe
};

static_assert(sizeof(OpBinTypeStr)/sizeof(OpBinTypeStr[0]) == static_cast<size_t>(numOpBinType), "OpBinTypeStr size mismatch");

Value *OpBin::eval() const
{
    // @todo 实现二元运算符的求值逻辑
    return nullptr;
}

std::string OpBin::getExpression(Object *context) const
{
    return std::string("(") + left_->getExpression(context) + " " +
           OpBinTypeStr[op_] + " " +
           right_->getExpression(context) + ")";
}

AST_NAMESPACE_END