///
/// @file      ExprAttribute.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "Expr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// 属性表达式
class AST_SCRIPT_API ExprAttribute : public Expr
{
public:
    AST_EXPR(ExprAttribute)

    /// 创建表达式
    /// @param attribute 属性
    /// @details 从属性创建表达式
    static ExprAttribute* New(const Attribute& attribute);

    /// 构造函数
    /// @param attribute 属性
    /// @details 从属性创建表达式
    ExprAttribute(const Attribute& attribute);

    /// 评估表达式
    /// @return 属性值
    /// @details 评估表达式，返回属性值
    Value* eval() const override;

    /// 设置属性值
    /// @param value 属性值
    /// @return 设置结果
    errc_t setValue(Value* value) override;

    /// 获取表达式字符串
    /// @return 表达式字符串
    /// @details 获取表达式字符串，用于调试和显示
    std::string getExpression(Object* context=nullptr) const override;


    /// 获取属性
    /// @return 属性
    /// @details 获取表达式中的属性
    const Attribute& attribute() const{return attribute_;}

    /// 设置属性
    /// @param attribute 属性
    /// @details 设置表达式中的属性
    void setAttribute(const Attribute& attribute){attribute_ = attribute;}
private:
    Attribute attribute_;    ///< 属性指针
};


/*! @} */

AST_NAMESPACE_END
