///
/// @file      AttitudeECIVVLH.hpp
/// @brief     对地指向 + ECI 速度约束姿态
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
#include "AttitudeProfile.hpp"
#include "AttitudeTrajectoryRelated.hpp"
#include "AttitudeVVLH.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief 对地指向 + ECI 速度约束姿态
/// @details 
/// 体 Z 轴严格对齐地心对地方向，体 X 轴在保持 Z 轴对齐的前提下尽量指向惯性系速度方向，
/// 由此得到的姿态即 VVLH(Vehicle Velocity Local Horizontal, 速度局部水平)系在惯性系下的姿态
/// 与 @see AttitudeVVLH 的区别在于：计算该姿态的位置速度的参考系为天体惯性系
class AST_CORE_API AttitudeECIVVLH : public AttitudeVVLH
{
public:
    AST_OBJECT(AttitudeECIVVLH)

    AttitudeECIVVLH();
    AttitudeECIVVLH(Point* point, Body* body);
    ~AttitudeECIVVLH() override = default;
private:
    // 屏蔽父类的 setFrame 方法
    void setFrame(Frame* frame) = delete;
public:
    void setBody(Body* body);
};

/*! @} */

AST_NAMESPACE_END
