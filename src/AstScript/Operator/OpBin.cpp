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
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Logger.hpp"
#include "AstScript/Value.hpp"
#include "AstUtil/Class.hpp"

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
    if(A_UNLIKELY(!left_ || !right_))
    {
        return nullptr;
    }
    // 计算左操作数和右操作数的值
    SharedPtr<Value> leftval = left_->eval();
    SharedPtr<Value> rightval = right_->eval();
    
    // 检查左操作数和右操作数是否为null
    if(A_UNLIKELY(!leftval.get() || !rightval.get()))
    {
        aError("left or right value is null");
        return nullptr;
    }
    auto leftType = leftval->type();
    auto rightType = rightval->type();
    if(A_UNLIKELY(!leftType || !rightType)){
        aError("left or right value type is null");
        return nullptr;
    }

    // 检查是否缓存了函数指针，且参数类型匹配
    if(A_LIKELY(this->func_ && this->leftType_ == leftType && this->rightType_ == rightType)){
        return this->func_(leftval.get(), rightval.get());
    }

    // 尝试获取新的函数指针
    this->func_ = aGetOpBinFunc(op_, leftType, rightType);
    if(this->func_){
        // 缓存新的函数指针和参数类型
        this->leftType_ = leftType;
        this->rightType_ = rightType;
        return this->func_(leftval.get(), rightval.get());
    }
    // 未找到匹配的函数指针
    aError("no operator function found for %s %s %s",
           leftType->name().c_str(),
           OpBinTypeStr[op_],
           rightType->name().c_str());

    return nullptr;
}

std::string OpBin::getExpression(Object *context) const
{
    return std::string("(") + left_->getExpression(context) + " " +
           OpBinTypeStr[op_] + " " +
           right_->getExpression(context) + ")";
}

AST_NAMESPACE_END