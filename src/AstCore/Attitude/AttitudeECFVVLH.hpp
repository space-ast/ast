///
/// @file      AttitudeECFVVLH.hpp
/// @brief     对地指向 + ECF 速度约束姿态
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

/// @brief 对地指向 + ECF 速度约束姿态
/// @details STK 的 "Nadir alignment with ECF velocity constraint" 剖面。
/// 体 Z 轴严格对齐地心对地方向，体 X 轴在保持 Z 轴对齐的前提下尽量指向地固系速度方向。
/// 注意 ECF 速度与 ECI 速度在大偏心率轨道上差异很大(逼近远地点时尤为明显)，
/// 同步轨道下固连系速度更是接近于零，此时请改用 AttitudeECIVVLH。
/// @note 本剖面的对齐与约束关系是**类定义的一部分**，已在构造函数中固定为 STK 对应剖面的取值。
/// 基类提供设置接口是为了支持通用的"对齐与约束"剖面，在这里修改它们会让类名与实际行为不符。
class AST_CORE_API AttitudeECFVVLH : public AttitudeAlignConstrain
{
public:
    AST_OBJECT(AttitudeECFVVLH)

    AttitudeECFVVLH();
    ~AttitudeECFVVLH() override = default;

protected:
    /// @brief 本剖面的自然参考坐标系为地球固连系
    Frame* defaultFrame() const override;
};

/*! @} */

AST_NAMESPACE_END
