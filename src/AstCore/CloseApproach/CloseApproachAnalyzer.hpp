///
/// @file      CloseApproachAnalyzer.hpp
/// @brief     接近分析编排器
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
#include "CloseApproachOptions.hpp"
#include "CloseApproachResult.hpp"
#include "AstCore/TLE.hpp"
#include "AstCore/TLEDatabase.hpp"
#include <vector>

AST_NAMESPACE_BEGIN

/// @brief 接近分析编排器
/// @details 给定参考对象（TLE）与候选目录，按 STK CAT 顺序执行
///          过期 → 近/远地点 → 轨道路径 → 时间 四级预过滤，再对幸存对象
///          做暴力采样检测与最近点精化，输出交会结果。
class AST_CORE_API CloseApproachAnalyzer
{
public:
    explicit CloseApproachAnalyzer(const CloseApproachOptions& opts);

    /// @brief 在 TLE 目录中搜索与参考对象的接近事件
    /// @param reference 参考对象 TLE
    /// @param catalog   TLE 目录
    /// @param report    输出报告
    /// @return 错误码
    errc_t analyze(const TLE& reference, const TLEDatabase& catalog, CloseApproachReport& report);

    /// @brief 在 TLE 列表中搜索与参考对象的接近事件
    /// @param reference  参考对象 TLE
    /// @param candidates 候选 TLE 列表
    /// @param report     输出报告
    /// @return 错误码
    errc_t analyze(const TLE& reference, const std::vector<TLE>& candidates, CloseApproachReport& report);

private:
    CloseApproachOptions opts_;
};

AST_NAMESPACE_END
