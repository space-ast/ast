///
/// @file      AttitudeUtil.hpp
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

#pragma once

#include "AstGlobal.h"
#include "AstMath/Vector.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/


/// @brief     由两个姿态四元数求平均角速度
/// @details   已知参考系下两个时刻的姿态四元数 q1、q2（均为「参考系 -> 本体系」的旋转），
///            求本体系相对参考系在 dt 内的平均角速度。
///            记增量旋转为 dq，其轴角给出平均旋转轴与转角，转角除以 dt 即平均角速率。
///            dt 足够小时，结果收敛于瞬时角速度。
/// @param     q1    起始时刻的姿态四元数（参考系 -> 本体系），假设已归一化
/// @param     q2    结束时刻的姿态四元数（参考系 -> 本体系），假设已归一化
/// @param     dt    时间间隔，单位 s
/// @param     angvel 输出：平均角速度，分量为参考系(父系)分量，
///                   与 KinematicRotation::getRotationRate 的约定一致
/// @note      只有两个采样点时无法分辨转角与其补角，转角一律取小于 180 度的短弧，
///            故单步转角接近或超过 180 度时结果不唯一（符号会反转）。
/// @note      需要本体系分量时，对结果再做一次逆变换：`Rotation(q1).transformVector(angvel)`。
AST_MATH_API void aQuatAverageAngularVelocity(const Quaternion& q1, const Quaternion& q2, double dt, Vector3d& angvel);


/// @brief     由两个姿态四元数求平均角速度
/// @param     q1    起始时刻的姿态四元数（参考系 -> 本体系），假设已归一化
/// @param     q2    结束时刻的姿态四元数（参考系 -> 本体系），假设已归一化
/// @param     dt    时间间隔，单位 s
/// @see       aQuatAverageAngularVelocity(const Quaternion&, const Quaternion&, double, Vector3d&)
A_ALWAYS_INLINE Vector3d aQuatAverageAngularVelocity(const Quaternion& q1, const Quaternion& q2, double dt)
{
    Vector3d angvel;
    aQuatAverageAngularVelocity(q1, q2, dt, angvel);
    return angvel;
}


/// @brief     双矢量定姿（Aligned and Constrained）——只求姿态
/// @details   两对向量确定一个姿态，即 Aligned and Constrained 姿态类型。
///            第一对为「对齐」对：本体系固定向量 axesVector1 与参考向量 refVector1 严格重合；
///            第二对为「约束」对：本体系固定向量 axesVector2 在垂直于 axesVector1 的方向上被约束到参考向量 refVector2，
///                              即 axesVector2 落在 refVector1 与 refVector2 张成的平面内
/// @param     axesVector1 对齐向量，分量在本体系(待求轴系)下
/// @param     refVector1  对齐参考向量，分量在参考系下
/// @param     axesVector2 约束向量，分量在本体系(待求轴系)下
/// @param     refVector2  约束参考向量，分量在参考系下
/// @param     rotation    输出：参考系分量到本体系分量的旋转，与 Axes::getTransform 的约定一致
/// @return    错误码；出现退化（零向量，或两向量平行）时返回 eErrorInvalidParam，并将 rotation 置为单位旋转
/// @see       aAlignConstrainTransform(const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&, const Vector3d&, KinematicRotation&)
AST_MATH_API errc_t aAlignConstrainTransform(
    const Vector3d& axesVector1, const Vector3d& refVector1,
    const Vector3d& axesVector2, const Vector3d& refVector2,
    Rotation& rotation);


/// @brief     双矢量定姿（Aligned and Constrained）——姿态与角速度
/// @details   与上方的重载相同，区别在于需要额外提供两个参考向量的时间变化率，
///            由参考向量对的三轴正交化过程解析地求出姿态角速度。
/// @param     axesVector1 对齐向量，分量在本体系(待求轴系)下
/// @param     refVector1  对齐参考向量，分量在参考系下
/// @param     refRate1    对齐参考向量的时间变化率，分量在参考系下
/// @param     axesVector2 约束向量，分量在本体系(待求轴系)下
/// @param     refVector2  约束参考向量，分量在参考系下
/// @param     refRate2    约束参考向量的时间变化率，分量在参考系下
/// @param     rotation    输出：参考系到本体系的运动学旋转 @see KinematicRotation
/// @return    错误码；出现退化（零向量，或两向量平行）时返回 eErrorInvalidParam，并将 rotation 置为单位旋转
/// @note      角速度只由参考向量对及其变化率决定，与本体系的固定向量 axesVector1、axesVector2 无关
AST_MATH_API errc_t aAlignConstrainTransform(
    const Vector3d& axesVector1, const Vector3d& refVector1, const Vector3d& refRate1,
    const Vector3d& axesVector2, const Vector3d& refVector2, const Vector3d& refRate2,
    KinematicRotation& rotation);


/*! @} */

AST_NAMESPACE_END

