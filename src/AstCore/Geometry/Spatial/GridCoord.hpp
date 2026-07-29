///
/// @file      GridCoord.hpp
/// @brief     体积网格坐标轴定义
/// @author    axel
/// @date      2026-07-29
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

AST_NAMESPACE_BEGIN

/// @brief 网格坐标离散化方法
enum class EGridCoordMethod
{
    eFixedNumSteps,   ///< 固定步数
    eFixedSpacing     ///< 固定间距
};

/// @brief 坐标轴定义
/// @details 描述体积网格单个坐标轴的范围和离散化方式
struct AST_CORE_API GridCoord
{
    EGridCoordMethod method_{EGridCoordMethod::eFixedNumSteps};  ///< 离散化方法
    double           minValue_{0.0};                              ///< 坐标最小值
    double           maxValue_{0.0};                              ///< 坐标最大值
    int              numSteps_{10};                               ///< 步数
};

AST_NAMESPACE_END
