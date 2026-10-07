///
/// @file      AttitudeNadirNormal.hpp
/// @brief     对地指向 + 轨道法向约束姿态
/// @details
/// @author    axel
/// @date      2026-09-28
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
#include "AttitudeAlignConstrain.hpp"
#include "AttitudeProfile.hpp"
#include "AttitudeTrajectoryRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 对地指向 + 轨道法向约束姿态
/// @details "Nadir Alignment with Orbit Normal Constraint"
/// 体 Z 轴严格对齐坐标系原点方向(对地指向)，体 X 轴在保持 Z 轴对齐的前提下尽量指向轨道法向(位置叉乘速度)
/// 对圆轨道而言该姿态与 VVLH 系只相差绕对地轴的一个固定转角
class AST_CORE_API AttitudeNadirNormal : public AttitudeTrajectoryRelated
{
public:
    AST_OBJECT(AttitudeNadirNormal)

    AttitudeNadirNormal() = default;
    AttitudeNadirNormal(Point* point, Body* body);
    AttitudeNadirNormal(Point* point, Frame* frame);
    ~AttitudeNadirNormal() override = default;
public:
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
};

/*! @} */

AST_NAMESPACE_END
