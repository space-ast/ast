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
class KinematicTransform : public Transform
{
public:
    KinematicTransform(){};

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


private:
    KinematicRotation& kinematicRotation() { return (KinematicRotation&)rotation_; }
protected:
    Vector3d angvel_;
    Vector3d velocity_;
};

AST_NAMESPACE_END
