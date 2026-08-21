///
/// @file      DistanceConstraint.hpp
/// @brief     两点距离约束
/// @details   计算主、次两点之间的距离，并返回阈值减去距离的有符号特征值。
///            正值表示两点距离小于阈值（满足约束），负值表示大于阈值，零为边界。
///            与 AccessEvaluator 配合可用于搜索交会/接近时间窗。
/// @author    axel
/// @date      2026-08-19
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#include "AccessConstraint.hpp"

AST_NAMESPACE_BEGIN

class Point;

/// @brief 两点距离约束
/// @details 特征值 = 阈值 - 两点距离。两点须在同一坐标系下给出位置
///          （典型用法：两个 SGP4 预报器，均输出 TEME 系）。
class AST_CORE_API DistanceConstraint : public AccessConstraint
{
public:
    AST_OBJECT(DistanceConstraint)

    DistanceConstraint() = default;
    A_DISABLE_COPY(DistanceConstraint);

    /// @brief 构造两点距离约束
    /// @param primary   主对象
    /// @param secondary 次对象
    /// @param threshold 距离阈值 [m]
    DistanceConstraint(Point* primary, Point* secondary, double threshold);

    double evaluate(const TimePoint& time) const override;

    void setPrimary(Point* p) { primary_ = p; }
    Point* primary() const { return primary_; }

    void setSecondary(Point* p) { secondary_ = p; }
    Point* secondary() const { return secondary_; }

    void setThreshold(double v) { threshold_ = v; }
    double threshold() const { return threshold_; }

private:
    Point*  primary_{};     ///< 主对象
    Point*  secondary_{};   ///< 次对象
    double  threshold_{0.0};///< 距离阈值 [m]
};

AST_NAMESPACE_END
