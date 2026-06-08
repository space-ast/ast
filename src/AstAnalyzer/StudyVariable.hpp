///
/// @file      StudyVariable.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-14
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
#include "AstUtil/ObjectNamed.hpp"
#include "AstScript/Expr.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

class AST_ANALYZER_API StudyVariable : public ObjectNamed
{
public:
    AST_OBJECT(StudyVariable)

    double startValue() const { return startValue_; }
    void setStartValue(double value) { startValue_ = value; }

    double endValue() const { return endValue_; }
    void setEndValue(double value) { endValue_ = value; }
    
    double stepSize() const { return stepSize_; }
    void setStepSize(double value) { stepSize_ = value; }

    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }

    errc_t getValue(double& value) const;
    errc_t setValue(double value);

    std::vector<double>& values() { return values_; }
    const std::vector<double>& values() const { return values_; }
private:
    double startValue_{0.0};        ///< 变量的起始值
    double endValue_{0.0};          ///< 变量的结束值
    double stepSize_{0.0};          ///< 分析步长
    SharedPtr<Expr> expr_;          ///< 遍历表达式
    std::vector<double> values_;    ///< 值列表
};


/*! @} */

AST_NAMESPACE_END
