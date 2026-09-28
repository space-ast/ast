///
/// @file      AttitudeUtil.hpp
/// @brief     姿态计算工具函数
/// @details   由向量对计算姿态与角速度的通用工具
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
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/


/// @brief     双矢量定姿（Aligned and Constrained）——只求姿态
/// @details   两对向量确定一个姿态，对应 STK 的 Aligned and Constrained 姿态类型。
///            第一对为「对齐」对：本体系固定向量 axesVector1 与参考向量 refVector1 严格重合；
///            第二对为「约束」对：本体系固定向量 axesVector2 只在垂直于 axesVector1 的方向上
///            被约束到参考向量 refVector2，即 axesVector2 落在 refVector1 与 refVector2 张成的平面内
///            （两者正交时才与 refVector2 完全重合）。
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
/// @param     rotation    输出：参考系到本体系的运动学旋转，getRotationRate() 为本体系相对参考系的
///                        角速度，分量在参考系下
/// @return    错误码；出现退化（零向量，或两向量平行）时返回 eErrorInvalidParam，并将 rotation 置为单位旋转
/// @note      角速度只由参考向量对及其变化率决定，与本体系的固定向量 axesVector1、axesVector2 无关
AST_MATH_API errc_t aAlignConstrainTransform(
    const Vector3d& axesVector1, const Vector3d& refVector1, const Vector3d& refRate1,
    const Vector3d& axesVector2, const Vector3d& refVector2, const Vector3d& refRate2,
    KinematicRotation& rotation);


/*! @} */

AST_NAMESPACE_END

