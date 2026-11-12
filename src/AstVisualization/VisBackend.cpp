///
/// @file      VisBackend.cpp
/// @brief     可视化渲染后端插件接口与宿主 API 实现
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

#include "VisBackend.hpp"
#include "VisRenderer.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/PluginRegistry.hpp"

AST_NAMESPACE_BEGIN

namespace {

/// @brief 按索引键查找渲染后端提供者插件
IVisRendererProvider* findVisRendererProvider(StringView name)
{
    aEnsurePluginsScanned();

    PluginInstance* instance = aPluginRegistry().find(aInterfaceId<IVisRendererProvider>(), name);
    if (!instance)
        return nullptr;

    IVisRendererProvider* provider = aPluginCast<IVisRendererProvider>(instance);
    if (!provider) {
        aError(_("插件接口类型不匹配：期望 IVisRendererProvider"));
    }
    return provider;
}

} // namespace

VisRenderer* aNewVisRenderer(StringView name)
{
    if (name.empty()) {
        aWarning(_("可视化渲染后端索引键为空"));
        return nullptr;
    }

    IVisRendererProvider* provider = findVisRendererProvider(name);
    if (!provider) {
        aWarning(_("未找到可视化渲染后端插件 '%.*s'，请确认插件已构建并位于可扫描目录"), name.size(), name.data());
        return nullptr;
    }
    return provider->createRenderer();
}

errc_t aUseVisRenderer(StringView name)
{
    if (name.empty())
        return eErrorNullInput;

    VisRenderer* renderer = aNewVisRenderer(name);
    if (!renderer) {
        aError(_("未找到可视化渲染后端插件 '%.*s'，请确认插件已构建并位于可扫描目录"), name.size(), name.data());
        return eErrorNotFound;
    }

    // 默认渲染器由进程持有；SetDefault 接管所有权并释放被替换的旧实例
    VisRenderer::SetDefault(renderer);
    return eNoError;
}

std::vector<std::string> aVisRendererNames()
{
    aEnsurePluginsScanned();
    return aPluginRegistry().keys<IVisRendererProvider>();
}

AST_NAMESPACE_END
