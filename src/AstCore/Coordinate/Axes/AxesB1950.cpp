///
/// @file      AxesB1950.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "AxesB1950.hpp"
#include "AxesJ2000.hpp"
#include "AstCore/InertialFrame.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

AxesB1950 *AxesB1950::Instance()
{
    static SharedPtr<AxesB1950> instance(new AxesB1950);
    return instance.get();
}

Axes *AxesB1950::getParent() const
{
    return AxesJ2000::Instance();
}

err_t AxesB1950::getTransform(const TimePoint &tp, Rotation &rotation) const
{
    A_UNUSED(tp);
    aJ2000ToB1950Transform(rotation);
    return eNoError;
}

err_t AxesB1950::getTransform(const TimePoint &tp, KinematicRotation &rotation) const
{
    A_UNUSED(tp);
    aJ2000ToB1950Transform(rotation.getRotation());
    rotation.setRotationRate(Vector3d::Zero());
    return eNoError;
}

AST_NAMESPACE_END


