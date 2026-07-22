///
/// @file      FieldOfViewConstraint.cpp
/// @brief     视场约束实现
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



#include "FieldOfViewConstraint.hpp"
#include "AstCore/Point.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

FieldOfViewConstraint::FieldOfViewConstraint(
    Point* fromObject, Point* toObject, FieldOfView* fov)
    : fromObject_(fromObject)
    , toObject_(toObject)
    , fov_(fov)
{
}

double FieldOfViewConstraint::evaluate(const TimePoint& time) const
{
    if (!fromObject_ || !toObject_ || !fov_) { return -1.0; }

    // 获取目标在观测者坐标系中的位置
    Vector3d posTo;
    toObject_->getPosIn(fromObject_->getFrame(), time, posTo);

    return fov_->angularMargin(posTo);
}

AST_NAMESPACE_END
