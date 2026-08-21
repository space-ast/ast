///
/// @file      OrbitPathFilter.hpp
/// @brief     轨道路径预过滤
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
#include "AstUtil/Constants.h"

AST_NAMESPACE_BEGIN

/// @brief 轨道路径预过滤
/// @details 计算候选对象与参考对象两条二体椭圆在惯性空间的最小路径距离，
///          大于 阈值 + padding 则剔除。不考虑时间/相位。
class AST_CORE_API OrbitPathFilter final : public CloseApproachFilter
{
public:
    OrbitPathFilter(const CloseApproachOptions& opts, const CloseApproachContext& ctx);

    bool keep(const TLE& candidate) const override;

private:
    OrbElem referenceOrbit_{};
    double threshold_{};
    double gm_{kEarthGrav};
};

AST_NAMESPACE_END
