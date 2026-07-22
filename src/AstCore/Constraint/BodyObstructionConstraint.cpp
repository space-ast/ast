///
/// @file      BodyObstructionConstraint.cpp
/// @brief     天体遮挡约束实现
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



#include "BodyObstructionConstraint.hpp"
#include "AstCore/Point.hpp"
#include "AstCore/BodyObstruction.hpp"

AST_NAMESPACE_BEGIN

BodyObstructionConstraint::BodyObstructionConstraint(
    Point* fromObject, Point* toObject, CelestialBody* centralBody)
    : fromObject_(fromObject)
    , toObject_(toObject)
    , centralBody_(centralBody)
{
}

double BodyObstructionConstraint::evaluate(const TimePoint& time) const
{
    if (!fromObject_ || !toObject_ || !centralBody_) { return -1.0; }

    // 获取两点在天体固连系中的位置
    Frame* bodyFixed = centralBody_->frameFixed();
    Vector3d posFrom, posTo;
    fromObject_->getPosIn(bodyFixed, time, posFrom);
    toObject_->getPosIn(bodyFixed, time, posTo);

    return aLineOfSightClearance(posFrom, posTo, centralBody_->shape());
}

AST_NAMESPACE_END
