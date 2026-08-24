///
/// @file      CloseApproachResult.hpp
/// @brief     接近分析结果
/// @author    axel
/// @date      2026-08-19
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
#include "AstCore/TimePoint.hpp"
#include <cstddef>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 单条接近结果
struct CloseApproachResult
{
    int         noradId{0};       ///< 次要目标 NORAD 目录编号
    std::string name{};           ///< 次要目标名称（来自 3 行 TLE 名称行，2 行格式为空）
    TimePoint   tca{};            ///< 最近接近时刻（TCA）
    double      minDistance{0.0}; ///< 最小距离 [m]
    double      relVelocity{0.0}; ///< TCA 处相对速度 [m/s]
};

/// @brief 接近分析报告
/// @details 汇总一次接近分析的结果与各级预过滤存活计数（用于诊断）。
struct CloseApproachReport
{
    std::vector<CloseApproachResult> results{};   ///< 接近结果，按 TCA 排序
    size_t totalCatalog{0};         ///< 目录总数
    size_t afterOutOfDate{0};       ///< 过期过滤后存活数
    size_t afterApogeePerigee{0};   ///< 近/远地点过滤后存活数
    size_t afterOrbitPath{0};       ///< 轨道路径过滤后存活数
    size_t afterTimeFilter{0};      ///< 时间过滤后存活数

    // 各阶段耗时 [s]（诊断用）
    double secondsOutOfDate{0.0};
    double secondsApogeePerigee{0.0};
    double secondsOrbitPath{0.0};
    double secondsTimeFilter{0.0};
    double secondsBruteForce{0.0};
    double secondsCoarseScan{0.0};   ///< 时间过滤粗扫描耗时
    double secondsFineScan{0.0};     ///< 细采样+精化耗时
    double totalSearchSeconds{0.0};  ///< 细采样搜索区间总时长（诊断）
};

AST_NAMESPACE_END
