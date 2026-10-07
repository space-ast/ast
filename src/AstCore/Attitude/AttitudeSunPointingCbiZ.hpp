///
/// @file      AttitudeSunPointingCbiZ.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-06
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
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
#include "AttitudeSunRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief     太阳指向 + 天体惯性系Z轴方向约束
/// @details   "Sun Alignment with ECI Z Axis Constraint"
/// 体 X 轴严格指向太阳方向，体 Z 轴在保持 X 轴对齐的前提下尽量指向天体惯性系Z轴方向。
/// 参考坐标系固定为天体惯性系，因此约束方向即该系的 Z 轴(极轴方向)。
class AST_CORE_API AttitudeSunPointingCbiZ : public AttitudeSunRelated
{
public:
    AST_OBJECT(AttitudeSunPointingCbiZ)

    AttitudeSunPointingCbiZ() = default;
    AttitudeSunPointingCbiZ(Point* point, Body* body);
    ~AttitudeSunPointingCbiZ() override = default;
public:
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
private:
    // 屏蔽父类的 setFrame 方法
    void setFrame(Frame* frame) = delete;
public:
    void setBody(Body* body);
};



/*! @} */

AST_NAMESPACE_END
