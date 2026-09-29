///
/// @file      LocalOrbitFrame.cpp
/// @brief     局部轨道坐标系
/// @details   局部轨道坐标系是一种基于位置或速度向量的坐标系
/// @author    axel
/// @date      2026-01-13
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

#include "LocalOrbitFrame.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstMath/KinematicTransform.hpp"

AST_NAMESPACE_BEGIN

errc_t aFrameToVVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    Vector3d axis_y = velInFrame.cross(posInFrame);

	// pos为0，vel为0，或者pos和vel方向相同
	if (A_UNLIKELY(axis_y.normalize() == 0))
	{
		matrix.setIdentity();
        return eErrorInvalidParam;
	}
	else
	{
		Vector3d axis_x = posInFrame.cross(axis_y).normalized();
        Vector3d axis_z = -posInFrame.normalized();
		matrix = {
            axis_x[0], axis_x[1], axis_x[2],
            axis_y[0], axis_y[1], axis_y[2],
            axis_z[0], axis_z[1], axis_z[2],
        };
		return eNoError;
	}
}

errc_t aVVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    errc_t rc = aFrameToVVLHMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}

errc_t aFrameToLVLHMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    Vector3d axis_z = posInFrame.cross(velInFrame);
    if(A_UNLIKELY(axis_z.normalize() == 0))
    {
        matrix.setIdentity();
        return eErrorInvalidParam;
    }
    else
    {
        Vector3d axis_y = axis_z.cross(posInFrame).normalized();
        Vector3d axis_x = posInFrame.normalized();
        matrix = {
            axis_x[0], axis_x[1], axis_x[2],
            axis_y[0], axis_y[1], axis_y[2],
            axis_z[0], axis_z[1], axis_z[2],
        };
        return eNoError;
    }
}

errc_t aFrameToLVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, Rotation& rotation)
{
    return aFrameToLVLHMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

errc_t aFrameToLVLHTransform(const Vector3d& posInFrame, const Vector3d& velInFrame, KinematicRotation& rotation)
{
    errc_t rc = aFrameToLVLHMatrix(posInFrame, velInFrame, rotation.getRotation().getMatrix());
    if (A_UNLIKELY(rc != eNoError))
    {
        rotation.setRotationRate(Vector3d::Zero());
        return rc;
    }

    /*!
        LVLH 系的角速度沿轨道法向，大小为 |r×v|/|r|²，单位向量为 (r×v)/|r×v|，角速度可写作 ω = (r×v)/|r|²
        分量在输入系下分解，符合 KinematicRotation 的约定。
    */
    rotation.setRotationRate(posInFrame.cross(velInFrame) * (1.0 / posInFrame.squaredNorm()));
    return eNoError;
}

errc_t aLVLHToFrameMatrix(const Vector3d& posInFrame, const Vector3d& velInFrame, Matrix3d& matrix)
{
    errc_t rc = aFrameToLVLHMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}


errc_t aFrameToVNCMatrix(const Vector3d &posInFrame, const Vector3d &velInFrame, Matrix3d &matrix)
{
    Vector3d axis_y = posInFrame.cross(velInFrame);
    if(A_UNLIKELY(axis_y.normalize() == 0))
    {
        matrix.setIdentity();
        return eErrorInvalidParam;
    }else{
        Vector3d axis_z = velInFrame.cross(axis_y).normalized();
        Vector3d axis_x = velInFrame.normalized();
        matrix = {
            axis_x[0], axis_x[1], axis_x[2],
            axis_y[0], axis_y[1], axis_y[2],
            axis_z[0], axis_z[1], axis_z[2],
        };
        return eNoError;
    }
}

errc_t aFrameToVNCTransform(const Vector3d &posInFrame, const Vector3d &velInFrame, Rotation &rotation)
{
    return aFrameToVNCMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

errc_t aVNCToFrameMatrix(const Vector3d &posInFrame, const Vector3d &velInFrame, Matrix3d &matrix)
{
    errc_t rc = aFrameToVNCMatrix(posInFrame, velInFrame, matrix);
    matrix.transposeInPlace();
    return rc;
}

errc_t aVNCToFrameTransform(const Vector3d &posInFrame, const Vector3d &velInFrame, Rotation &rotation)
{
    return aVNCToFrameMatrix(posInFrame, velInFrame, rotation.getMatrix());
}

AST_NAMESPACE_END

