///
/// @file      DistanceConstraint.cpp
/// @brief     两点距离约束实现
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

#include "DistanceConstraint.hpp"
#include "AstCore/Point.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

DistanceConstraint::DistanceConstraint(Point* primary, Point* secondary, double threshold)
    : primary_(primary)
    , secondary_(secondary)
    , threshold_(threshold)
{
}

double DistanceConstraint::evaluate(const TimePoint& time) const
{
    if (!primary_ || !secondary_) { return -1.0; }

    Vector3d posPrimary, posSecondary;
    auto frame = primary_->getFrame();
    primary_->getPos(time, posPrimary);
    secondary_->getPosIn(frame, time, posSecondary);

    double distance = (posPrimary - posSecondary).norm();
    return threshold_ - distance;
}

AST_NAMESPACE_END
