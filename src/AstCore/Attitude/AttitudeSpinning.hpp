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

/// @brief 自旋姿态 (Spinning)
/// @details "Spinning"
/// 绕参考坐标系下的一个自旋轴以恒定角速度旋转
/// 自旋轴用两个方向指定：参考坐标系下的方向 a 与体系下的方向 b(通常即某个体轴，例如绕体 Z 轴自旋)
/// 二者的基准取向是两方向各自的自旋轴取向之差 M0 = B * A^T：
/// 把 321(yaw-pitch-roll)序列的 yaw 取 0 即得到 Z 轴沿该方向的姿态
///
/// 正的自旋速率表示绕自旋轴的右手旋转，t 时刻的姿态为
/// M(t) = Rot(spinOffset + spinRate * (t - epoch), a) * M0，
/// 其中 Rot(theta, a) 是绕 a 右手旋转 theta 的参考系到体系转换矩阵。
class AST_CORE_API AttitudeSpinning : public AttitudeProfile
{
public:
    AST_OBJECT(AttitudeSpinning)

    AttitudeSpinning() = default;
    ~AttitudeSpinning() override = default;

    Axes* getParent() const override;
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, AccelerationRotation& rotation) const override;

PROPERTIES:
    /// @brief 获取参考坐标系轴系
    Axes* getReferenceAxes() const { return referenceAxes_.get(); }
    /// @brief 设置参考坐标系轴系
    void setReferenceAxes(Axes* axes) { referenceAxes_ = axes; }

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
    /// @brief 计算 t 时刻的自旋角(初相角 + 角速度 * 时间差) [rad]
    double spinAngle(const TimePoint& tp) const;
protected:
    WeakPtr<Axes> referenceAxes_{};                  ///< 参考坐标系轴系
    Vector3d  spinAxisInFrame_{Vector3d::UnitZ()};   ///< 参考坐标系下的自旋轴
    Vector3d  spinAxisInBody_{Vector3d::UnitZ()};    ///< 体系下的自旋轴
    double    spinRate_{0.0};                        ///< 自旋角速度 [rad/s]
    double    spinOffset_{0.0};                      ///< 自旋初相角 [rad]
    TimePoint epoch_{};                              ///< 初相角对应的历元
};

/*! @} */

AST_NAMESPACE_END
