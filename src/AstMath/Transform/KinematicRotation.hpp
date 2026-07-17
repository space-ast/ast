///
/// @file      KinematicRotation.hpp
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
#include "Rotation.hpp"
#include "AstCore/OrbitElement.hpp"    /// @fixme 头文件依赖倒置

AST_NAMESPACE_BEGIN

/// @brief     运动学坐标系旋转
/// @details   在静态坐标系旋转的基础上，增加了坐标系旋转的角速度信息
class KinematicRotation: protected Rotation
{
public:
    using Rotation::getMatrix;
    using Rotation::getQuaternion;
    using Rotation::transformVector;
    using Rotation::transformVectorInv;

    /// @brief 获取单位运动学旋转
    static KinematicRotation Identity();

    /// @brief 运动学旋转默认构造函数
    KinematicRotation() = default;

    /// @brief 运动学坐标系旋转构造函数
    /// @param rot 旋转
    /// @param angvel 旋转角速度
    KinematicRotation(const Rotation& rot, const Vector3d& angvel);


    /// @brief 运动学坐标系旋转构造函数
    /// @param mat 旋转矩阵
    /// @param angvel 旋转角速度
    KinematicRotation(const Matrix3d& mat, const Vector3d& angvel);

    /// @brief 获取坐标系旋转角速度
    /// @return 旋转角速度
    const Vector3d& getRotationRate() const { return angvel_; }

    /// @brief 设置坐标系旋转角速度
    /// @param angvel 旋转角速度
    void setRotationRate(const Vector3d& angvel) { angvel_ = angvel; }
    
    /// @brief 获取坐标系旋转
    /// @return 旋转
    const Rotation& getRotation() const { return *this; }
    Rotation& getRotation() { return *this; }


    /// @brief 设置坐标系旋转
    /// @param rot 旋转
    void setRotation(const Rotation& rot) { (Rotation&)*this = rot; }

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    KinematicRotation& compose(const KinematicRotation& next);

    /// @brief 组合下一个旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    KinematicRotation composed(const KinematicRotation& next) const;

    /// @brief 组合下一个旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    KinematicRotation operator*(const KinematicRotation& next) const;

    /// @brief 组合下一个坐标系旋转
    /// @warning 组合旋转是先应用当前旋转，再应用下一个坐标系旋转。
    /// @param next 下一个坐标系旋转
    /// @return 组合旋转
    KinematicRotation& operator*=(const KinematicRotation& next);

    /// @brief 获取逆旋转
    /// @param inversed 逆旋转
    void getInverse(KinematicRotation& inversed) const;

    /// @brief 获取逆旋转
    /// @return 逆旋转
    KinematicRotation inverse() const;

    /// @brief 变换向量和速度
    /// @param vector 向量
    /// @param velocity 速度
    /// @param vectorOut 变换后的向量
    /// @param velocityOut 变换后的速度
    void transformVectorVelocity(const Vector3d& vector, const Vector3d& velocity, Vector3d& vectorOut, Vector3d& velocityOut) const;

    /// @brief 变换状态向量和速度
    /// @param state 状态向量
    /// @return 变换后的状态向量
    CartState transformCartState(const CartState& state) const;

    /// @brief 变换位置和速度（逆变换）
    /// @warning 注意：这里是逆变换，等价于 `inverse().transformVectorVelocity(...)`;
    /// @param vector 向量
    /// @param velocity 速度
    /// @param vectorOut 变换后的向量
    /// @param velocityOut 变换后的速度
    void transformVectorVelocityInv(const Vector3d& vector, const Vector3d& velocity, Vector3d& vectorOut, Vector3d& velocityOut) const;
protected:
    Vector3d angvel_{};       ///< 角速度
};

A_ALWAYS_INLINE KinematicRotation KinematicRotation::Identity()
{
    return KinematicRotation(Matrix3d::Identity(), Vector3d::Zero());
}

A_ALWAYS_INLINE KinematicRotation::KinematicRotation(const Matrix3d &mat, const Vector3d &angvel)
    : Rotation(mat)
    , angvel_(angvel)
{
}

A_ALWAYS_INLINE KinematicRotation::KinematicRotation(const Rotation &rot, const Vector3d &angvel)
    : Rotation(rot)
    , angvel_(angvel)
{
}

A_ALWAYS_INLINE KinematicRotation &KinematicRotation::compose(const KinematicRotation &next)
{
    angvel_ = this->angvel_ + next.angvel_ * this->matrix_;
    matrix_ = next.matrix_ * this->matrix_;
    return *this;
}

A_ALWAYS_INLINE KinematicRotation KinematicRotation::composed(const KinematicRotation &next) const
{
    return KinematicRotation(next.matrix_ * this->matrix_, this->angvel_ + next.angvel_ * this->matrix_);
}

A_ALWAYS_INLINE KinematicRotation KinematicRotation::operator*(const KinematicRotation &next) const
{
    return composed(next);
}

A_ALWAYS_INLINE KinematicRotation &KinematicRotation::operator*=(const KinematicRotation &next)
{
    return compose(next);
}

A_ALWAYS_INLINE void KinematicRotation::getInverse(KinematicRotation &inversed) const
{
    inversed.setRotationRate(-(this->matrix_*angvel_));
    this->Rotation::getInverse(inversed);
}

A_ALWAYS_INLINE KinematicRotation KinematicRotation::inverse() const
{
    KinematicRotation retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void KinematicRotation::transformVectorVelocity(const Vector3d &vector, const Vector3d &velocity, Vector3d& vectorOut, Vector3d &velocityOut) const
{
    // 注意：这里要先计算velocityOut，防止vector和vectorOut地址相同时值被覆盖
    velocityOut = this->matrix_ * (velocity - this->angvel_.cross(vector));
    vectorOut = this->matrix_ * vector;
}

A_ALWAYS_INLINE CartState KinematicRotation::transformCartState(const CartState &state) const
{
    CartState stateOut;
    this->transformVectorVelocity(state.pos(), state.vel(), stateOut.pos(), stateOut.vel());
    return stateOut;
}

A_ALWAYS_INLINE void KinematicRotation::transformVectorVelocityInv(const Vector3d &vector, const Vector3d &velocity, Vector3d &vectorOut, Vector3d &velocityOut) const
{
    vectorOut = vector * this->matrix_;
    velocityOut = velocity* this->matrix_ + this->angvel_.cross(vectorOut);
}

AST_NAMESPACE_END
