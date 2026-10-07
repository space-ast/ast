///
/// @file      AccelerationTransform.hpp
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
#include "KinematicTransform.hpp"
#include "AccelerationRotation.hpp"

AST_NAMESPACE_BEGIN


#define _AST_DEF_ACCELERATIONTRANSFORM_PROPERTIES\
    _AST_DEF_KINEMATICTRANSFORM_PROPERTIES\
    const Vector3d& acceleration() const { return acceleration_; }\
    Vector3d& acceleration() { return acceleration_; }\
    const Vector3d& getAcceleration() const { return acceleration_; }\
    Vector3d& getAcceleration() { return acceleration_; }\
    void setAcceleration(const Vector3d& acceleration) { acceleration_ = acceleration; }\
    const KinematicTransform& kinematicTransform() const { return reinterpret_cast<const KinematicTransform&>(velocity_); }\
    KinematicTransform& kinematicTransform() { return reinterpret_cast<KinematicTransform&>(velocity_); }\
    const KinematicTransform& getKinematicTransform() const { return kinematicTransform(); }\
    KinematicTransform& getKinematicTransform() { return kinematicTransform(); }\
    void setKinematicTransform(const KinematicTransform& transform) { kinematicTransform() = transform; }\

/// @brief     加速度(二阶运动学)坐标系变换
/// @details   在运动学坐标系变换的基础上，增加了坐标系旋转的角加速度和平移的加速度，
///            因此可以变换由位置、速度和加速度组成的二阶状态量，而不只是位置和速度。
/// @note      参考系约定与 KinematicTransform / AccelerationRotation 一致：
///            平移 t、平移速度 V、平移加速度 A 均取源坐标系分量；
///            角速度 ω、角加速度 α 表示本坐标系相对源坐标系(父坐标系)的量，其分量在源坐标系下分解，
///            且 α 为该分量的坐标时间导数。
///            其中 A 约定为平移 t 的坐标二阶导数。
/// @warning    平动加速度为零、旋转角加速度为零的两个变换组合之后，其加速度一般并不为零：
///            A = 2ω₁×(M₁ᵀV₂) + ω₁×(ω₁×τ)，α = ω₁×(M₁ᵀω₂)（τ = M₁ᵀt₂）。
///            即内层坐标系一旦相对转动，外层的匀速平移在源坐标系下就表现为圆周运动。
class AccelerationTransform
{
public:
    /// @brief 加速度变换默认构造函数
    AccelerationTransform() = default;

    /// @brief 加速度变换构造函数
    /// @param translation 平移（源坐标系分量）
    /// @param velocity 平移速度（源坐标系分量）
    /// @param acceleration 平移加速度（源坐标系分量）
    /// @param rot 加速度旋转
    AccelerationTransform(const Vector3d& translation, const Vector3d& velocity,
                          const Vector3d& acceleration, const AccelerationRotation& rot);

    /// @brief 由一阶变换与二阶增量构造加速度变换
    /// @param transform 运动学变换
    /// @param acceleration 平移加速度（源坐标系分量）
    /// @param angvelDot 旋转角加速度
    AccelerationTransform(const KinematicTransform& transform, const Vector3d& acceleration,
                          const Vector3d& angvelDot);

    /// @brief 获取单位加速度变换
    /// @return 单位加速度变换
    static AccelerationTransform Identity();

    _AST_DEF_ACCELERATIONTRANSFORM_PROPERTIES
    _AST_DEF_ACCELERATIONROTATION_PROPERTIES


    /// @brief 组合下一个坐标系变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个坐标系变换
    /// @return 组合变换
    AccelerationTransform& compose(const AccelerationTransform& next);

    /// @brief 组合下一个坐标系变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个坐标系变换
    /// @return 组合变换
    AccelerationTransform composed(const AccelerationTransform& next) const;

    /// @brief 组合下一个坐标系变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个坐标系变换
    /// @return 组合变换
    AccelerationTransform operator*(const AccelerationTransform& next) const;

