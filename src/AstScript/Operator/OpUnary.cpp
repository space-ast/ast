///
/// @file      OpUnary.cpp
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

#include "OpUnary.hpp"
#include "AstUtil/SharedPtr.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Class.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/OpUnaryPredefined.hpp"

AST_NAMESPACE_BEGIN

const char* OpUnaryTypeStr[] = {
    "!",    // eNot
    "-",    // eNeg
    "++",   // eInc
    "--",   // eDec
    "~",    // eTilde (按位取反)
};

static_assert(sizeof(OpUnaryTypeStr)/sizeof(OpUnaryTypeStr[0]) == static_cast<size_t>(numOpUnaryType), "OpUnaryTypeStr size mismatch");

Value *OpUnary::eval() const
{
    // 计算操作数的值
    SharedPtr<Value> value = expr_->eval();

    /// 检查操作数是否为空
    if (A_UNLIKELY(!value.get())) {
        aError("Failed to evaluate operand for unary operator");
        return nullptr;
    }
    auto type = value->type();
    if (A_UNLIKELY(!type)) {
        aError("Failed to get type of operand for unary operator");
        return nullptr;
    }
    
    // --- 双重检查锁定模式 (DCLP) 开始 ---
    // 1. 第一次检查：无锁读取缓存值
    // 使用 memory_order_acquire 确保在读取 func_ 之后，
    // 之前写入 func_、type_ 的操作对当前线程可见。
    OpUnaryFunc current_func = func_.load(std::memory_order_acquire);
    Class* current_type = type_.load(std::memory_order_acquire);

    if (A_LIKELY(current_func && current_type == type)) {
        return current_func(value.get());
    }
    
    // 2. 如果缓存未命中，则获取互斥锁并进行第二次检查
    std::lock_guard<std::mutex> lock(cache_mutex_);

    // 在锁内进行第二次检查，以避免重复计算（如果其他线程刚刚更新了缓存）
    // 此时在锁的保护下，可以使用 memory_order_relaxed，因为互斥锁本身提供了同步保证。
    current_func = func_.load(std::memory_order_relaxed);
    current_type = type_.load(std::memory_order_relaxed);

    if (A_LIKELY(current_func && current_type == type)) {
        return current_func(value.get());
    }

    // 3. 如果仍然未在缓存中找到，则计算并存储新的函数指针和类型
    OpUnaryFunc new_func = aGetOpUnaryFunc(op_, type);
    if (new_func) {
        // 使用 memory_order_release 确保在写入 func_ 之前，
        // 所有对 leftType_ 和 rightType_ 的写入操作都已完成并对其他线程可见。
        func_.store(new_func, std::memory_order_release);
        type_.store(type, std::memory_order_release);
        return new_func(value.get());
    }
    // --- 双重检查锁定模式 (DCLP) 结束 ---

    // 未找到匹配的函数指针
    aError("no operator function found for %s %s", 
        type->name().c_str(),
        OpUnaryTypeStr[op_]
    );
    return nullptr;
}

std::string OpUnary::getExpression(Object *context) const
{
    return std::string(OpUnaryTypeStr[static_cast<size_t>(op_)]) + expr_->getExpression(context);
}

AST_NAMESPACE_END