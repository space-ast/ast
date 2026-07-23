///
/// @file      BodyObstruction.hpp
/// @brief     天体遮挡判断
/// @details   提供视线到天体表面最短距离的计算函数，用于可见性分析中的遮挡约束。
/// @author    axel
/// @date      2026-07-21
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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
#include "AstCore/BodyShape.hpp"

AST_NAMESPACE_BEGIN

/// @brief 计算视线到天体表面的最短有符号距离
/// @param observerPos 观测者在天体固连系中的位置
/// @param targetPos 目标在天体固连系中的位置
/// @param bodyShape 天体形状（球体 / 扁球体 / 椭球体）
/// @return 最短距离：>0 视线未穿入天体表面，<0 视线穿入，==0 恰好相切
AST_CORE_API double aLineOfSightClearance(
    const Vector3d& observerPos,
    const Vector3d& targetPos,
    const BodyShape* bodyShape);

/// @brief 便捷方法：视线是否被中心天体遮挡
/// @param observerPos 观测者在天体固连系中的位置
/// @param targetPos 目标在天体固连系中的位置
/// @param bodyShape 天体形状
/// @return true = 被遮挡，false = 无遮挡
inline bool aLineOfSightObstructed(
    const Vector3d& observerPos,
    const Vector3d& targetPos,
    const BodyShape* bodyShape)
{
    return aLineOfSightClearance(observerPos, targetPos, bodyShape) < 0.0;
}

AST_NAMESPACE_END
