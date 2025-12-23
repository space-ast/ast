///
/// @file      Expr.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstUtil/Object.hpp"
#include "AstScript/ScriptAPI.hpp"

AST_NAMESPACE_BEGIN

class Value;   

/// @brief 表达式基类
/// @details 
/// 表达式是脚本语言中的基本构造单元，可以是常量、变量、运算符表达式、函数调用等
/// 不同类型的表达式通过继承自Expr类来实现多态行为
/// 表达式可以嵌套组合，形成复杂的计算逻辑
/// 表达式的特点是“可以求值”，即每个表达式在求值后都会产生一个结果值
class Expr: public Object
{
public:
    using Object::Object;
    ~Expr() override = default;

    /// @brief 接受表达式访问者
    /// @param visitor 表达式访问者对象
    virtual void accept(ExprVisitor& visitor) = 0;

    /// @brief 求值
    /// @return Value* 求值结果
    virtual Value* eval() const = 0;

    /// @brief 执行
    /// @return Expr* 执行结果
    virtual Expr* exec() const {return const_cast<Expr*>(this);};
    
    /// @brief 设置表达式的值
    virtual err_t setValue(Value* val) = 0;

    /// @brief 获取表达式的字符串表示
    /// @param context 可选的上下文对象，用于解析变量等
    /// @return std::string 表达式的字符串表示
    virtual std::string getExpression(Object* context=nullptr) const = 0;
};


AST_NAMESPACE_END
