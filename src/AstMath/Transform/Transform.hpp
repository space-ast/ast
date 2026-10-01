///
/// @file      Transform.hpp
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
#include "AstMath/Rotation.hpp"

AST_NAMESPACE_BEGIN


#define _AST_DEF_TRANSFORM_PROPERTIES\
    const Vector3d& translation() const { return translation_; }\
    Vector3d& translation() { return translation_; }\
    const Vector3d& getTranslation() const { return translation_; }\
    Vector3d& getTranslation() { return translation_; }\
    void setTranslation(const Vector3d& translation) { translation_ = translation; }\

/// @brief 坐标系转换类
/// @details 坐标系转换类表示三维空间中的坐标系转换
/// 包括坐标系平移和坐标系旋转
class Transform
{
public:

    /// @brief 转换默认构造函数
    Transform() = default;

    /// @brief 转换构造函数
    Transform(const Vector3d& trans, const Rotation& rot);

    /// @brief 获取单位转换
    /// @details 单位转换是指不进行任何转换的转换
    /// @return 单位转换
    static Transform Identity();

    _AST_DEF_TRANSFORM_PROPERTIES
    _AST_DEF_ROTATION_PROPERTIES


    /// @brief 组合下一个坐标系转换
    /// @warning 组合转换是先应用当前转换，再应用下一个转换。
    /// @param next 下一个转换
    Transform& compose(const Transform& next);

    /// @brief 组合下一个坐标系转换
    /// @warning 组合转换是先应用当前转换，再应用下一个转换。
    /// @param next 下一个坐标系转换
    /// @return 组合转换
    Transform composed(const Transform& next) const;

    /// @brief 组合下一个坐标系转换
    /// @warning 组合转换是先应用当前转换，再应用下一个转换。
    /// @param next 下一个坐标系转换
    /// @return 组合转换
    Transform operator*(const Transform& next) const;

    /// @brief 组合下一个坐标系转换
    /// @warning 组合转换是先应用当前转换，再应用下一个转换。
    /// @param next 下一个坐标系转换
    /// @return 组合转换
    Transform& operator*=(const Transform& next);

    /// @brief 获取逆转换
    /// @param inversed 逆转换
    void getInverse(Transform& inversed) const;

    /// @brief 获取逆转换
    /// @return 逆转换
    Transform inverse() const;

    /// @brief 设置为单位转换
    void setIdentity() { rotation_ = Rotation::Identity(); translation_ = Vector3d::Zero(); }

    /// @brief 转换位置
    /// @param position 位置
    /// @param positionOut 转换后的位置
    void transformPosition(const Vector3d& position, Vector3d& positionOut) const;

    /// @brief 转换位置
    /// @param position 位置
    /// @return 转换后的位置
    Vector3d transformPosition(const Vector3d& position) const;

protected:
    Vector3d translation_{};  ///< 平移
    Rotation rotation_{};     ///< 旋转
};

A_ALWAYS_INLINE Transform::Transform(const Vector3d &trans, const Rotation &rot)
    : translation_(trans), rotation_(rot)
{
}

A_ALWAYS_INLINE Transform Transform::Identity()
{
    return Transform(Vector3d::Zero(), Rotation::Identity());
}

A_ALWAYS_INLINE Transform &Transform::compose(const Transform &next)
{
    translation_ += next.translation() * matrix();
    rotation_ = rotation_.composed(next.rotation());
    return *this;
}

A_ALWAYS_INLINE Transform Transform::composed(const Transform &next) const
{
    /*!
    也可以通过 getRotation().transformVectorInv() 来实现。
    */
    return Transform(
        translation_ + next.translation() * matrix(),
        rotation_.composed(next.rotation())
    );
}

A_ALWAYS_INLINE Transform Transform::operator*(const Transform &next) const
{
    return this->composed(next);
}

A_ALWAYS_INLINE Transform &Transform::operator*=(const Transform &next)
{
    return this->compose(next);
}

A_ALWAYS_INLINE void Transform::getInverse(Transform &inversed) const
{
    rotation_.transformVector(-translation_, inversed.translation());
    rotation_.getInverse(inversed.rotation());
}

A_ALWAYS_INLINE Transform Transform::inverse() const
{
    Transform retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void Transform::transformPosition(const Vector3d &position, Vector3d &positionOut) const
{
    /*!
    先进行坐标系平移，再进行坐标系旋转。
    */
    rotation_.transformVector(position - translation_, positionOut);
}

A_ALWAYS_INLINE Vector3d Transform::transformPosition(const Vector3d &position) const
{
    Vector3d retval;
    this->transformPosition(position, retval);
    return retval;
}

AST_NAMESPACE_END
