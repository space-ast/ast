///
/// @file      AttitudeSpinning.hpp
/// @brief     自旋姿态(Spinning)
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
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 自旋姿态剖面 (Spinning)
/// @details STK 的 "Spinning" 剖面：载体绕一根**惯性固连**的自旋轴以恒定角速度旋转。
///          自旋轴用两个方向指定：参考坐标系下的方向 a 与体系下的方向 b
///          (通常即某个体轴，例如绕体 Z 轴自旋)。二者的基准取向取最短弧旋转，
///          即绕 a x b 转 a 与 b 之间的夹角。
///
///          正的自旋速率表示绕自旋轴的**右手**旋转，t 时刻的姿态为
///          M(t) = Rot(spinOffset + spinRate * (t - epoch), a) * M0，
///          其中 Rot(theta, a) 是绕 a 右手旋转 theta 的参考系到体系转换矩阵。
class AST_CORE_API AttitudeSpinning : public AttitudeProfile
{
public:
    AST_OBJECT(AttitudeSpinning)

    AttitudeSpinning() = default;
    ~AttitudeSpinning() override = default;

    using AttitudeProfile::getTransform;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;

    /// @brief 获取本轴系相对父轴系的运动学旋转变换
    /// @details 自旋轴在参考坐标系下固定，角速度恒为 spinRate * a，这里直接给出闭式解。
    /// @param tp 时间点
    /// @param rotation 输出参数，运动学旋转变换
    /// @return 错误码
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;

PROPERTIES:
    /// @brief 获取参考坐标系下的自旋轴方向
    const Vector3d& getSpinAxisInFrame() const { return spinAxisInFrame_; }
    /// @brief 设置参考坐标系下的自旋轴方向
    void setSpinAxisInFrame(const Vector3d& axis) { spinAxisInFrame_ = axis; }

    /// @brief 获取体系下的自旋轴方向
    const Vector3d& getSpinAxisInBody() const { return spinAxisInBody_; }
    /// @brief 设置体系下的自旋轴方向(通常是某个体轴)
    void setSpinAxisInBody(const Vector3d& axis) { spinAxisInBody_ = axis; }

    /// @brief 获取自旋角速度 [rad/s]
    double getSpinRate() const { return spinRate_; }
    /// @brief 设置自旋角速度 [rad/s]，正值表示绕自旋轴右手旋转
    void setSpinRate(double rate) { spinRate_ = rate; }

    /// @brief 获取自旋初相角 [rad]
    double getSpinOffset() const { return spinOffset_; }
    /// @brief 设置自旋初相角 [rad]
    void setSpinOffset(double offset) { spinOffset_ = offset; }

    /// @brief 获取初相角对应的历元
    const TimePoint& getEpoch() const { return epoch_; }
    /// @brief 设置初相角对应的历元
    void setEpoch(const TimePoint& epoch) { epoch_ = epoch; }

protected:
    Frame* defaultFrame() const override;

    /// @brief 计算 t 时刻的自旋角(初相角 + 角速度 * 时间差) [rad]
    double spinAngle(const TimePoint& tp) const;

protected:
    Vector3d  spinAxisInFrame_{Vector3d::UnitZ()};   ///< 参考坐标系下的自旋轴
    Vector3d  spinAxisInBody_{Vector3d::UnitZ()};    ///< 体系下的自旋轴
    double    spinRate_{0.0};                        ///< 自旋角速度 [rad/s]
    double    spinOffset_{0.0};                      ///< 自旋初相角 [rad]
    TimePoint epoch_{};                              ///< 初相角对应的历元
};

/*! @} */

AST_NAMESPACE_END
