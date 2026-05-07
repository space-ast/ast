///
/// @file      ShooterResult.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-28
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
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 结果变量，用于定义微分修正中的结果变量
class AST_CORE_API ShooterResult : public ObjectNamed
{
public:
    AST_OBJECT(ShooterResult)
    AST_PROPERT(active)
    AST_PROPERT(desired)
    AST_PROPERT(scale)
    AST_PROPERT(tolerance)
    AST_PROPERT(valid)
    AST_PROPERT(weight)
    static ShooterResult* New();

    ShooterResult() = default;
    ~ShooterResult() override = default;
    
PROPERTIES:
    bool active() const { return active_; }
    void setActive(bool active) { active_ = active; }

    double desired() const { return desired_; }
    void setDesired(double desired) { desired_ = desired; }
    
    double scale() const { return scale_; }
    void setScale(double scale) { scale_ = scale; }
    
    double tolerance() const { return tolerance_; }
    void setTolerance(double tolerance) { tolerance_ = tolerance; }
    
    bool valid() const { return valid_; }
    void setValid(bool valid) { valid_ = valid; }
    
    double weight() const { return weight_; }
    void setWeight(double weight) { weight_ = weight; }
    
    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }
private:
    bool active_{false};        ///< 是否激活
    bool valid_{true};          ///< 数值是否有效
    WeakPtr<Expr> expr_;        ///< 计算表达式
    double desired_{0.0};       ///< 目标值
    double scale_{1.0};         ///< 缩放因子
    double tolerance_{0.0};     ///< 容差
    double weight_{1.0};        ///< 权重
};

AST_NAMESPACE_END