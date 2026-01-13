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
/// 但是你不用关心其内部实现细节，只需要关注并使用其公开接口，注意不要依赖其内部实现。
class Rotation
{
public:
    /// @brief 旋转类默认构造函数
    Rotation() = default;

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
    Rotation(double angle, const Vector3d& axis);


    /// @brief 从轴角构造旋转
    /// @param aa 轴角
    Rotation(const AngleAxis& aa);

    /// @brief 获取旋转矩阵
    /// @return 旋转矩阵
    const Matrix3d& getMatrix() const { return matrix_; }
    Matrix3d& getMatrix() { return matrix_; }

    /// @brief 设置旋转矩阵
    /// @param mat 旋转矩阵
    void setMatrix(const Matrix3d& mat) { matrix_ = mat; }

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

    /// @brief 组合下一个旋转
    /// @warning 
    /// 组合旋转是先应用当前旋转，再应用下一个旋转
    /// 这个函数会修改当前旋转对象。
    /// @param next 下一个旋转
    Rotation& compose(const Rotation& next);

    /// @brief 组合下一个旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个旋转。
    /// @param next 下一个旋转
    /// @return 组合旋转
    Rotation composed(const Rotation& next) const;

    /// @brief 组合下一个旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个旋转。
    /// @param next 下一个旋转
    /// @return 组合旋转
    Rotation operator*(const Rotation& next) const;
    
    /// @brief 组合下一个旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个旋转。
    /// @param next 下一个旋转
    /// @return 组合旋转
    Rotation& operator*=(const Rotation& next);

    /// @brief 获取逆旋转
    /// @param inversed 逆旋转
    void getInverse(Rotation& inversed) const;

    /// @brief 获取逆旋转
    /// @return 逆旋转
    Rotation inverse() const;

    /// @brief 变换向量
    /// @param vector 向量
    /// @param vectorOut 变换后的向量
    void transformVector(const Vector3d& vector, Vector3d& vectorOut) const;

    /// @brief 变换向量
    /// @param vector 向量
    /// @return 变换后的向量
    Vector3d transformVector(const Vector3d& vector) const;

protected:
    Matrix3d matrix_;
};



A_ALWAYS_INLINE Rotation::Rotation(const Quaternion& quat)
{
    aQuatToMatrix(quat, matrix_);
}


A_ALWAYS_INLINE Rotation::Rotation(const Matrix3d& mat)
    : matrix_(mat)
{
}

A_ALWAYS_INLINE Rotation::Rotation(double angle, const Vector3d &axis)
    : Rotation(AngleAxis(angle, axis))
{
}

A_ALWAYS_INLINE Rotation::Rotation(const AngleAxis &aa)
    : matrix_(aa.toRotationMatrix())
{
}

A_ALWAYS_INLINE Quaternion Rotation::getQuaternion() const
{
    return aMatrixToQuat(matrix_);
}

A_ALWAYS_INLINE void Rotation::setQuaternion(const Quaternion& quat)
{
    aQuatToMatrix(quat, matrix_);
}

A_ALWAYS_INLINE Vector3d Rotation::getAxis() const
{
    AngleAxis aa;
    aMatrixToAngleAxis(matrix_, aa);
    return aa.axis();
}

A_ALWAYS_INLINE double Rotation::getAngle() const
{
    AngleAxis aa;
    aMatrixToAngleAxis(matrix_, aa);
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
    aMatrixToAngleAxis(matrix_, aa);
}

A_ALWAYS_INLINE void Rotation::setAngleAxis(const AngleAxis &aa)
{
    matrix_ = aa.toRotationMatrix();
}

A_ALWAYS_INLINE Rotation& Rotation::compose(const Rotation &next)
{
    matrix_ = next.matrix_ * matrix_;
    return *this;
}

A_ALWAYS_INLINE Rotation Rotation::composed(const Rotation &next) const
{
    return Rotation(next.matrix_ * matrix_);
}

A_ALWAYS_INLINE Rotation Rotation::operator*(const Rotation &next) const
{
    return composed(next);
}

A_ALWAYS_INLINE Rotation &Rotation::operator*=(const Rotation &next)
{
    return compose(next);
}

A_ALWAYS_INLINE void Rotation::getInverse(Rotation &inversed) const
{
    inversed.matrix_ = this->matrix_.transpose();
}

A_ALWAYS_INLINE Rotation Rotation::inverse() const
{
    Rotation retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void Rotation::transformVector(const Vector3d &vector, Vector3d &vectorOut) const
{
    vectorOut = this->matrix_ * vector;
}

A_ALWAYS_INLINE Vector3d Rotation::transformVector(const Vector3d& vector) const
{
    Vector3d retval;
    this->transformVector(vector, retval);
    return retval;
}

AST_NAMESPACE_END