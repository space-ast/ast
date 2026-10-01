///
/// @file      KinematicTransform.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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
#include "Transform.hpp"
#include "AstMath/KinematicRotation.hpp"
#include "AstCore/OrbitElement.hpp"   /// @fixme 头文件依赖倒置

AST_NAMESPACE_BEGIN


#define _AST_DEF_KINEMATICTRANSFORM_PROPERTIES\
    _AST_DEF_TRANSFORM_PROPERTIES\
    const Vector3d& velocity() const { return velocity_; }\
    Vector3d& velocity() { return velocity_; }\
    const Vector3d& getVelocity() const { return velocity_; }\
    Vector3d& getVelocity() { return velocity_; }\
    void setVelocity(const Vector3d& velocity) { velocity_ = velocity; }\
    const Transform& transform() const { return reinterpret_cast<const Transform&>(translation_); }\
    Transform& transform() { return reinterpret_cast<Transform&>(translation_); }\
    const Transform& getTransform() const { return transform(); }\
    Transform& getTransform() { return transform(); }\
    void setTransform(const Transform& xform) { transform() = xform; }



/// @brief 运动学变换
/// @details 在静态变换的基础上，添加了旋转角速度和平移速度。
class KinematicTransform
{
public:
    KinematicTransform() = default;

    /// @brief 从旋转和平移状态构造变换
    /// @param translate 平移量
    /// @param rot 旋转量
    A_ALWAYS_INLINE
    KinematicTransform(const CartState& translate, const KinematicRotation& rot)
        : velocity_(translate.vel())
        , translation_(translate.pos())
        , rotation_(rot.rotation())
        , angvel_(rot.rotationRate()) 
    {}

    A_ALWAYS_INLINE
    KinematicTransform(const Vector3d& translation, const Vector3d& velocity, const KinematicRotation& rot)
        : velocity_(velocity)
        , translation_(translation)
        , rotation_(rot.rotation())
        , angvel_(rot.rotationRate()) 
    {}

    /// @brief 获取单位变换
    /// @return 单位变换
    static KinematicTransform Identity();


public:
    _AST_DEF_KINEMATICTRANSFORM_PROPERTIES
    _AST_DEF_KINEMATICROTATION_PROPERTIES

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    KinematicTransform& compose(const KinematicTransform& next);

    /// @brief 获取组合变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    KinematicTransform composed(const KinematicTransform& next) const;

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    KinematicTransform operator*(const KinematicTransform& next) const;

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    KinematicTransform& operator*=(const KinematicTransform& next);


    /// @brief 获取逆变换
    /// @return 逆变换
    KinematicTransform inverse() const;

    /// @brief 获取逆变换
    /// @param inversed 逆变换
    void getInverse(KinematicTransform& inversed) const;
    
    /// @brief 设置为单位变换
    void setIdentity();

    /// @brief 变换位置和速度
    /// @param position 位置
    /// @param velocity 速度
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    void transformPositionVelocity(const Vector3d& position, const Vector3d& velocity, Vector3d& positionOut, Vector3d& velocityOut);

    /// @brief 变换位置和速度
    /// @param position 位置
    /// @param velocity 速度
    /// @return 变换后的位置和速度
    CartState transformPositionVelocity(const CartState& state);
protected:
    Vector3d velocity_{};     ///< 速度
    Vector3d translation_{};  ///< 平移
    Rotation rotation_{};     ///< 旋转
    Vector3d angvel_{};       ///< 角速度
};

A_ALWAYS_INLINE KinematicTransform KinematicTransform::Identity()
{
    return KinematicTransform(CartState::Zero(), KinematicRotation::Identity());
}

A_ALWAYS_INLINE void KinematicTransform::setIdentity()
{
    velocity_ = Vector3d::Zero();
    translation_ = Vector3d::Zero();
    rotation_ = Rotation::Identity();
    angvel_ = Vector3d::Zero();
}

A_ALWAYS_INLINE KinematicTransform &KinematicTransform::compose(const KinematicTransform &next)
{
    *this = this->composed(next);
    return *this;
}

A_ALWAYS_INLINE KinematicTransform KinematicTransform::composed(const KinematicTransform &next) const
{
    KinematicRotation rotation = this->kinematicRotation().composed(next.kinematicRotation());
    /*
    也可以通过 getRotation().transformVectorVelocityInv() 来实现。
    */
    Vector3d vector = next.translation() * this->matrix();

    Vector3d translation = this->translation() + vector;
    Vector3d velocity = this->velocity() + next.velocity() * this->matrix() 
                        + this->rotationRate().cross(vector);
    return KinematicTransform(translation, velocity, rotation);
}

A_ALWAYS_INLINE KinematicTransform KinematicTransform::operator*(const KinematicTransform &next) const
{
    return composed(next);
}

A_ALWAYS_INLINE KinematicTransform &KinematicTransform::operator*=(const KinematicTransform &next)
{
    return compose(next);
}

A_ALWAYS_INLINE KinematicTransform KinematicTransform::inverse() const
{
    KinematicTransform retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void KinematicTransform::getInverse(KinematicTransform &inversed) const
{
    this->kinematicRotation().transformVectorVelocity(-this->translation(), -this->velocity(), inversed.translation(), inversed.velocity());
    this->kinematicRotation().getInverse(inversed.kinematicRotation());
}

A_ALWAYS_INLINE void KinematicTransform::transformPositionVelocity(const Vector3d &position, const Vector3d &velocity, Vector3d &positionOut, Vector3d &velocityOut)
{
    #if 0
    // 先计算相对位置向量
    Vector3d relPos = position - this->getTranslation();
    // 旋转位置
    this->getRotation().transformVector(relPos, positionOut);
    // 计算速度：绝对速度 - 平动速度 - 角速度 × 相对位置向量
    Vector3d velDiff = velocity - this->getVelocity() - this->getRotationRate().cross(relPos);
    // 旋转速度
    this->getRotation().transformVector(velDiff, velocityOut);
    #else
    /*!
    先进行坐标系平移，再进行坐标系旋转。
    */
    this->kinematicRotation().transformVectorVelocity(
        position - this->translation(), 
        velocity - this->velocity(), 
        positionOut, 
        velocityOut
    );
    #endif
}

A_ALWAYS_INLINE CartState KinematicTransform::transformPositionVelocity(const CartState &state)
{
    CartState stateOut;
    this->transformPositionVelocity(state.pos(), state.vel(), stateOut.pos(), stateOut.vel());
    return stateOut;
}


AST_NAMESPACE_END
