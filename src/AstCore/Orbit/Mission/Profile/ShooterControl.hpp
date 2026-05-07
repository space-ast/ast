///
/// @file      ShooterControl.hpp
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
#include "AstScript/Expr.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectNamed.hpp"
#include <string>

AST_NAMESPACE_BEGIN

/// @brief 控制变量，用于定义微分修正中的控制变量
class AST_CORE_API ShooterControl : public ObjectNamed
{
public:
    AST_OBJECT(ShooterControl)
    AST_PROPERT(active)
    AST_PROPERT(correction)
    AST_PROPERT(maxStep)
    AST_PROPERT(perturbation)
    AST_PROPERT(scale)
    AST_PROPERT(tolerance)
    AST_PROPERT(totalCorrection)
    static ShooterControl* New();
    
    ShooterControl() = default;
    ~ShooterControl() override = default;

PROPERTIES:
    bool active() const { return active_; }
    void setActive(bool active) { active_ = active; }

    double correction() const { return correction_; }
    void setCorrection(double correction) { correction_ = correction; }

    double maxStep() const { return maxStep_; }
    void setMaxStep(double maxStep) { maxStep_ = maxStep; }

    double perturbation() const { return perturbation_; }
    void setPerturbation(double perturbation) { perturbation_ = perturbation; }

    double scale() const { return scale_; }
    void setScale(double scale) { scale_ = scale; }

    double tolerance() const { return tolerance_; }
    void setTolerance(double tolerance) { tolerance_ = tolerance; }

    double totalCorrection() const { return totalCorrection_; }
    void setTotalCorrection(double totalCorrection) { totalCorrection_ = totalCorrection; }

    Expr* expr() const { return expr_.get(); }
    void setExpr(Expr* expr) { expr_ = expr; }
private:
    bool active_{true};             ///< 是否激活
    WeakPtr<Expr> expr_;            ///< 表达式
    double correction_{0.0};        ///< 修正值
    double maxStep_{100.0};         ///< 最大步长
    double perturbation_{0.1};      ///< 扰动值
    double scale_{1.0};             ///< 缩放因子
    double tolerance_{1e-6};        ///< 容差
    double totalCorrection_{0.0};   ///< 总修正值??
};

AST_NAMESPACE_END