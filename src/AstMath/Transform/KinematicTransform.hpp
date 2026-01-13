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

AST_NAMESPACE_BEGIN

/// @brief 运动学变换
/// @details 在静态变换的基础上，添加了旋转角速度和平移速度。
class KinematicTransform : public Transform
{
public:
    KinematicTransform() = default;

    /// @brief 获取变换
    /// @return 变换
    const Transform& getTransform() const { return *this; }

    /// @brief 设置变换
    /// @param transform 变换
    void setTransform(const Transform& transform) { (Transform&)*this = transform; }

    /// @brief 获取旋转
    /// @return 旋转
    const KinematicRotation& getKinematicRotation() const { return const_cast<KinematicTransform&>(*this).kinematicRotation(); }

    /// @brief 设置旋转
    /// @param rot 旋转
    void setKinematicRotation(const KinematicRotation& rot) { kinematicRotation() = rot; }

    /// @brief 获取平移速度
    /// @return 速度
    const Vector3d& getVelocity() const { return velocity_; }

    /// @brief 设置平移速度
    /// @param vel 平移速度
    void setVelocity(const Vector3d& vel) { velocity_ = vel; }

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
    
    /// @brief 变换位置和速度
    /// @param position 位置
    /// @param velocity 速度
    /// @param positionOut 变换后的位置
    /// @param velocityOut 变换后的速度
    void transformPositionVelocity(const Vector3d& position, const Vector3d& velocity, Vector3d& positionOut, Vector3d& velocityOut);

private:
    KinematicRotation& kinematicRotation() { return (KinematicRotation&)rotation_; }
protected:
    Vector3d angvel_;
    Vector3d velocity_;
};

A_ALWAYS_INLINE KinematicTransform KinematicTransform::inverse() const
{
    KinematicTransform retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void KinematicTransform::getInverse(KinematicTransform &inversed) const
{
    // @todo 实现获取逆变换
}

A_ALWAYS_INLINE void KinematicTransform::transformPositionVelocity(const Vector3d &position, const Vector3d &velocity, Vector3d &positionOut, Vector3d &velocityOut)
{
    // @todo 实现变换位置和速度
}

AST_NAMESPACE_END
