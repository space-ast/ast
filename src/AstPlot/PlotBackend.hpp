///
/// @file      PlotBackend.hpp
/// @brief     matplot++ 绘图后端插件接口与宿主 API
/// @details   定义绘图后端提供者接口，以及按插件键创建/选择后端的宿主接口
/// @author    axel
/// @date      2026-11-11
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
#include "AstUtil/PluginInterface.hpp"
#include "AstUtil/StringView.hpp"
#include <string>
#include <vector>

namespace matplot {
namespace backend {
class backend_interface;
}
} // namespace matplot

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Plot
    @{
*/

/// @brief 绘图后端提供者接口
/// @details 由绘图后端插件（AstPluginAgg、AstPluginQwt 等）实现，
///          主程序通过插件注册表按索引键查找并创建后端实例
class AST_PLOT_API IPlotBackendProvider
{
public:
    virtual ~IPlotBackendProvider() = default;

    /// @brief 创建后端实例
    /// @return 后端实例；所有权归调用方
    virtual matplot::backend::backend_interface* createBackend() const = 0;
};

/// @brief 绘图后端提供者接口的唯一 ID（插件元数据里的 "iid" 必须是此值）
#define AST_IID_IPlotBackendProvider "space.ast.AstPlot.IPlotBackendProvider"

AST_DECLARE_INTERFACE(IPlotBackendProvider, AST_IID_IPlotBackendProvider)

/// @brief 按插件键创建绘图后端
/// @param name 后端索引键，如 "agg"、"qwt"
/// @return 后端实例；未找到插件时返回 nullptr。所有权归调用方
AST_PLOT_API matplot::backend::backend_interface* aNewPlotBackend(StringView name);

/// @brief 按插件键选择默认绘图后端
/// @param name 后端索引键，如 "agg"、"qwt"
/// @return 错误码
/// @note 失败时默认后端保持不变
AST_PLOT_API errc_t aUsePlotBackend(StringView name);

/// @brief 列出已发现插件提供的绘图后端索引键
AST_PLOT_API std::vector<std::string> aPlotBackendNames();

/*! @} */

AST_NAMESPACE_END
