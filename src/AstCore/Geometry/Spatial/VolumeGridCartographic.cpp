///
/// @file      VolumeGridCartographic.cpp
/// @brief     经纬高体积网格实现
/// @author    axel
/// @date      2026-07-29
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

#include "VolumeGridCartographic.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

VolumeGridCartographic::VolumeGridCartographic()
{
    setIsComponent(true);
}

void VolumeGridCartographic::setCentralBody(CelestialBody* body)
{
    body_ = body;
}

size_t VolumeGridCartographic::getGridPointCount() const
{
    return static_cast<size_t>(lat_.numSteps_) * static_cast<size_t>(lon_.numSteps_) * static_cast<size_t>(alt_.numSteps_);
}

void VolumeGridCartographic::enumerateGridPoints(std::vector<Vector3d>& points) const
{
    (void)points;
    // Phase 2: 实现网格点枚举
}

void VolumeGridCartographic::getBoundingBox(Vector3d& minCorner, Vector3d& maxCorner) const
{
    (void)minCorner;
    (void)maxCorner;
    // Phase 2: 实现包围盒计算
}

Frame *VolumeGridCartographic::getFrame() const
{
    if(auto body = body_.get())
    {
        return body->getFrameFixed();
    }
    return nullptr;
}

AST_NAMESPACE_END
