///
/// @file      EarthOrientation.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/BodyOrientation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup SolarSystem
    @{
*/


/// @brief 地球旋转类
class EarthOrientation final: public BodyOrientation
{
public:
    EarthOrientation() = default;
    ~EarthOrientation() override = default;
    void getICRFToFixedTransform(const TimePoint& tp, Rotation &rotation) const override;
    void getICRFToFixedTransform(const TimePoint& tp, KinematicRotation &rotation) const override;
    void getICRFToInertialTransform(const TimePoint& tp, Rotation &rotation) const override;
    Axes* getMODParent() const override;
    void getMODTransform(const TimePoint& tp, Rotation &rot) const override;
    Axes* getTODParent() const override;
    void getTODTransform(const TimePoint& tp, Rotation &rot) const override;
};



/*! @} */

AST_NAMESPACE_END
