///
/// @file      AttitudeAlignConstrain.cpp
/// @brief     对齐/约束姿态(Aligned and Constrained)
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

#include "AttitudeAlignConstrain.hpp"
#include "AstMath/AttitudeConvert.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

/// @brief 判定"约束方向是否几乎没有垂直于对齐方向的分量"的相对阈值
/// @details 取单位向量叉乘的模(即夹角正弦)，而不是绝对分量，
///          这样判据与向量的物理量级无关(位置、速度的量级可能差好几个数量级)。
static constexpr double kAttitudeConstrainSinEps = 1e-8;

Vector3d aAttitudeAxisVector(EAttitudeAxis axis)
{
    switch (axis)
    {
    case EAttitudeAxis::eX: return Vector3d::UnitX();
    case EAttitudeAxis::eY: return Vector3d::UnitY();
    case EAttitudeAxis::eZ: return Vector3d::UnitZ();
    }
    return Vector3d::UnitX();
}

errc_t aAlignConstrainRotation(const Vector3d& alignRef, const Vector3d& constrRef,
                               const Vector3d& alignBody, const Vector3d& constrBody,
                               EAttitudeAxis offsetAxis, EAttitudeOffsetSense offsetSense,
                               double offset, Rotation& rotation)
{
    // 1. 归一化输入向量
    if (A_UNLIKELY(alignRef.norm() == 0.0 || constrRef.norm() == 0.0 ||
                   alignBody.norm() == 0.0 || constrBody.norm() == 0.0))
        return eErrorInvalidParam;

    Vector3d f1 = alignRef.normalized();
    const Vector3d constrRefUnit = constrRef.normalized();
    Vector3d e1 = alignBody.normalized();
    const Vector3d constrBodyUnit = constrBody.normalized();

    // 2. 构造参考系三轴: f1 为对齐方向，f2 取约束方向垂直于 f1 的分量，f3 由右手法则补全
    Vector3d f2 = constrRefUnit - f1 * constrRefUnit.dot(f1);
    if (A_UNLIKELY(f2.norm() < kAttitudeConstrainSinEps))
    {
        // 约束方向与对齐方向平行，无法确定绕对齐轴的转角
        aWarning(_("姿态剖面的约束向量与对齐向量平行，无法定姿"));
        return eErrorInvalidParam;
    }
    f2.normalize();
    const Vector3d f3 = f1.cross(f2);

    // 3. 构造体固连三轴
    Vector3d e2 = constrBodyUnit - e1 * constrBodyUnit.dot(e1);
    if (A_UNLIKELY(e2.norm() < kAttitudeConstrainSinEps))
    {
        aWarning(_("姿态剖面的体固连约束方向与对齐方向平行，无法定姿"));
        return eErrorInvalidParam;
    }
    e2.normalize();
    const Vector3d e3 = e1.cross(e2);

    // 4. 基础旋转: 把 (f1,f2,f3) 映到 (e1,e2,e3)，即 M0 = E * F^T。
    //    E 的列是 e_i、F 的列是 f_i，故 M0(i,j) = e1[i]f1[j] + e2[i]f2[j] + e3[i]f3[j]。
    //    结果的第 i 行即体轴 i 在参考系下的分量，符合 Axes::getTransform 的约定。
    Matrix3d m0;
    const Vector3d e[3] = {e1, e2, e3};
    const Vector3d f[3] = {f1, f2, f3};
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m0(i, j) = e[0][i] * f[0][j] + e[1][i] * f[1][j] + e[2][i] * f[2][j];

    // 5. 施加绕指定体轴的 offset 旋转
    if (offset != 0.0)
    {
        const double theta = (offsetSense == EAttitudeOffsetSense::eLeftHanded) ? -offset : offset;
        Matrix3d offsetMatrix;
        aRotationMatrix(theta, static_cast<int>(offsetAxis), offsetMatrix);

        Matrix3d result;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                for (int k = 0; k < 3; k++)
                    value += offsetMatrix(i, k) * m0(k, j);
                result(i, j) = value;
            }
        rotation = Rotation::FromMatrix(result);
    }
    else
    {
        rotation = Rotation::FromMatrix(m0);
    }
    return eNoError;
}

errc_t AttitudeAlignConstrain::getTransform(const TimePoint& tp, Rotation& rotation) const
{
    Vector3d alignRef, constrRef;
    errc_t rc = getReferenceVector(alignVector_, tp, alignRef);
    if (rc != eNoError)
        return rc;
    rc = getReferenceVector(constraintVector_, tp, constrRef);
    if (rc != eNoError)
        return rc;

    return aAlignConstrainRotation(alignRef, constrRef,
                                   aAttitudeAxisVector(alignAxis_),
                                   aAttitudeAxisVector(constraintAxis_),
                                   offsetAxis_, offsetSense_, azimuth_, rotation);
}

AST_NAMESPACE_END
