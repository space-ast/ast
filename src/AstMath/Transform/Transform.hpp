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
    Transform();

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

protected:
    Vector3d translation_;  ///< 平移
    Rotation rotation_;     ///< 旋转
};


A_ALWAYS_INLINE Transform::Transform()
    : rotation_()
    , translation_(Vector3d::Zero())
{
}

AST_NAMESPACE_END
