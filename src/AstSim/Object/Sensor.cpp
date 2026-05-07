///
/// @file      Sensor.cpp
/// @brief     传感器对象实现
/// @author    axel
/// @date      2026-04-09
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

#include "Sensor.hpp"
#include "AstCore/Frame.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN


Sensor::Sensor()
{

}

Frame *Sensor::getFrame() const
{
    if(!location_.expired())
    {
        return location_->getFrame();
    }
    return nullptr;
}

errc_t Sensor::getPos(const TimePoint& tp, Vector3d& pos) const
{
    if(!location_.expired())
    {
        return location_->getPos(tp, pos);
    }
    return eErrorNullPtr;
}

errc_t Sensor::getPosVel(const TimePoint& tp, Vector3d& pos, Vector3d& vel) const
{
    if(!location_.expired())
    {
        return location_->getPosVel(tp, pos, vel);
    }
    return eErrorNullPtr;
}

void Sensor::setFieldOfView(FieldOfView *fov)
{
    if(fov)
    {
        fov_ = fov;
    }
}

AST_NAMESPACE_END