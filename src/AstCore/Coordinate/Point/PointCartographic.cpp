///
/// @file      PointCartographic.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-01
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

#include "PointCartographic.hpp"
#include "AstCore/BodyShape.hpp"
#include "AstCore/LocalHorizonalFrame.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

PointCartographic::PointCartographic(Body* body, const GeodeticPoint& position)
    : position_(position)
    , body_(body)
    , shape_(body? body->getShape() : nullptr)
{
}

PointCartographic::PointCartographic(Body *body)
    : position_{}
    , body_(body)
    , shape_(body? body->getShape() : nullptr)
{
}

Frame *PointCartographic::getFrame() const
{
    Body* body = body_.get();
    if (!body) {
        return nullptr;
    }
    return body->getFrameFixed();
}

errc_t PointCartographic::getPos(const TimePoint& tp, Vector3d& pos) const
{
    (void)tp; ///< 点固定于天体固连坐标系，位置不随时间变化
    BodyShape* shape = shape_.get();
    if (shape) {
        pos = shape->transform(position_);
    } else {
        aWarning("shape is null, use sphere approximation");
        // 无形状数据时，以球体近似处理
        Body* body = body_.get();
        if (!body) {
            return eErrorInvalidParam;
        }
        aGeodeticToBodyFixed(position_, pos, body->getRadius());
    }
    return eNoError;
}

errc_t PointCartographic::getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const
{
    vel.setZero();
    return getPos(tp, pos);
}

void PointCartographic::setBody(Body *body)
{
    body_ = body;
    shape_ = body ? body->getShape() : nullptr;
}

AST_NAMESPACE_END
