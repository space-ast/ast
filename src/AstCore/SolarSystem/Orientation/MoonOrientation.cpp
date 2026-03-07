///
/// @file      MoonOrientation.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-06
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

#include "MoonOrientation.hpp"
#include "AstCore/BuiltinAxes.hpp"
#include "AstCore/MoonFrame.hpp"

AST_NAMESPACE_BEGIN

void MoonOrientation::getICRFToFixedTransform(const TimePoint &tp, Rotation &rotation) const
{
    aICRFToMoonMeanEarthTransform(tp, rotation);
}

void MoonOrientation::getICRFToFixedTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    aICRFToMoonMeanEarthTransform(tp, rotation.getRotation());
    /// @todo 实现动力学旋转
    rotation.setRotationRate(Vector3d::Zero());
}

void MoonOrientation::getICRFToInertialTransform(const TimePoint &tp, Rotation &rotation) const
{
    /// @todo 实现月球惯性系变换
}

Axes *MoonOrientation::getMODParent() const
{
    return aAxesICRF();
}

void MoonOrientation::getMODTransform(const TimePoint &tp, Rotation &rot) const
{
    /// @todo 实现月球MOD系变换
}

Axes *MoonOrientation::getTODParent() const
{
    return aAxesICRF();
}

void MoonOrientation::getTODTransform(const TimePoint &tp, Rotation &rot) const
{
    /// @todo 实现月球TOD系变换
}

AST_NAMESPACE_END
