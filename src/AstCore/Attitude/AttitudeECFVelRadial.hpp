///
/// @file      AttitudeECFVelRadial.hpp
/// @brief     ECF 速度对齐 + 径向约束姿态
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
#include "AstCore/AttitudeTrajectoryRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief ECF 速度对齐 + 径向约束姿态
/// @details "ECF Velocity Alignment with Radial Constraint"
/// 体 X 轴严格对齐地固系速度方向，体 Z 轴在保持 X 轴对齐的前提下尽量指向径向(远离地心)
/// 常用于飞机、地面车辆等大气层内载体
class AST_CORE_API AttitudeECFVelRadial : public AttitudeTrajectoryRelated
{
public:
    AST_OBJECT(AttitudeECFVelRadial)

    AttitudeECFVelRadial() = default;
    AttitudeECFVelRadial(Point* point, Body* body);
    ~AttitudeECFVelRadial() override = default;
public:
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
private:
    // 屏蔽父类的 setFrame 方法
    void setFrame(Frame* frame) = delete;
public:
    void setBody(Body* body);
};

/*! @} */

AST_NAMESPACE_END
