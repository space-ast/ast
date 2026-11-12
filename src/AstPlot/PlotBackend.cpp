///
/// @file      PlotBackend.cpp
/// @brief     绘图后端插件接口与宿主 API 实现
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

#include "PlotBackend.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/PluginRegistry.hpp"

#if defined(AST_WITH_MATPLOTPLUSPLUS)
A_SUPPRESS_WARNINGS_BEGIN
#include <matplot/backend/backend_registry.h>
A_SUPPRESS_WARNINGS_END
#endif

AST_NAMESPACE_BEGIN

namespace {

/// @brief 按索引键查找绘图后端提供者插件
IPlotBackendProvider* findPlotBackendProvider(StringView name)
{
    aEnsurePluginsScanned();

    PluginInstance* instance = aPluginRegistry().find(aInterfaceId<IPlotBackendProvider>(), name);
    if (!instance)
        return nullptr;

    IPlotBackendProvider* provider = aPluginCast<IPlotBackendProvider>(instance);
    if (!provider) {
        aError(_("插件接口类型不匹配：期望 IPlotBackendProvider"));
    }
    return provider;
}

#if defined(AST_WITH_MATPLOTPLUSPLUS)

/// @brief 当前默认绘图后端提供者（仅本翻译单元持有，避免每个 DLL 各有一份）
IPlotBackendProvider* g_defaultPlotBackendProvider = nullptr;

/// @brief matplot++ 默认后端工厂（无捕获，供函数指针注册使用）
matplot::backend::backend_interface* createDefaultPlotBackend()
{
    return g_defaultPlotBackendProvider
               ? g_defaultPlotBackendProvider->createBackend()
               : nullptr;
}

#endif  // AST_WITH_MATPLOTPLUSPLUS

} // namespace

matplot::backend::backend_interface* aNewPlotBackend(StringView name)
{
    if (name.empty()) {
        aWarning(_("绘图后端索引键为空"));
        return nullptr;
    }

    IPlotBackendProvider* provider = findPlotBackendProvider(name);
    if (!provider) {
        aWarning(_("未找到绘图后端插件 '%.*s'，请确认插件已构建并位于可扫描目录"), name.size(), name.data());
        return nullptr;
    }
    return provider->createBackend();
}

errc_t aUsePlotBackend(StringView name)
{
    if (name.empty())
        return eErrorNullInput;

    IPlotBackendProvider* provider = findPlotBackendProvider(name);
    if (!provider) {
        aError(_("未找到绘图后端插件 '%.*s'，请确认插件已构建并位于可扫描目录"), name.size(), name.data());
        return eErrorNotFound;
    }

#if defined(AST_WITH_MATPLOTPLUSPLUS)
    g_defaultPlotBackendProvider = provider;
    matplot::change_default_backend(&createDefaultPlotBackend);
    return eNoError;
#else
    aError(_("matplot++ 未启用，无法设置绘图后端 '%s'"),
           std::string(name.data(), name.size()).c_str());
    return eErrorUnsupported;
#endif
}

std::vector<std::string> aPlotBackendNames()
{
    aEnsurePluginsScanned();
    return aPluginRegistry().keys<IPlotBackendProvider>();
}

AST_NAMESPACE_END
