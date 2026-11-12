///
/// @file      GnuplotBackend.hpp
/// @brief     matplot++ gnuplot 后端薄封装
/// @details   复用 matplot++ 自带的 gnuplot 管道后端（通过外部 gnuplot 进程渲染），
///            当前不增加任何行为，仅作为插件公开的类型，便于后续定制。
/// @author    axel
/// @date      2026-11-12
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
#include "AstPluginGnuplotGlobal.hpp"

#if defined(AST_WITH_MATPLOTPLUSPLUS)

A_SUPPRESS_WARNINGS_BEGIN
#include <matplot/backend/gnuplot.h>
A_SUPPRESS_WARNINGS_END

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plot
    @{
*/

/// matplot++ gnuplot 后端
///
/// 继承 matplot++ 自带的 gnuplot 后端，行为完全一致：构造时启动外部
/// gnuplot 进程，figure 的绘制命令通过管道发送给 gnuplot 执行。
class AST_PLUGIN_GNUPLOT_API GnuplotBackend : public matplot::backend::gnuplot
{
public:
    GnuplotBackend();
    ~GnuplotBackend() override;
};

/*! @} */

AST_NAMESPACE_END

#endif // AST_WITH_MATPLOTPLUSPLUS
