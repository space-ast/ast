///
/// @file      AttitudeConvertInline.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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
#include "AttitudeConvertProto.hpp"
#include "AstMath/Quaternion.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Euler.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/


/// @brief 矩阵转四元数
/// @param mtx 旋转矩阵
/// @return 四元数
A_ALWAYS_INLINE	Quaternion aMatrixToQuat(const Matrix3d& mtx)
{
	Quaternion q;
	aMatrixToQuat(mtx, q);
	return q;
}


A_ALWAYS_INLINE	Matrix3d aEuler323ToMatrix(const Euler& euler)
{
	Matrix3d mtx;
	aEuler323ToMatrix(euler, mtx);
	return mtx;
}


A_ALWAYS_INLINE	Matrix3d aEuler313ToMatrix(const Euler& euler)
{
	Matrix3d mtx;
	aEuler313ToMatrix(euler, mtx);
	return mtx;
}

A_ALWAYS_INLINE	Matrix3d aRotationZMatrix(double angle)
{
	Matrix3d mtx;
	aRotationZMatrix(angle, mtx);
	return mtx;
}

A_ALWAYS_INLINE	Matrix3d aRotationXMatrix(double angle)
{
	Matrix3d mtx;
	aRotationXMatrix(angle, mtx);
	return mtx;
}


/*! @} */

AST_NAMESPACE_END
