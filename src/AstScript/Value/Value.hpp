///
/// @file      Value.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstScript/Expr.hpp"

AST_NAMESPACE_BEGIN


/// @brief 值对象基类
/// @details 
/// 所有值对象的基类，包含值对象的公共接口。
/// 值对象用于表示表达式的求值结果，可以是各种类型的数据，如整数、浮点数、字符串、布尔值等。
/// 值对象的特点是“求值的结果等于自身”
class Value: public Expr
{
public:
    using Expr::Expr;
    ~Value() override = default;

    /// @brief 求值表达式
    Value* eval() const final{return const_cast<Value*>(this);}
    
    /// @brief 设置值
    err_t setValue(Value*) final{return eErrorReadonly;}
    
};

AST_NAMESPACE_END
