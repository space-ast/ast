///
/// @file      ExprRange.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-26
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

#include "AstScript/ExprRange.hpp"
#include "AstScript/ValRange.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

ExprRange::ExprRange(Expr *start, Expr *stop, Expr *step)
    : start_(start), stop_(stop), step_(step)
{
}


Value *ExprRange::eval() const
{
    if(!start_ || !stop_){
        aError(_("start 或 stop 为空"));
        return nullptr;
    }
    double start = 0;
    double stop = 0;
    double step = 1.0;
    
    SharedPtr<Value> startVal = start_->eval();
    if(!aValueIsArithmetic(startVal.get())){
        aError(_("start 不是算术值"));
        return nullptr;
    }
    start = aValueToDouble(startVal.get());
    
    SharedPtr<Value> stopVal = stop_->eval();
    if(!aValueIsArithmetic(stopVal.get())){
        aError(_("stop 不是算术值"));
        return nullptr;
    }
    stop = aValueToDouble(stopVal.get());
    if(step_){
        SharedPtr<Value> stepVal = step_->eval();
        if(!aValueIsArithmetic(stepVal.get())){
            aError(_("step 不是算术值"));
            return nullptr;
        }
        step = aValueToDouble(stepVal.get());
    }

    return new ValRange(start, stop, step);
}


std::string ExprRange::getExpression(Object *context) const
{
    if (!start_ || !stop_) {
        return "(range error)";
    }

    std::string exprStr;
    exprStr += "(";
    exprStr += start_->getExpression(context);
    exprStr += ":";

    if (step_) {
        exprStr += step_->getExpression(context);
        exprStr += ":";
    }

    exprStr += stop_->getExpression(context);
    exprStr += ")";
    return exprStr;
}

AST_NAMESPACE_END