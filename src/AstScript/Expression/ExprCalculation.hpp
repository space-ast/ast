///
/// @file      ExprCalculation.hpp
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
#include "AstUtil/ObjectCalculation.hpp"


AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


class AST_SCRIPT_API ExprCalculationBase : public Expr
{
public:


};



class AST_SCRIPT_API ExprCalculation : public ExprCalculationBase
{
public:
    AST_EXPR(ExprCalculation)

    /// @brief 创建新的计算表达式
    /// @param obj 要计算的对象
    /// @param calc 要使用的计算式
    /// @return 计算表达式
    static ExprCalculation* New(Object* obj=nullptr, ObjectCalculation* calc=nullptr);


    ExprCalculation() = default;
    ExprCalculation(Object* obj, ObjectCalculation* calc);
    ~ExprCalculation() = default;

    Value* eval() const override;

    errc_t setValue(Value* val) override;

    std::string getExpression(Object* context=nullptr) const override;

public:
    /// @brief 计算表达式
    /// @param val 计算结果
    /// @return 错误码
    errc_t calculate(double& val);

public:
    Object* object() const {return object_.get();}
    void setObject(Object* obj);
    
    ObjectCalculation* calculation() const {return calculation_.get();}
    void setCalculation(ObjectCalculation* calc);
private:
    void syncTypeMatched();
private:
    bool                        typeMatched_{false};    ///< 是否类型匹配
    WeakPtr<Object>             object_;                ///< 对象
    WeakPtr<ObjectCalculation>  calculation_;           ///< 计算式
};

/*! @} */

AST_NAMESPACE_END


