///
/// @file      AttitudeYPRFixedECI.hpp
/// @brief     偏航-俯仰-滚转固定姿态
/// @details
/// @author    axel
/// @date      2026-09-28
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
#include "AttitudeFixed.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 偏航-俯仰-滚转固定姿态剖面 (YPRFixedECI)
/// @details 用欧拉角(Yaw/Pitch/Roll)描述体轴系相对惯性系的恒定取向，
///          对应 STK .sa 文件中 BEGIN YPRFixedECI 块的 Yaw / Pitch / Roll / UiSequence 字段。
///
/// @note UiCoordType 字段目前只作数据保存，其取值到坐标系的映射关系留待接入 .sa 加载器时
///       确定，本类一律使用地球惯性系。
/// @note 三个欧拉角一经设置就会立刻重建内部旋转，因此直接调用基类的 setRotation 会让
///       二者不一致；需要直接指定取向时请使用 AttitudeFixed。
class AST_CORE_API AttitudeYPRFixedECI : public AttitudeFixed
{
public:
    AST_OBJECT(AttitudeYPRFixedECI)

    AttitudeYPRFixedECI();
    ~AttitudeYPRFixedECI() override = default;

PROPERTIES:
    /// @brief 获取偏航角 [rad]
    double getYaw() const { return yaw_; }
    /// @brief 设置偏航角 [rad]
    void setYaw(double yaw) { yaw_ = yaw; updateRotation(); }

    /// @brief 获取俯仰角 [rad]
    double getPitch() const { return pitch_; }
    /// @brief 设置俯仰角 [rad]
    void setPitch(double pitch) { pitch_ = pitch; updateRotation(); }

    /// @brief 获取滚转角 [rad]
    double getRoll() const { return roll_; }
    /// @brief 设置滚转角 [rad]
    void setRoll(double roll) { roll_ = roll; updateRotation(); }

    /// @brief 获取欧拉角转序(如 321 表示偏航-俯仰-滚转)
    int getUiSequence() const { return uiSequence_; }
    /// @brief 设置欧拉角转序
    void setUiSequence(int sequence) { uiSequence_ = sequence; updateRotation(); }

    /// @brief 获取 STK 的坐标系类型字段
    int getUiCoordType() const { return uiCoordType_; }
    /// @brief 设置 STK 的坐标系类型字段
    void setUiCoordType(int coordType) { uiCoordType_ = coordType; }

protected:
    /// @brief 依据当前的欧拉角与转序重建内部旋转
    void updateRotation();

protected:
    double yaw_{0.0};        ///< 偏航角 [rad]
    double pitch_{0.0};      ///< 俯仰角 [rad]
    double roll_{0.0};       ///< 滚转角 [rad]
    int    uiSequence_{321}; ///< 欧拉角转序
    int    uiCoordType_{2};  ///< STK 坐标系类型字段(语义待定)
};

/*! @} */

AST_NAMESPACE_END
