///
/// @file      ConeEclipseCalculator.hpp
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

/// @brief 双锥阴影模型光照计算器
/// @details 使用双锥阴影模型（本影-半影模型）计算光照强度。该模型考虑光源的视张角，
///          当光源（如太阳）被遮挡体（如地球）部分遮挡时，可计算出0~1之间的半影光照比例。
///          - 全光照（1）：光源与遮挡体视圆盘相离
///          - 全阴影（0）：光源完全被遮挡体遮蔽
///          - 半影（0~1）：光源部分被遮挡，按被遮挡立体角比例计算
class AST_CORE_API ConeEclipseCalculator final: public EclipseCalculator
{
public:
    ConeEclipseCalculator() = default;
    ~ConeEclipseCalculator() = default;

    /// @brief 计算光照强度（含半影）
    /// @param time 时间点
    /// @param position 观测者位置（相对于 frame 坐标系）
    /// @param frame 观测者所在参考系
    /// @return 光照强度, 0: 全阴影, 1: 全光照，0-1: 部分遮挡（半影）
    double getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame) override;

    /// @brief 计算光照强度（含半影），并返回造成最暗遮蔽的遮挡体
    /// @param time 时间点
    /// @param position 观测者位置（相对于 frame 坐标系）
    /// @param frame 观测者所在参考系
    /// @param obstructionOut 输出：给出最小光照比的遮挡体（全光照时为 nullptr；可为 nullptr 表示不关心）
    /// @return 光照强度, 0: 全阴影, 1: 全光照，0-1: 部分遮挡（半影）
    double getLightingRatio(const TimePoint& time, const Vector3d& position, Frame* frame,
                            CelestialBody** obstructionOut);
};



/*! @} */

AST_NAMESPACE_END
