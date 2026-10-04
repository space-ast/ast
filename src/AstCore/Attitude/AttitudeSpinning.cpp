///
/// @file      AttitudeSpinning.cpp
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

#include "AttitudeSpinning.hpp"
#include "AstCore/BuiltinFrame.hpp"
#include "AstMath/AccelerationRotation.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstMath/Euler.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/Vector.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

/// @brief 构造自旋轴取向：321(yaw-pitch-roll)序列在 yaw = 0 时得到的姿态，其 Z 轴沿 v
/// @param v 自旋轴方向(零向量除外，无需预先归一化)
static Matrix3d aSpinAxisOrientation(const Vector3d& v)
{
    const Vector3d u = v.normalized();
    const double ux = u.x() > 1.0 ? 1.0 : (u.x() < -1.0 ? -1.0 : u.x());
    const double pitch = std::asin(ux);
    const double roll = std::atan2(-u.y(), u.z());

    Matrix3d frame;
    aEuler123ToMatrix({roll, pitch, 0.0}, frame);
    return frame;
}

/// @brief 计算把参考系方向 a 映到体系方向 b 的基准取向 M0
/// @param a 参考坐标系下的自旋轴方向
/// @param b 体系下的自旋轴方向
/// @param rotation 输出参数，参考系到体系的转换矩阵
/// @return 错误码
static errc_t aSpinBaseRotation(const Vector3d& a, const Vector3d& b, Rotation& rotation)
{
    if (a.norm() == 0.0 || b.norm() == 0.0)
        return eErrorInvalidParam;

    const Matrix3d frameA = aSpinAxisOrientation(a);   // 标准 A 的转置
    const Matrix3d frameB = aSpinAxisOrientation(b);   // 标准 B 的转置
    rotation = Rotation::FromMatrix(frameB.transpose() * frameA);
    return eNoError;
}

double AttitudeSpinning::spinAngle(const TimePoint& tp) const
{
    return spinOffset_ + spinRate_ * (tp - epoch_);
}

Axes* AttitudeSpinning::getParent() const
{
    return referenceAxes_.get();
}

errc_t AttitudeSpinning::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    if (spinAxisInFrame_.norm() == 0.0 || spinAxisInBody_.norm() == 0.0)
        return eErrorInvalidParam;

    Rotation base;
    errc_t rc = aSpinBaseRotation(spinAxisInFrame_, spinAxisInBody_, base);
    if (rc != eNoError)
        return rc;

    // 体轴系绕参考系下的自旋轴右手转过自旋角：姿态为 M(t) = M0 * C(theta, a)
    const Rotation spin(spinAngle(tp), spinAxisInFrame_.normalized());
    rotation = Rotation::FromMatrix(base.getMatrix() * spin.getMatrix());
    return eNoError;
}

errc_t AttitudeSpinning::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    errc_t rc = getTransform(tp, rotation.rotation());
    if (rc != eNoError)
        return rc;
    rotation.setRotationRate(spinAxisInFrame_.normalized() * spinRate_);
    return eNoError;
}

errc_t AttitudeSpinning::getTransform(const TimePoint &tp, AccelerationRotation &rotation) const
{
    errc_t rc = getTransform(tp, rotation.kinematicRotation());
    if (rc != eNoError)
        return rc;
    rotation.setRotationRateDot(Vector3d::Zero());
    return eNoError;
}



AST_NAMESPACE_END
