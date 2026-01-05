///
/// @file      Rotation.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-08
/// @copyright 版权所有 (C) 2025-present, ast项目.
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
#include "AstMath/Quaternion.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Vector.hpp"
#include "AstMath/AngleAxis.hpp"
#include "AstMath/AttitudeConvert.hpp"

AST_NAMESPACE_BEGIN


/// @brief 旋转类
/// @details 旋转类表示三维空间的旋转。
/// 旋转类当前是基于旋转矩阵实现的，未来也可能会切换到四元数进行实现。
/// 但是你不用关心其内部实现细节，只需要关注其相关的接口。
class Rotation
{
public:
    /// @brief 旋转类默认构造函数
    Rotation();

    /// @brief 复制构造函数
    /// @param other 旋转对象
    Rotation(const Rotation& other) = default;

    /// @brief 构造函数
    /// @param quat 四元数
    Rotation(const Quaternion& quat);

    /// @brief 从旋转矩阵构造旋转
    /// @param mat 旋转矩阵
    Rotation(const Matrix3d& mat);

    /// @brief 从轴角构造旋转
    /// @param axis 旋转轴
    /// @param angle 旋转角度（弧度）
    Rotation(const Vector3d& axis, double angle);


    /// @brief 从轴角构造旋转
    /// @param aa 轴角
    Rotation(const AngleAxis& aa);

    /// @brief 获取旋转矩阵
    /// @return 旋转矩阵
    const Matrix3d& getMatrix() const { return mat_; }

    /// @brief 设置旋转矩阵
    /// @param mat 旋转矩阵
    void setMatrix(const Matrix3d& mat) { mat_ = mat; }

    /// @brief 获取四元数
    /// @return 四元数
    Quaternion getQuaternion() const;

    /// @brief 设置四元数
    /// @param quat 四元数
    void setQuaternion(const Quaternion& quat);

    /// @brief 获取旋转轴
    /// @return 旋转轴
    Vector3d getAxis() const;

    /// @brief 获取旋转角度（弧度）
    /// @return 旋转角度（弧度）
    double getAngle() const;


    /// @brief 获取旋转轴和角度（弧度）
    /// @param axis 旋转轴
    /// @param angle 旋转角度（弧度）
    void getAngleAxis(double& angle, Vector3d& axis) const;

    
    /// @brief 设置轴角
    /// @param angle 旋转角度（弧度）
    /// @param axis 旋转轴
    void setAngleAxis(double angle, const Vector3d& axis);

    /// @brief 获取轴角
    /// @param aa 轴角
    void getAngleAxis(AngleAxis& aa) const;

    /// @brief 设置轴角
    /// @param aa 轴角
    void setAngleAxis(const AngleAxis& aa);

    /// @brief 组合旋转
    /// @param other 其他旋转
    void compose(const Rotation& other);

    /// @brief 组合旋转
    /// @param other 其他旋转
    /// @return 组合旋转
    Rotation composed(const Rotation& other) const;


protected:
    Matrix3d mat_;
};


A_ALWAYS_INLINE Rotation::Rotation()
    : mat_(Matrix3d::Identity())
{
}

A_ALWAYS_INLINE Rotation::Rotation(const Quaternion& quat)
{
    aQuatToMatrix(quat, mat_);
}


A_ALWAYS_INLINE Rotation::Rotation(const Matrix3d& mat)
    : mat_(mat)
{
}

A_ALWAYS_INLINE Rotation::Rotation(const Vector3d &axis, double angle)
    : Rotation(AngleAxis(angle, axis))
{
}

A_ALWAYS_INLINE Rotation::Rotation(const AngleAxis &aa)
    : mat_(aa.toRotationMatrix())
{
}

A_ALWAYS_INLINE Quaternion Rotation::getQuaternion() const
{
    return aMatrixToQuat(mat_);
}

A_ALWAYS_INLINE void Rotation::setQuaternion(const Quaternion& quat)
{
    aQuatToMatrix(quat, mat_);
}

A_ALWAYS_INLINE Vector3d Rotation::getAxis() const
{
    AngleAxis aa;
    aMatrixToAngleAxis(mat_, aa);
    return aa.axis();
}

A_ALWAYS_INLINE double Rotation::getAngle() const
{
    AngleAxis aa;
    aMatrixToAngleAxis(mat_, aa);
    return aa.angle();
}

A_ALWAYS_INLINE void Rotation::getAngleAxis(double &angle, Vector3d &axis) const
{
    AngleAxis aa;
    getAngleAxis(aa);
    axis = aa.axis();
    angle = aa.angle();
}

A_ALWAYS_INLINE void Rotation::setAngleAxis(double angle, const Vector3d &axis)
{
    setAngleAxis(AngleAxis(angle, axis));
}

A_ALWAYS_INLINE void Rotation::getAngleAxis(AngleAxis &aa) const
{
    aMatrixToAngleAxis(mat_, aa);
}

A_ALWAYS_INLINE void Rotation::setAngleAxis(const AngleAxis &aa)
{
    mat_ = aa.toRotationMatrix();
}

AST_NAMESPACE_END