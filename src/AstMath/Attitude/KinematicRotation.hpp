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

AST_NAMESPACE_BEGIN

/// @brief     运动学旋转
/// @details   在静态旋转的基础上，增加了旋转的角速度信息
class KinematicRotation: public Rotation
{
public:
    /// @brief 运动学旋转默认构造函数
    KinematicRotation()
        : Rotation()
        , angvel_(Vector3d::Zero())
    {
    }

    /// @brief 获取旋转角速度
    /// @return 旋转角速度
    const Vector3d& getRotationRate() const { return angvel_; }

    /// @brief 设置旋转角速度
    /// @param angvel 旋转角速度
    void setRotationRate(const Vector3d& angvel) { angvel_ = angvel; }
    
    /// @brief 获取旋转
    /// @return 旋转
    const Rotation& getRotation() const { return *this; }

    /// @brief 设置旋转
    /// @param rot 旋转
    void setRotation(const Rotation& rot) { (Rotation&)*this = rot; }

protected:
    Vector3d angvel_;       ///< 角速度
};


AST_NAMESPACE_END