    /// @brief 组合下一个坐标系变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个坐标系变换
    /// @return 组合变换
    AccelerationTransform& operator*=(const AccelerationTransform& next);

    /// @brief 获取逆变换
    /// @param inversed 逆变换
    void getInverse(AccelerationTransform& inversed) const;

    /// @brief 获取逆变换
    /// @return 逆变换
    AccelerationTransform inverse() const;

    /// @brief 设置为单位变换
    void setIdentity()
    {
        acceleration_ = Vector3d::Zero();
        velocity_ = Vector3d::Zero();
        translation_ = Vector3d::Zero();
        rotation_ = Rotation::Identity();
        angvel_ = Vector3d::Zero();
        angvelDot_ = Vector3d::Zero();
    }

    /// @brief 变换位置、速度和加速度
    /// @details 设 ρ = p − t 为点相对目标坐标系原点的位置，ρ̇、ρ̈ 分别为其在源坐标系下的
    ///          坐标导数和坐标二阶导数，则
    ///          positionOut     = M ρ
    ///          velocityOut     = M ( ρ̇ − ω×ρ )
    ///          accelerationOut = M ( ρ̈ − 2ω×ρ̇ − ω̇×ρ + ω×(ω×ρ) )
    /// @param position 位置（源坐标系分量）
    /// @param velocity 速度（源坐标系分量）
    /// @param acceleration 加速度（源坐标系分量）
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    /// @param accelerationOut 变换后的加速度
    void transformPosVelAcc(
        const Vector3d& position,
        const Vector3d& velocity,
        const Vector3d& acceleration,
        Vector3d& positionOut,
        Vector3d& velocityOut,
        Vector3d& accelerationOut) const;

    /// @brief 变换位置、速度和加速度（逆变换）
    /// @warning 注意：这里是逆变换，等价于 `inverse().transformPosVelAcc(...)`;
    /// @param position 位置（目标坐标系分量）
    /// @param velocity 速度（目标坐标系分量）
    /// @param acceleration 加速度（目标坐标系分量）
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    /// @param accelerationOut 变换后的加速度
    void transformPosVelAccInv(
        const Vector3d& position,
        const Vector3d& velocity,
        const Vector3d& acceleration,
        Vector3d& positionOut,
        Vector3d& velocityOut,
        Vector3d& accelerationOut) const;

protected:
    Vector3d acceleration_{};  ///< 平移加速度
    Vector3d velocity_{};      ///< 速度
    Vector3d translation_{};   ///< 平移
    Rotation rotation_{};      ///< 旋转
    Vector3d angvel_{};        ///< 角速度
    Vector3d angvelDot_{};     ///< 角加速度
};

A_ALWAYS_INLINE AccelerationTransform::AccelerationTransform(const Vector3d &translation, const Vector3d &velocity, const Vector3d &acceleration, const AccelerationRotation &rot)
    : acceleration_(acceleration)
    , velocity_(velocity)
    , translation_(translation)
    , rotation_(rot.rotation())
    , angvel_(rot.rotationRate())
    , angvelDot_(rot.rotationRateDot())
{
}

A_ALWAYS_INLINE AccelerationTransform::AccelerationTransform(const KinematicTransform &transform, const Vector3d &acceleration, const Vector3d &angvelDot)
    : acceleration_(acceleration)
    , velocity_(transform.velocity())
    , translation_(transform.translation())
    , rotation_(transform.rotation())
    , angvel_(transform.rotationRate())
    , angvelDot_(angvelDot)
{
}

A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::Identity()
{
    return AccelerationTransform(Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(), AccelerationRotation::Identity());
}



