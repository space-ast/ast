///
/// @file      NoneEclipseCalculator.hpp
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
#include "AstCore/EclipseCalculator.hpp"

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/

/// @brief 空阴影光照计算器（无阴影模型）
/// @details 不计算任何阴影，始终返回全光照（1.0）。
///          用于无需考虑阴影影响的场景，或作为默认/占位计算器。
///          采用空对象模式（Null Object Pattern），避免空指针检查。
class AST_CORE_API NoneEclipseCalculator : public EclipseCalculator
{
public:
    NoneEclipseCalculator() = default;
    NoneEclipseCalculator(CelestialBody* lightSource)
        : EclipseCalculator(lightSource)
    {
    }
    ~NoneEclipseCalculator() = default;

    /// @brief 始终返回全光照
    /// @param time 时间点（未使用）
    /// @param position 观测者位置（未使用）
    /// @param frame 观测者所在参考系（未使用）
    /// @return 始终为 1.0（全光照）
    double getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame) override{return 1.0;}
};

/*! @} */

AST_NAMESPACE_END
