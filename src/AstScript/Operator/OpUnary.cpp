///
/// @file      OpUnary.cpp
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
    
    // 检查是否缓存了函数指针，且类型匹配
    if (A_LIKELY(this->func_ && this->type_ == type)) {
        return this->func_(value.get());
    }
    
    // 尝试获取新的函数指针
    this->func_ = aGetOpUnaryFunc(op_, type);
    if(this->func_)
    {
        this->type_ = type;
        return this->func_(value.get());
    }
    
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