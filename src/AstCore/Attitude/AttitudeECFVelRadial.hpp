///
/// @file      AttitudeECFVelRadial.hpp
/// @brief     ECF 速度对齐 + 径向约束姿态
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
#include "AttitudeAlignConstrain.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief ECF 速度对齐 + 径向约束姿态
/// @details STK 的 "ECF Velocity Alignment with Radial Constraint" 剖面。
/// 体 X 轴严格对齐地固系速度方向，体 Z 轴在保持 X 轴对齐的前提下尽量指向径向(远离地心)。
/// 常用于飞机、地面车辆等大气层内载体。
/// @note 本剖面的对齐与约束关系是**类定义的一部分**，已在构造函数中固定为 STK 对应剖面的取值。
/// 基类提供设置接口是为了支持通用的"对齐与约束"剖面，在这里修改它们会让类名与实际行为不符。
class AST_CORE_API AttitudeECFVelRadial : public AttitudeAlignConstrain
{
public:
    AST_OBJECT(AttitudeECFVelRadial)

    AttitudeECFVelRadial();
    ~AttitudeECFVelRadial() override = default;

protected:
    /// @brief 本剖面的自然参考坐标系为地球固连系
    Frame* defaultFrame() const override;
};

/*! @} */

AST_NAMESPACE_END
