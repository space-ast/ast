///
/// @file      plugin.cpp
/// @brief     Agg 绘图后端插件：注册入口
/// @details   本动态库向主程序注册 matplot++ 的 "agg" 离屏渲染后端。
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

#include "AstGlobal.h"
#include "AstUtil/PluginInterface.hpp"
#include "AstPlot/PlotBackend.hpp"
#include "AggBackend.hpp"

AST_USING_NAMESPACE

namespace {

/// @brief Agg 绘图后端提供者
class AggBackendProvider : public PluginInstance, public IPlotBackendProvider
{
public:
    matplot::backend::backend_interface* createBackend() const override
    {
        return new AggBackend();
    }
};

} // namespace

AST_PLUGIN_IID(IPlotBackendProvider)     ///< 插件接口 ID
AST_PLUGIN_KEYS("agg")                   ///< 插件注册名
AST_PLUGIN_INSTANCE(AggBackendProvider)  ///< 插件实例
