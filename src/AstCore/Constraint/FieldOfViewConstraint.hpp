///
/// @file      FieldOfViewConstraint.hpp
/// @brief     视场约束
/// @details   检查目标是否在传感器的视场内。
/// @author    axel
/// @date      2026-07-22
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
#include "AstCore/FieldOfView.hpp"

AST_NAMESPACE_BEGIN

/// @brief 视场约束
/// @details 计算目标方向到视场边界的角度余量。
///          正值表示在视场内，负值表示在视场外，零为恰在边界。
class AST_CORE_API FieldOfViewConstraint : public AccessConstraint
{
public:
    AST_OBJECT(FieldOfViewConstraint)

    FieldOfViewConstraint() = default;

    /// @brief 构造视场约束
    /// @param fromObject 传感器所在的观测者
    /// @param toObject 待检测的目标
    /// @param fov 视场定义
    FieldOfViewConstraint(Point* fromObject, Point* toObject, FieldOfView* fov);

    double evaluate(const TimePoint& time) const override;

    void setFromObject(Point* obj) { fromObject_ = obj; }
    Point* fromObject() const { return fromObject_; }

    void setToObject(Point* obj) { toObject_ = obj; }
    Point* toObject() const { return toObject_; }

    void setFieldOfView(FieldOfView* fov) { fov_ = fov; }
    FieldOfView* fieldOfView() const { return fov_; }

private:
    Point*        fromObject_{};
    Point*        toObject_{};
    FieldOfView*  fov_{};
};

AST_NAMESPACE_END
