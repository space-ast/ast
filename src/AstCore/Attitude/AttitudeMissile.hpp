///
/// @file      AttitudeMissile.hpp
/// @brief     ECI 速度对齐 + 对地约束姿态
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
#include "AttitudeTrajectoryRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief ECI 速度对齐 + 对地约束姿态
/// @details "ECI Velocity Alignment with Nadir Constraint"
/// 体 X 轴严格对齐惯性系速度方向(导弹纵轴指向)，体 Z 轴在保持 X 轴对齐的前提下尽量指向地心
/// 与 @see AttitudeAircraftZDown 的区别在于：计算该姿态的位置速度的参考系为天体惯性系，
/// 而不是天体固连系，故速度中不包含地球自转带来的牵连分量。
class AST_CORE_API AttitudeMissile : public AttitudeTrajectoryRelated
{
public:
    AST_OBJECT(AttitudeMissile)

    AttitudeMissile() = default;
    AttitudeMissile(Point* point, Body* body);
    ~AttitudeMissile() override = default;
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
