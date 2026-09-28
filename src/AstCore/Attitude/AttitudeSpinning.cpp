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
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/Rotation.hpp"
#include <cmath>

AST_NAMESPACE_BEGIN

/// @brief 判定两向量是否近似平行的相对阈值(单位向量叉乘的模)
static constexpr double kSpinAxisParallelEps = 1e-12;

/// @brief 计算把参考系方向 a 映到体系方向 b 的基准取向 M0
/// @details 取最短弧旋转：绕 a x b 转 a 与 b 之间的夹角。两方向平行时退化为单位旋转
///          (同向)或绕垂直于 a 的任意轴转 180 度(反向)。
/// @param a 参考坐标系下的自旋轴方向
/// @param b 体系下的自旋轴方向
/// @param rotation 输出参数，参考系到体系的转换矩阵
/// @return 错误码
static errc_t aSpinBaseRotation(const Vector3d& a, const Vector3d& b, Rotation& rotation)
{
    if (a.norm() == 0.0 || b.norm() == 0.0)
        return eErrorInvalidParam;

    const Vector3d ua = a.normalized();
    const Vector3d ub = b.normalized();

    Vector3d cross = ua.cross(ub);
    const double sinAngle = cross.norm();
    const double cosAngle = ua.dot(ub);

    if (sinAngle < kSpinAxisParallelEps)
    {
        if (cosAngle > 0.0)
        {
            rotation = Rotation::Identity();
        }
        else
        {
            // 反向：绕垂直于 a 的任意轴转 180 度
            Vector3d axis = ua.cross(Vector3d::UnitX());
            if (axis.norm() < kSpinAxisParallelEps)
                axis = ua.cross(Vector3d::UnitY());
            axis.normalize();
            rotation = Rotation(kPI, axis);
        }
        return eNoError;
    }

    cross.normalize();
    const double angle = std::atan2(sinAngle, cosAngle);

    // 需要的是"把 a 映到 b"的转换矩阵。绕 cross = a x b 转 angle 的右手旋转 R 满足 R*a = b，
    // 而本工程的 Rotation(theta, axis) 是 R 的转置(行是转过的轴分量)，故角度取负。
    rotation = Rotation(-angle, cross);
    return eNoError;
}

double AttitudeSpinning::spinAngle(const TimePoint& tp) const
{
    return spinOffset_ + spinRate_ * (tp - epoch_);
}

errc_t AttitudeSpinning::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    if (spinAxisInFrame_.norm() == 0.0 || spinAxisInBody_.norm() == 0.0)
        return eErrorInvalidParam;

    Rotation base;
    errc_t rc = aSpinBaseRotation(spinAxisInFrame_, spinAxisInBody_, base);
    if (rc != eNoError)
        return rc;

    // 体轴系绕参考系下的自旋轴右手转过自旋角：姿态为 M(t) = M0 * C(theta, a)。
    // 注意是右乘——C(theta, a) 作用在体系一侧。基准取向等于单位阵时左右乘恰好相同，
    // 因此只有自旋轴倾斜时才能区分，测试覆盖了这一点。
    const Rotation spin(spinAngle(tp), spinAxisInFrame_.normalized());
    rotation = Rotation::FromMatrix(base.getMatrix() * spin.getMatrix());
    return eNoError;
}

errc_t AttitudeSpinning::getTransform(const TimePoint& tp, KinematicRotation& rotation) const
{
    if (spinAxisInFrame_.norm() == 0.0)
        return eErrorInvalidParam;

    Rotation rot;
    errc_t rc = getTransform(tp, rot);
    if (rc != eNoError)
        return rc;

    rotation.setRotation(rot);
    rotation.setRotationRate(spinAxisInFrame_.normalized() * spinRate_);
    return eNoError;
}

Frame* AttitudeSpinning::defaultFrame() const
{
    return aFrameECI();
}

AST_NAMESPACE_END
