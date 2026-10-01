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
class AccelerationTransform : protected KinematicTransform
{
public:
    using KinematicTransform::getTranslation;
    using KinematicTransform::setTranslation;
    using KinematicTransform::getVelocity;
    using KinematicTransform::setVelocity;
    using KinematicTransform::getRotation;
    using KinematicTransform::getRotationRate;
    using KinematicTransform::getKinematicRotation;
    using KinematicTransform::setKinematicRotation;
    using KinematicTransform::getTransform;
    using KinematicTransform::setTransform;
    using KinematicTransform::transformPosition;
    using KinematicTransform::transformPositionVelocity;

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

    /// @brief 获取旋转角加速度
    /// @return 旋转角加速度
    const Vector3d& getRotationRateDot() const { return angvelDot_; }
    const Vector3d& rotationRateDot() const { return angvelDot_; }

    /// @brief 设置坐标系旋转角速度
    /// @note  KinematicTransform 没有提供该接口（只能用 getRotationRate() 的非 const 重载改），
    ///        这里补上，与 AccelerationRotation::setRotationRate 对齐。
    /// @param angvel 旋转角速度
    void setRotationRate(const Vector3d& angvel) { angvel_ = angvel; }

    /// @brief 设置旋转角加速度
    /// @param angvelDot 旋转角加速度
    void setRotationRateDot(const Vector3d& angvelDot) { angvelDot_ = angvelDot; }

    /// @brief 获取平移加速度
    /// @return 平移加速度
    const Vector3d& getAcceleration() const { return acceleration_; }

    /// @brief 设置平移加速度
    /// @param acc 平移加速度
    void setAcceleration(const Vector3d& acc) { acceleration_ = acc; }

    /// @brief 降阶取出其中的一阶部分
    /// @details 丢弃平移加速度与角加速度，得到一个运动学变换，可用于复用 KinematicTransform 的接口。
    /// @return 运动学变换
    const KinematicTransform& getKinematicTransform() const { return *this; }

    /// @brief 组装出其中的加速度旋转（由旋转、角速度、角加速度构成）
    /// @note  因内存布局所限（详见 AccelerationTransform.cpp 的布局约束），这里只能按值返回，
    ///        不能像 KinematicTransform::getKinematicRotation() 那样返回引用。
    /// @return 加速度旋转
    AccelerationRotation getAccelerationRotation() const;

    /// @brief 组装出其中的加速度旋转
    /// @param rot 加速度旋转
    void getAccelerationRotation(AccelerationRotation& rot) const;

    /// @brief 设置其中的加速度旋转
    /// @param rot 加速度旋转
    void setAccelerationRotation(const AccelerationRotation& rot);

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
        KinematicTransform::setIdentity();
        angvelDot_ = Vector3d::Zero();
        acceleration_ = Vector3d::Zero();
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
    Vector3d angvelDot_{};     ///< 角加速度
    Vector3d acceleration_{};  ///< 平移加速度
};

A_ALWAYS_INLINE AccelerationTransform::AccelerationTransform(const Vector3d &translation, const Vector3d &velocity, const Vector3d &acceleration, const AccelerationRotation &rot)
    : KinematicTransform(translation, velocity, rot.getKinematicRotation())
    , angvelDot_(rot.getRotationRateDot())
    , acceleration_(acceleration)
{
}

A_ALWAYS_INLINE AccelerationTransform::AccelerationTransform(const KinematicTransform &transform, const Vector3d &acceleration, const Vector3d &angvelDot)
    : KinematicTransform(transform)
    , angvelDot_(angvelDot)
    , acceleration_(acceleration)
{
}

A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::Identity()
{
    return AccelerationTransform(Vector3d::Zero(), Vector3d::Zero(), Vector3d::Zero(), AccelerationRotation::Identity());
}

A_ALWAYS_INLINE AccelerationRotation AccelerationTransform::getAccelerationRotation() const
{
    return AccelerationRotation(this->getRotation(), this->getRotationRate(), angvelDot_);
}

A_ALWAYS_INLINE void AccelerationTransform::getAccelerationRotation(AccelerationRotation &rot) const
{
    rot = AccelerationRotation(this->getRotation(), this->getRotationRate(), angvelDot_);
}

A_ALWAYS_INLINE void AccelerationTransform::setAccelerationRotation(const AccelerationRotation &rot)
{
    this->getRotation() = rot.getRotation();
    this->getRotationRate() = rot.getRotationRate();
    angvelDot_ = rot.getRotationRateDot();
}

A_ALWAYS_INLINE AccelerationTransform AccelerationTransform::composed(const AccelerationTransform &next) const
{
    /*!
    记 τ = M₁ᵀ t₂，则平移与速度的合成与 KinematicTransform 一致，
    加速度在此基础上多出三项：τ 与 M₁ᵀV₂ 都随中间系一起转动，其方向随时间变化，故求二阶导时不消去。
        A = A₁ + M₁ᵀA₂ + 2ω₁×(M₁ᵀV₂) + ω₁×(ω₁×τ) + α₁×τ
    */
    // 先取到局部量，再构造返回值，避免原地赋值
    Matrix3d mat1 = this->getMatrix();
    Vector3d angvel1 = this->getRotationRate();
    Vector3d angvelDot1 = this->rotationRateDot();
    Vector3d tau = next.getTranslation() * mat1;             // 即 M₁ᵀ t₂
    Vector3d velocity2 = next.getVelocity() * mat1;          // 即 M₁ᵀ V₂
    Vector3d acceleration2 = next.getAcceleration() * mat1;  // 即 M₁ᵀ A₂

    Vector3d translation = this->getTranslation() + tau;
    Vector3d velocity = this->getVelocity() + velocity2 + angvel1.cross(tau);
    Vector3d acceleration = this->getAcceleration() + acceleration2
        + angvel1.cross(velocity2) * 2.0
        + angvel1.cross(angvel1.cross(tau))
        + angvelDot1.cross(tau);

    AccelerationRotation rotation = AccelerationRotation(mat1, angvel1, angvelDot1)
        .composed(next.getAccelerationRotation());

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
    AccelerationRotation rotation = this->getAccelerationRotation();
    AccelerationRotation inversedRotation;
    rotation.getInverse(inversedRotation);

    rotation.transformVecVelAcc(
        -this->getTranslation(),
        -this->getVelocity(),
        -this->getAcceleration(),
        inversed.getTranslation(),
        inversed.getVelocity(),
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
    this->getAccelerationRotation().transformVecVelAcc(
        position - this->getTranslation(),
        velocity - this->getVelocity(),
        acceleration - this->getAcceleration(),
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
    this->getAccelerationRotation().transformVecVelAccInv(
        position,
        velocity,
        acceleration,
        positionOut,
        velocityOut,
        accelerationOut);

    positionOut += this->getTranslation();
    velocityOut += this->getVelocity();
    accelerationOut += this->getAcceleration();
}

AST_NAMESPACE_END
