///
/// @file      AttitudeRelSunLH.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-03
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

#pragma once

#include "AstGlobal.h"
#include "AstCore/AttitudeTrajectoryRelated.hpp"
#include "AstCore/AttitudeSunRelated.hpp"
#include "AstCore/CelestialBody.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 对原点对齐 + 太阳方向约束
/// @details "Nadir alignment with Sun constraint"
/// 体 Z 轴严格指向坐标系原点，体 X 轴在保持 Z 轴对齐的前提下尽量指向太阳方向，
class AST_CORE_API AttitudeRelSunLH : public AttitudeSunRelated
{
public:
    AttitudeRelSunLH() = default;
    AttitudeRelSunLH(Point* point, Frame* frame);
    ~AttitudeRelSunLH() = default;
public:
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
};


/*! @} */

AST_NAMESPACE_END


