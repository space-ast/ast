///
/// @file      BodyObstructionConstraint.hpp
/// @brief     天体遮挡约束
/// @details   检查两个对象之间的视线是否被中心天体遮挡。
/// @author    axel
/// @date      2026-07-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "AccessConstraint.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

/// @brief 天体遮挡约束
/// @details 计算从观测者到目标的视线到天体表面的最短有符号距离。
///          正值表示无遮挡，负值表示被遮挡，零为相切。
class AST_CORE_API BodyObstructionConstraint : public AccessConstraint
{
public:
    AST_OBJECT(BodyObstructionConstraint)

    BodyObstructionConstraint() = default;

    /// @brief 构造天体遮挡约束
    /// @param fromObject 观测者
    /// @param toObject 目标
    /// @param centralBody 中心天体
    BodyObstructionConstraint(Point* fromObject, Point* toObject, CelestialBody* centralBody);

    double evaluate(const TimePoint& time) const override;

    void setFromObject(Point* obj) { fromObject_ = obj; }
    Point* fromObject() const { return fromObject_; }

    void setToObject(Point* obj) { toObject_ = obj; }
    Point* toObject() const { return toObject_; }

    void setCentralBody(CelestialBody* body) { centralBody_ = body; }
    CelestialBody* centralBody() const { return centralBody_; }

private:
    Point*          fromObject_{};
    Point*          toObject_{};
    CelestialBody*  centralBody_{};
};

AST_NAMESPACE_END
