///
/// @file      IAUXYSSeries.cpp
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

#include "AstCore/IAUXYS.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

err_t IAUXYS::load(StringView xSeriesPath, StringView ySeriesPath, StringView spxy2SeriesPath)
{
    err_t ret1 = xSeries_.load(xSeriesPath);
    if (ret1 != 0)
    {
        aError("failed to load x series file: %s", xSeriesPath.data());
    }
    err_t ret2 = ySeries_.load(ySeriesPath);
    if (ret2 != 0)
    {
        aError("failed to load y series file: %s", ySeriesPath.data());
    }
    err_t ret3 = spxy2Series_.load(spxy2SeriesPath);
    if (ret3 != 0)
    {
        aError("failed to load spxy2 series file: %s", spxy2SeriesPath.data());
    }
    err_t ret = ret1 | ret2 | ret3;
    if (ret == 0)
    {
        isLoaded_ = true;
    }
    return ret;
}


void IAUXYS::eval(double t,  const FundamentalArguments& fundargs, array3d &xys) const
{
    xys[0] = xSeries_.eval(t, fundargs);
    xys[1] = ySeries_.eval(t, fundargs);
    xys[2] = spxy2Series_.eval(t, fundargs) - xys[0] * xys[1] / 2.0;
}

AST_NAMESPACE_END


