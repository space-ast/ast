///
/// @file      AstPlotAPI.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-10-08
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

#include "AstPlotAPI.hpp"
#include "AstPlot/AggBackend.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

matplot::backend::backend_interface* aNewAggBackend()
{
#if defined(AST_WITH_AGG) && defined(AST_WITH_MATPLOT)
    return new AggBackend();
#else
    aWarning(_("Agg 后端不可用，请检查是否启用了 agg 和 matplot++ 库"));
    return nullptr;
#endif
}

AST_NAMESPACE_END
