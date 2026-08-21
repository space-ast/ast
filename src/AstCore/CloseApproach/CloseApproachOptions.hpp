///
/// @file      CloseApproachOptions.hpp
/// @brief     接近分析配置
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
#include "AstCore/TimeInterval.hpp"
#include "AstUtil/Literals.hpp"
#include <cstddef>

AST_NAMESPACE_BEGIN
using namespace literals;

/// @brief 接近分析配置
/// @details 描述一次交会分析的窗口、距离阈值、预过滤开关与 padding、
///          以及最终暴力采样的步长与结果上限。
struct CloseApproachOptions
{
    TimeInterval window;               ///< 分析时间段
    double       threshold{10.0e3};    ///< 最大允许距离 [m]

    // -- 预过滤开关与 padding --
    bool   enableOutOfDate{true};      ///< 过期预过滤
    double outOfDatePad{30.0_day};     ///< 过期 padding [s]

    bool   enableApogeePerigee{true};  ///< 近/远地点预过滤
    double apogeePerigeePad{50.0_km};  ///< 近/远地点 padding [m]

    bool   enableOrbitPath{false};     ///< 轨道路径预过滤（保守性不足，默认关闭）
    double orbitPathPad{100_km};       ///< 轨道路径 padding [m]

    bool   enableTimeFilter{true};     ///< 时间预过滤（SGP4 交点过境收窄）
    double planeDistancePad{30_km};    ///< 面距 padding [m]（与 threshold 相加，用于交点过境/近共面判定）
    
    double coarseStep{300.0};          ///< 时间过滤粗扫描步长 [s]（SGP4 采样检测交点过境，交点周期约半圈故可取分钟级）
    double alignPad{10.0};             ///< 两目标交点过境时刻对齐判定阈值 [s]
    double searchMargin{10.0};         ///< 最终采样在节点对齐附近的搜索半宽 [s]

    // -- 最终检测 --
    double sampleStep{30.0};           ///< 暴力采样步长 [s]
    size_t maxResults{0};              ///< 结果数量上限，0 表示不限

    // -- 常量 --
    double gm{0.0};                    ///< 引力常数 [m^3/s^2]，0 表示取地球
    double pathGridStepDeg{5.0};       ///< 轨道路径过滤粗网格步长 [度]
};

AST_NAMESPACE_END
