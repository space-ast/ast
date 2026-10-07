///
/// @file      AccelerationRotation.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-10-01
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
#include "KinematicRotation.hpp"

AST_NAMESPACE_BEGIN


#define _AST_DEF_ACCELERATIONROTATION_PROPERTIES\
    _AST_DEF_KINEMATICROTATION_PROPERTIES\
    const Vector3d& rotationRateDot() const { return angvelDot_; }\
    Vector3d& rotationRateDot() { return angvelDot_; }\
    const Vector3d& getRotationRateDot() const { return rotationRateDot(); }\
    Vector3d& getRotationRateDot() { return rotationRateDot(); }\
    void setRotationRateDot(const Vector3d& angvelDot) { angvelDot_ = angvelDot; }\
    const AccelerationRotation& accelerationRotation() const { return reinterpret_cast<const AccelerationRotation&>(rotation_); }\
    AccelerationRotation& accelerationRotation() { return reinterpret_cast<AccelerationRotation&>(rotation_); }\
    const AccelerationRotation& getAccelerationRotation() const { return accelerationRotation(); }\
    AccelerationRotation& getAccelerationRotation() { return accelerationRotation(); }\
    void setAccelerationRotation(const AccelerationRotation& rot) { accelerationRotation() = rot; }\


/// @brief     加速度(二阶运动学)坐标系旋转
/// @details   在运动学坐标系旋转的基础上，增加了坐标系旋转的角加速度信息。
///            相比 KinematicRotation 多携带一个角加速度，因此可以变换由位置、速度和
///            加速度组成的二阶状态量，而不只是位置和速度。
/// @note      角速度、角加速度的参考系约定与 KinematicRotation 一致：
///            均表示本坐标系相对源坐标系(父坐标系)的量，且其分量在源坐标系(父坐标系)下分解；
///            角加速度还额外约定为"该分量"的坐标时间导数。
class AccelerationRotation
{
public:

    /// @brief 获取单位加速度旋转
    /// @return 单位加速度旋转
    static AccelerationRotation Identity();

    /// @brief 加速度旋转默认构造函数
    AccelerationRotation() = default;

    /// @brief 加速度旋转构造函数
    /// @param mat 旋转矩阵
    /// @param angvel 旋转角速度
    /// @param angvelDot 旋转角加速度
    AccelerationRotation(const Matrix3d& mat, const Vector3d& angvel, const Vector3d& angvelDot);

    /// @brief 加速度旋转构造函数
    /// @param rot 旋转
    /// @param angvel 旋转角速度
    /// @param angvelDot 旋转角加速度
    AccelerationRotation(const Rotation& rot, const Vector3d& angvel, const Vector3d& angvelDot);

    /// @brief 加速度旋转构造函数
    /// @param rot 运动学旋转
    /// @param angvelDot 旋转角加速度
    AccelerationRotation(const KinematicRotation& rot, const Vector3d& angvelDot);

    _AST_DEF_ACCELERATIONROTATION_PROPERTIES

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    AccelerationRotation& compose(const AccelerationRotation& next);

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    AccelerationRotation composed(const AccelerationRotation& next) const;

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    AccelerationRotation operator*(const AccelerationRotation& next) const;

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    AccelerationRotation& operator*=(const AccelerationRotation& next);

    /// @brief 获取逆旋转
    /// @param inversed 逆旋转
    void getInverse(AccelerationRotation& inversed) const;

    /// @brief 获取逆旋转
    /// @return 逆旋转
    AccelerationRotation inverse() const;

    /// @brief 变换位置、速度和加速度
    /// @details 设 ρ 为位置，ρ̇、ρ̈ 分别为其源坐标系下的坐标导数和坐标二阶导数，则
    ///          positionOut     = M ρ
    ///          velocityOut     = M ( ρ̇ − ω×ρ )
    ///          accelerationOut = M ( ρ̈ − 2ω×ρ̇ − ω̇×ρ + ω×(ω×ρ) )
    /// @param position 位置
    /// @param velocity 速度
    /// @param acceleration 加速度
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    /// @param accelerationOut 变换后的加速度
    void transformVecVelAcc(
        const Vector3d& position,
        const Vector3d& velocity,
        const Vector3d& acceleration,
        Vector3d& positionOut,
        Vector3d& velocityOut,
        Vector3d& accelerationOut) const;

    /// @brief 变换位置、速度和加速度（逆变换）
    /// @warning 注意：这里是逆变换，等价于 `inverse().transformVecVelAcc(...)`;
    /// @param position 位置
    /// @param velocity 速度
    /// @param acceleration 加速度
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    /// @param accelerationOut 变换后的加速度
    void transformVecVelAccInv(
        const Vector3d& position,
        const Vector3d& velocity,
        const Vector3d& acceleration,
        Vector3d& positionOut,
        Vector3d& velocityOut,
        Vector3d& accelerationOut) const;
protected:
    Rotation rotation_{};     ///< 旋转
    Vector3d angvel_{};       ///< 角速度
    Vector3d angvelDot_{};    ///< 角加速度
};

