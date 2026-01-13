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


/// @brief 变换类
/// @details 变换类表示三维空间中的变换，包括平移和旋转。
class Transform
{
public:

    /// @brief 变换默认构造函数
    Transform() = default;

    /// @brief 获取旋转
    /// @return 旋转
    const Rotation& getRotation() const { return rotation_; }

    /// @brief 设置旋转
    /// @param rotation 旋转
    void setRotation(const Rotation& rotation) { rotation_ = rotation; }

    /// @brief 获取平移
    /// @return 平移
    const Vector3d& getTranslation() const { return translation_; }

    /// @brief 设置平移
    /// @param translation 平移
    void setTranslation(const Vector3d& translation) { translation_ = translation; }

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    Transform& compose(const Transform& next);

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    Transform composed(const Transform& next) const;

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    Transform operator*(const Transform& next) const;

    /// @brief 组合下一个变换
    /// @warning 组合变换是先应用当前变换，再应用下一个变换。
    /// @param next 下一个变换
    /// @return 组合变换
    Transform& operator*=(const Transform& next);

    /// @brief 获取逆变换
    /// @param inversed 逆变换
    void getInverse(Rotation& inversed) const;

    /// @brief 获取逆变换
    /// @return 逆变换
    Rotation inverse() const;

    /// @brief 变换位置
    /// @param position 位置
    /// @param positionOut 变换后的位置
    void transformPosition(const Vector3d& position, Vector3d& positionOut) const;

    /// @brief 变换位置
    /// @param position 位置
    /// @return 变换后的位置
    Vector3d transformPosition(const Vector3d& position) const;

protected:
    Vector3d translation_;  ///< 平移
    Rotation rotation_;     ///< 旋转
};



A_ALWAYS_INLINE void Transform::getInverse(Rotation &inversed) const
{
    // @todo 实现获取逆变换
}

A_ALWAYS_INLINE Rotation Transform::inverse() const
{
    Rotation retval;
    this->getInverse(retval);
    return retval;
}

A_ALWAYS_INLINE void Transform::transformPosition(const Vector3d &position, Vector3d &positionOut) const
{
    // @todo 实现变换位置
}

A_ALWAYS_INLINE Vector3d Transform::transformPosition(const Vector3d &position) const
{
    Vector3d retval;
    this->transformPosition(position, retval);
    return retval;
}

AST_NAMESPACE_END