A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::composed(const AccelerationTransform &next) const
{
    /*!
    记 τ = M₁ᵀ t₂，则平移与速度的合成与 KinematicTransform 一致，
    加速度在此基础上多出三项：τ 与 M₁ᵀV₂ 都随中间系一起转动，其方向随时间变化，故求二阶导时不消去。
        A = A₁ + M₁ᵀA₂ + 2ω₁×(M₁ᵀV₂) + ω₁×(ω₁×τ) + α₁×τ
    */
    // 先取到局部量，再构造返回值，避免原地赋值
    Matrix3d mat1 = this->matrix();
    Vector3d angvel1 = this->rotationRate();
    Vector3d angvelDot1 = this->rotationRateDot();
    Vector3d tau = next.translation() * mat1;             // 即 M₁ᵀ t₂
    Vector3d velocity2 = next.velocity() * mat1;          // 即 M₁ᵀ V₂
    Vector3d acceleration2 = next.acceleration() * mat1;  // 即 M₁ᵀ A₂

    Vector3d translation = this->translation() + tau;
    Vector3d velocity = this->velocity() + velocity2 + angvel1.cross(tau);
    Vector3d acceleration = this->acceleration() + acceleration2
        + angvel1.cross(velocity2) * 2.0
        + angvel1.cross(angvel1.cross(tau))
        + angvelDot1.cross(tau);

    AccelerationRotation rotation = AccelerationRotation(mat1, angvel1, angvelDot1)
        .composed(next.accelerationRotation());

    return AccelerationTransform(translation, velocity, acceleration, rotation);
}

A_ALWAYS_INLINE AccelerationTransform &AccelerationTransform::compose(const AccelerationTransform &next)
{
    *this = this->composed(next);
    return *this;
}

A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::operator*(const AccelerationTransform &next) const
{
    return composed(next);
}

A_ALWAYS_INLINE AccelerationTransform &AccelerationTransform::operator*=(const AccelerationTransform &next)
{
    return compose(next);
}

A_ALWAYS_INLINE void AccelerationTransform::getInverse(AccelerationTransform &inversed) const
{
    /*!
    与 KinematicTransform::getInverse 一致：把平移、速度、加速度取负后走正向变换。
    也可参考 Transform::getInverse 用的 rotation_.transformVector()
    */
    // 先全部取到局部量，允许 inversed 与 *this 为同一对象
    AccelerationRotation rotation = this->accelerationRotation();
    AccelerationRotation inversedRotation;
    rotation.getInverse(inversedRotation);

    rotation.transformVecVelAcc(
        -this->translation(),
        -this->velocity(),
        -this->acceleration(),
        inversed.translation(),
        inversed.velocity(),
        inversed.acceleration_);

    inversed.setAccelerationRotation(inversedRotation);
}

A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::inverse() const
{
    AccelerationTransform retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void AccelerationTransform::transformPosVelAcc(
    const Vector3d &position, const Vector3d &velocity, const Vector3d &acceleration,
    Vector3d &positionOut, Vector3d &velocityOut, Vector3d &accelerationOut) const
{
    /*!
    先进行坐标系平移（含速度和加速度），再进行加速度旋转。
    */
    this->accelerationRotation().transformVecVelAcc(
        position - this->translation(),
        velocity - this->velocity(),
        acceleration - this->acceleration(),
        positionOut,
        velocityOut,
        accelerationOut);
}

A_ALWAYS_INLINE void AccelerationTransform::transformPosVelAccInv(
    const Vector3d &position, const Vector3d &velocity, const Vector3d &acceleration,
    Vector3d &positionOut, Vector3d &velocityOut, Vector3d &accelerationOut) const
{
    /*!
    逆变换是"先逆旋转，再把平移加回去"，与正向的"先减平移，再旋转"次序相反
    */
    this->accelerationRotation().transformVecVelAccInv(
        position,
        velocity,
        acceleration,
        positionOut,
        velocityOut,
        accelerationOut);

    positionOut += this->translation();
    velocityOut += this->velocity();
    accelerationOut += this->acceleration();
}

AST_NAMESPACE_END