A_ALWAYS_INLINE AccelerationRotation AccelerationRotation::Identity()
{
    return AccelerationRotation(Matrix3d::Identity(), Vector3d::Zero(), Vector3d::Zero());
}

A_ALWAYS_INLINE AccelerationRotation::AccelerationRotation(const Matrix3d &mat, const Vector3d &angvel, const Vector3d &angvelDot)
    : rotation_(mat)
    , angvel_(angvel)
    , angvelDot_(angvelDot)
{
}

A_ALWAYS_INLINE AccelerationRotation::AccelerationRotation(const Rotation &rot, const Vector3d &angvel, const Vector3d &angvelDot)
    : rotation_(rot)
    , angvel_(angvel)
    , angvelDot_(angvelDot)
{
}

A_ALWAYS_INLINE AccelerationRotation::AccelerationRotation(const KinematicRotation &rot, const Vector3d &angvelDot)
    : rotation_(rot.rotation())
    , angvel_(rot.rotationRate())
    , angvelDot_(angvelDot)
{
}

A_ALWAYS_INLINE AccelerationRotation AccelerationRotation::composed(const AccelerationRotation &next) const
{
    /*!
    角速度的合成与 KinematicRotation 一致。
    角加速度在此之上多一项搬运项 ω1 × (M1ᵀ ω2)：
    相对角速度 ω2 的分量是在转动的中间系下分解的，而合成结果要求分量在源系下分解，
    故求时间导数时该项不会消去。
    */
    Matrix3d mat1 = this->matrix();
    Vector3d angvel1 = this->angvel_;
    Vector3d angvel2InSource = next.angvel_ * mat1;      // 即 M1ᵀ ω2
    Vector3d angvel = angvel1 + angvel2InSource;
    Vector3d angvelDot = this->angvelDot_ + angvel1.cross(angvel2InSource) + next.angvelDot_ * mat1;
    return AccelerationRotation(next.matrix() * mat1, angvel, angvelDot);
}

A_ALWAYS_INLINE AccelerationRotation &AccelerationRotation::compose(const AccelerationRotation &next)
{
    *this = this->composed(next);
    return *this;
}

A_ALWAYS_INLINE AccelerationRotation AccelerationRotation::operator*(const AccelerationRotation &next) const
{
    return composed(next);
}

A_ALWAYS_INLINE AccelerationRotation &AccelerationRotation::operator*=(const AccelerationRotation &next)
{
    return compose(next);
}

A_ALWAYS_INLINE void AccelerationRotation::getInverse(AccelerationRotation &inversed) const
{
    // 先取出matrix_，允许inversed与*this为同一对象
    Matrix3d mat = this->matrix();
    Vector3d angvel = this->angvel_;
    Vector3d angvelDot = this->angvelDot_;
    inversed.matrix() = mat.transpose();
    inversed.angvel_ = -(mat * angvel);
    inversed.angvelDot_ = -(mat * angvelDot);   // ω×ω = 0，故角加速度没有附加项
}

A_ALWAYS_INLINE AccelerationRotation AccelerationRotation::inverse() const
{
    AccelerationRotation retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void AccelerationRotation::transformVecVelAcc(
    const Vector3d &position, const Vector3d &velocity, const Vector3d &acceleration,
    Vector3d &positionOut, Vector3d &velocityOut, Vector3d &accelerationOut) const
{
    // 注意：这里要先计算accelerationOut，再计算velocityOut，最后计算positionOut，
    //       防止入参与出参地址相同时值被覆盖
    accelerationOut = this->matrix() * (acceleration
        - this->angvel_.cross(velocity) * 2.0
        - this->angvelDot_.cross(position)
        + this->angvel_.cross(this->angvel_.cross(position)));
    velocityOut = this->matrix() * (velocity - this->angvel_.cross(position));
    positionOut = this->matrix() * position;
}

A_ALWAYS_INLINE void AccelerationRotation::transformVecVelAccInv(
    const Vector3d &position, const Vector3d &velocity, const Vector3d &acceleration,
    Vector3d &positionOut, Vector3d &velocityOut, Vector3d &accelerationOut) const
{
    // 注意：这里要先计算positionOut，再计算velocityOut，最后计算accelerationOut
    positionOut = position * this->matrix();
    velocityOut = velocity * this->matrix() + this->angvel_.cross(positionOut);
    accelerationOut = acceleration * this->matrix()
        + this->angvelDot_.cross(positionOut)
        + this->angvel_.cross(velocityOut) * 2.0
        - this->angvel_.cross(this->angvel_.cross(positionOut));
}

AST_NAMESPACE_END
