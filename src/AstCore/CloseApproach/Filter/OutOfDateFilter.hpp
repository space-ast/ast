///
/// @file      OutOfDateFilter.hpp
/// @brief     过期预过滤
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

#include "CloseApproachFilter.hpp"
#include "AstCore/TimeInterval.hpp"
#include "AstUtil/Literals.hpp"

AST_NAMESPACE_BEGIN

/// @brief 过期预过滤
/// @details 剔除历元不在 [窗口起点 - pad, 窗口终点 + pad] 内的 TLE。
class AST_CORE_API OutOfDateFilter final : public CloseApproachFilter
{
public:
    explicit OutOfDateFilter(const CloseApproachOptions& opts);

    bool keep(const TLE& candidate) const override;

    void setInterval(const TimeInterval& interval){interval_ = interval;}
    void setThreshold(double threshold){threshold_ = threshold;}
private:
    TimeInterval interval_{};         ///< 分析窗口
    double threshold_{30_day};        ///< 过期门限 [s]
};

AST_NAMESPACE_END
