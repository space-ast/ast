///
/// @file      Symbol.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
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

#pragma once

#include "AstGlobal.h"
#include "AstScript/Expr.hpp"
#include "AstUtil/StringView.hpp"
#include <string>

AST_NAMESPACE_BEGIN


/// @brief 符号表达式
/// @details 表示变量或函数的名称
class Symbol: public Expr{
public:
    AST_EXPR(Symbol)

    Symbol(StringView name)
        : name_(name) {}

    const std::string& name() const { return name_; }

    std::string getExpression(Object* object) const override {
        return name_;
    }
    Value* eval() const final;
    Expr* exec() const final;
    err_t setValue(Value* value) final;
private:
    std::string name_;
};

AST_NAMESPACE_END

