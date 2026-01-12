///
/// @file      IAUXYSSeries.hpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-12
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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
#include "AstCore/NutationSeries.hpp"


AST_NAMESPACE_BEGIN

/// @brief IERS 2003/2010 岁差章动模型XYS系数
/// 参考 IAU 2000决议中的新岁差-章动模型
class IAUXYS
{
public:
    IAUXYS() = default;
    ~IAUXYS() = default;

    /// @brief 加载岁差章动模型XYS系数文件
    /// @param xSeriesPath X系数文件路径
    /// @param ySeriesPath Y系数文件路径
    /// @param spxy2SeriesPath S系数文件路径
    err_t load(StringView xSeriesPath, StringView ySeriesPath, StringView spxy2SeriesPath);


    /// @brief 加载默认岁差章动模型XYS系数文件
    err_t loadDefault();

    /// @brief 计算岁差章动模型XYS系数
    /// @param t 儒略日
    /// @param x 输出X系数
    /// @param y 输出Y系数
    /// @param s 输出S+XY/2系数
    void eval(double t, const FundamentalArguments& fundargs, array3d& xys) const;

protected:
    NutationSeries xSeries_;        ///< 岁差章动模型X系数
    NutationSeries ySeries_;        ///< 岁差章动模型Y系数
    NutationSeries spxy2Series_;    ///< 岁差章动模型S+XY/2
};


AST_NAMESPACE_END
