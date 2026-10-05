///
/// @file      AttitudeSunPointingEclpNormal.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-05
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
#include "AstCore/AttitudeSunRelated.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Attitude
    @{
*/

/// @brief     太阳指向 + 黄道法向约束姿态
/// @details   "Sun Alignment with Ecliptic Normal Constraint"
/// 体 X 轴严格指向太阳方向，体 Z 轴在保持 X 轴对齐的前提下尽量指向黄道面法向
class AST_CORE_API AttitudeSunPointingEclpNormal : public AttitudeSunRelated
{
public:
    AST_OBJECT(AttitudeSunPointingEclpNormal)

    AttitudeSunPointingEclpNormal() = default;
    AttitudeSunPointingEclpNormal(Point* point, Frame* frame);
    AttitudeSunPointingEclpNormal(Point* point, Body* body);
    ~AttitudeSunPointingEclpNormal() override = default;
public:
    errc_t getTransform(const TimePoint& tp, Rotation& rotation) const override;
    errc_t getTransform(const TimePoint& tp, KinematicRotation& rotation) const override;
protected:
    /// @brief 黄道面法向(黄道北极)在参考坐标系下的方向
    errc_t getEclipticNormalLocal(const TimePoint& tp, Vector3d& normal) const;
    /// @brief 黄道面法向在参考坐标系下的方向及其时间变化率
    errc_t getEclipticNormalLocal(const TimePoint& tp, Vector3d& normal, Vector3d& normalRate) const;
};

/*! @} */

AST_NAMESPACE_END
