///
/// @file      AttitudeAircraftZDown.hpp
/// @brief     对地指向 + ECF 速度约束的机体 Z 朝下姿态
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
#include "AttitudeTrajectoryRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 对地指向 + ECF 速度约束的机体 Z 朝下姿态
/// @details "ECF Velocity Alignment with Nadir Constraint" 
/// 即飞机、船舶、地面车辆等"大圆弧载体"的默认姿态：
/// 体 X 轴严格对齐地固系速度方向(机头指向)，体 Z 轴在保持 X 轴对齐的前提下尽量指向地心 (机体 Z 朝下)
class AST_CORE_API AttitudeAircraftZDown : public AttitudeTrajectoryRelated
{
public:
    AST_OBJECT(AttitudeAircraftZDown)

    AttitudeAircraftZDown() = default;
    AttitudeAircraftZDown(Point* point, Body* body);
    ~AttitudeAircraftZDown() override = default;
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
