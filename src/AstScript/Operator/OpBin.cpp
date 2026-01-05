///
/// @file      OpBin.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
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

    // --- 双重检查锁定模式 (DCLP) 开始 ---
    // 1. 第一次检查：无锁读取缓存值
    // 使用 memory_order_acquire 确保在读取 func_ 之后，
    // 之前写入 func_、leftType_ 和 rightType_ 的操作对当前线程可见。
    OpBinFunc current_func = func_.load(std::memory_order_acquire);
    Class* current_leftType = leftType_.load(std::memory_order_acquire);
    Class* current_rightType = rightType_.load(std::memory_order_acquire);

    if (A_LIKELY(current_func && current_leftType == leftType && current_rightType == rightType)) {
        return current_func(leftval.get(), rightval.get());
    }

    // 2. 如果缓存未命中，则获取互斥锁并进行第二次检查
    std::lock_guard<std::mutex> lock(cache_mutex_);

    // 在锁内进行第二次检查，以避免重复计算（如果其他线程刚刚更新了缓存）
    // 此时在锁的保护下，可以使用 memory_order_relaxed，因为互斥锁本身提供了同步保证。
    current_func = func_.load(std::memory_order_relaxed);
    current_leftType = leftType_.load(std::memory_order_relaxed);
    current_rightType = rightType_.load(std::memory_order_relaxed);

    if (A_LIKELY(current_func && current_leftType == leftType && current_rightType == rightType)) {
        return current_func(leftval.get(), rightval.get());
    }

    // 3. 如果仍然未在缓存中找到，则计算并存储新的函数指针和类型
    OpBinFunc new_func = aGetOpBinFunc(op_, leftType, rightType);
    if (new_func) {
        // 使用 memory_order_release 确保在写入 func_ 之前，
        // 所有对 leftType_ 和 rightType_ 的写入操作都已完成并对其他线程可见。
        func_.store(new_func, std::memory_order_release);
        leftType_.store(leftType, std::memory_order_release);
        rightType_.store(rightType, std::memory_order_release);
        return new_func(leftval.get(), rightval.get());
    }
    // --- 双重检查锁定模式 (DCLP) 结束 ---

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