///
/// @file      CylindricalEclipseCalculator.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-06-10
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
#include "EclipseCalculator.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 圆柱阴影模型光照计算器
/// @details 使用圆柱阴影模型计算光照强度。该模型假定光源为平行光（光源无穷远），
///          遮挡体在光源反方向投射出一个圆柱形阴影。
///          当观测者位于阴影圆柱内时，返回全阴影（0）；
///          否则返回全光照（1）。
///          该模型没有半影区，结果为二值（0或1）。
class AST_CORE_API CylindricalEclipseCalculator final: public EclipseCalculator
{
public:
    CylindricalEclipseCalculator() = default;
    ~CylindricalEclipseCalculator() = default;

    /// @brief 计算光照强度
    /// @param time 时间点
    /// @param position 观测者位置（相对于 frame 坐标系）
    /// @param frame 观测者所在参考系
    /// @return 光照强度, 0: 全阴影, 1: 全光照（圆柱模型无半影）
    double getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame) override;
};



/*! @} */

AST_NAMESPACE_END
