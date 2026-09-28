///
/// @file      AttitudeFixed.hpp
/// @brief     固定姿态(Fixed in Axes)
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
#include "AttitudeProfile.hpp"
#include "AstMath/Rotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 固定姿态剖面 (Fixed in Axes)
/// @details STK 的 "Fixed in Axes" / "Inertially Fixed" 剖面：
///          体轴系相对指定的参考坐标系(通过 setFrame 设置，默认地球惯性系)保持恒定取向。
///          参考坐标系取惯性系时即"惯性固定姿态"；取固连系时体轴系随地球一起转动。
class AST_CORE_API AttitudeFixed : public AttitudeProfile
{
public:
    AST_OBJECT(AttitudeFixed)

    AttitudeFixed() = default;
    ~AttitudeFixed() override = default;

    using AttitudeProfile::getTransform;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;

    /// @brief 获取本轴系相对父轴系的运动学旋转变换
    /// @details 固定姿态相对参考坐标系没有转动，角速度严格为零。这里直接给出闭式解，
    ///          既避免数值差分的误差，也保证长时间积分不会漂移。
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换(角速度恒为零)
    /// @return 错误码
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;

PROPERTIES:
    /// @brief 获取体轴系相对参考坐标系的取向
    const Rotation& getRotation() const { return rotation_; }
    /// @brief 设置体轴系相对参考坐标系的取向
    void setRotation(const Rotation& rotation) { rotation_ = rotation; }

protected:
    Frame* defaultFrame() const override;

protected:
    Rotation rotation_{Rotation::Identity()};   ///< 恒定的体轴系取向
};

/*! @} */

AST_NAMESPACE_END
