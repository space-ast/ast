///
/// @file      AttitudeUtil.cpp
/// @brief     姿态计算工具函数
/// @details   
/// @author    axel
/// @date      2026-09-28
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

#include "AttitudeUtil.hpp"

AST_NAMESPACE_BEGIN

namespace {

/// @brief 将三轴置为单位三轴
void setIdentityTriad(Vector3d axis[3])
{
    axis[0] = Vector3d::UnitX();
    axis[1] = Vector3d::UnitY();
    axis[2] = Vector3d::UnitZ();
}

/// @brief 由一对向量构造正交三轴（Gram-Schmidt 正交化）
/// @details 第一轴沿 v1，第二轴取 v2 中垂直于第一轴的分量，第三轴由右手法则得到
/// @param v1   第一轴方向向量
/// @param v2   第二轴方向向量，只有其垂直于 v1 的分量被使用
/// @param axis 输出的三个正交单位轴，出现退化时输出单位三轴
/// @return v1 为零向量或 v2 与 v1 平行时返回错误码
errc_t makeTriad(const Vector3d& v1, const Vector3d& v2, Vector3d axis[3])
{
    axis[0] = v1;
    if (A_UNLIKELY(axis[0].normalize() == 0))
    {
        setIdentityTriad(axis);
        return eErrorInvalidParam;
    }

    axis[1] = v2 - axis[0] * v2.dot(axis[0]);
    if (A_UNLIKELY(axis[1].normalize() == 0))
    {
        setIdentityTriad(axis);
        return eErrorInvalidParam;
    }

    axis[2] = axis[0].cross(axis[1]);
    return eNoError;
}

/// @brief 由一对向量及其时间变化率构造正交三轴及其时间变化率
/// @param v1    第一轴方向向量
/// @param vdot1 v1 的时间变化率
/// @param v2    第二轴方向向量，只有其垂直于 v1 的分量被使用
/// @param vdot2 v2 的时间变化率
/// @param axis  输出的三个正交单位轴，出现退化时输出单位三轴
/// @param rate  输出的三个轴的时间变化率，出现退化时输出零向量
/// @return v1 为零向量或 v2 与 v1 平行时返回错误码
errc_t makeTriadRate(const Vector3d& v1, const Vector3d& vdot1,
                     const Vector3d& v2, const Vector3d& vdot2,
                     Vector3d axis[3], Vector3d rate[3])
{
    errc_t rc = makeTriad(v1, v2, axis);
    if (A_UNLIKELY(rc != eNoError))
    {
        rate[0] = Vector3d::Zero();
        rate[1] = Vector3d::Zero();
        rate[2] = Vector3d::Zero();
        return rc;
    }

    const Vector3d& f1 = axis[0];
    const Vector3d& f2 = axis[1];

    // f1 = v1 / |v1|，故 df1 = (dv1 - (dv1·f1) f1) / |v1|
    Vector3d fdot1 = (vdot1 - f1 * vdot1.dot(f1)) / v1.norm();

    // k = v2 - (v2·f1) f1 为 v2 中垂直于 f1 的分量，除以 |k| 得到 f2
    // 对 k 求导
    double normK = (v2 - f1 * v2.dot(f1)).norm();
    Vector3d kdot = vdot2 - f1 * vdot2.dot(f1) - fdot1 * v2.dot(f1) - f1 * v2.dot(fdot1);

    // df2 = (dk - (dk·f2) f2) / |k|
    Vector3d fdot2 = (kdot - f2 * kdot.dot(f2)) / normK;

    rate[0] = fdot1;
    rate[1] = fdot2;
    rate[2] = fdot1.cross(f2) + f1.cross(fdot2);
    return eNoError;
}

/// @brief 由本体系三轴与参考系三轴填充旋转矩阵
/// @details 记本体系三轴为 e、参考系三轴为 f。本体系 -> 参考系的旋转为 R = F E^T，
///          其中 E、F 分别以 e、f 为列，故参考系 -> 本体系的旋转为 M = R^T = E F^T，
///          元素形式为 M(i,j) = sum_k e_k[i] * f_k[j]。
///          注意 M(i,j) 不是 e_i 与 f_j 的点积，两者互为转置。
/// @param axesAxis 本体系三轴
/// @param refAxis  参考系三轴
/// @param rotation 输出的旋转
void fillRotation(const Vector3d axesAxis[3], const Vector3d refAxis[3], Rotation& rotation)
{
    Matrix3d& matrix = rotation.getMatrix();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            matrix(i, j) = axesAxis[0][i] * refAxis[0][j]
                         + axesAxis[1][i] * refAxis[1][j]
                         + axesAxis[2][i] * refAxis[2][j];
        }
    }
}

/// @brief 由参考系三轴及其时间变化率求本体系相对参考系的角速度
/// @details 角速度的参考系分量为 ω = (df2·f3) f1 + (df3·f1) f2 + (df1·f2) f3
/// @param refAxis 参考系三轴
/// @param refRate 参考系三轴的时间变化率
/// @return 角速度，参考系分量
Vector3d triadRateToAngularVelocity(const Vector3d refAxis[3], const Vector3d refRate[3])
{
    return refAxis[0] * refRate[1].dot(refAxis[2])
         + refAxis[1] * refRate[2].dot(refAxis[0])
         + refAxis[2] * refRate[0].dot(refAxis[1]);
}

} // namespace

errc_t aAlignConstrainTransform(
    const Vector3d& axesVector1, const Vector3d& refVector1,
    const Vector3d& axesVector2, const Vector3d& refVector2,
    Rotation& rotation)
{
    Vector3d axesAxis[3];
    Vector3d refAxis[3];
    errc_t rcRef = makeTriad(refVector1, refVector2, refAxis);
    errc_t rcAxes = makeTriad(axesVector1, axesVector2, axesAxis);
    if (A_UNLIKELY(rcRef != eNoError || rcAxes != eNoError))
    {
        rotation = Rotation::Identity();
        return (rcRef != eNoError) ? rcRef : rcAxes;
    }
    fillRotation(axesAxis, refAxis, rotation);
    return eNoError;
}

errc_t aAlignConstrainTransform(
    const Vector3d& axesVector1, const Vector3d& refVector1, const Vector3d& refRate1,
    const Vector3d& axesVector2, const Vector3d& refVector2, const Vector3d& refRate2,
    KinematicRotation& rotation)
{
    Vector3d axesAxis[3];
    Vector3d refAxis[3];
    Vector3d refRate[3];
    errc_t rcRef = makeTriadRate(refVector1, refRate1, refVector2, refRate2, refAxis, refRate);
    errc_t rcAxes = makeTriad(axesVector1, axesVector2, axesAxis);
    if (A_UNLIKELY(rcRef != eNoError || rcAxes != eNoError))
    {
        rotation.setRotation(Rotation::Identity());
        rotation.setRotationRate(Vector3d::Zero());
        return (rcRef != eNoError) ? rcRef : rcAxes;
    }
    fillRotation(axesAxis, refAxis, rotation.getRotation());
    rotation.setRotationRate(triadRateToAngularVelocity(refAxis, refRate));
    return eNoError;
}

AST_NAMESPACE_END
