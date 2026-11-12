///
/// @file      VisRenderer.cpp
/// @brief     渲染器注册表实现
/// @details   维护全局默认渲染器；当未显式设置时，通过插件注册表自动选择
///            已发现的可视化渲染后端（优先 "vtk"）。
/// @author    axel
/// @date      2026-09-06
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

#include "VisRenderer.hpp"
#include "VisBackend.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Span.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

namespace {

/// @brief 进程级默认渲染器及其同步设施
std::mutex g_defaultRendererMutex;
std::unique_ptr<VisRenderer> g_defaultRenderer;
std::once_flag g_selectDefaultOnce;

/// @brief 从已注册的插件键中选择默认后端，优先 VTK
std::string selectDefaultRendererKey(Span<std::string> names)
{
    for (const std::string& name : names) {
        if (name == "vtk")
            return name;
    }
    if (!names.empty())
        return names.front();   // 注册表按 map 键序返回，结果确定
    return std::string();
}

} // namespace

VisRenderer* VisRenderer::Default()
{
    std::call_once(g_selectDefaultOnce, [] {
        {
            std::lock_guard<std::mutex> lock(g_defaultRendererMutex);
            if (g_defaultRenderer)
                return;   // 已由 SetDefault 显式指定
        }

        // 不持锁扫描插件/调用插件代码，避免与动态库加载器锁交叉
        std::vector<std::string> names = aVisRendererNames();
        if (names.empty()) {
            aWarning(_("未找到可视化渲染后端插件，请确认插件已构建并位于可扫描目录"));
            return;
        }

        // 局部 unique_ptr 保证并发竞态下未被采用的实例也会被释放
        std::unique_ptr<VisRenderer> renderer(aNewVisRenderer(selectDefaultRendererKey(names)));
        if (!renderer)
            return;

        std::lock_guard<std::mutex> lock(g_defaultRendererMutex);
        if (!g_defaultRenderer) {
            g_defaultRenderer = std::move(renderer);
        }
        // 若并发下已有显式默认值，保留显式值，renderer 在作用域结束时释放
    });

    std::lock_guard<std::mutex> lock(g_defaultRendererMutex);
    return g_defaultRenderer.get();
}

void VisRenderer::SetDefault(VisRenderer* renderer)
{
    if (!renderer)
        return;

    std::unique_ptr<VisRenderer> owned(renderer);
    std::unique_ptr<VisRenderer> previous;
    {
        std::lock_guard<std::mutex> lock(g_defaultRendererMutex);
        previous = std::move(g_defaultRenderer);
        g_defaultRenderer = std::move(owned);
    }
    // previous 在锁外析构，避免在持锁时执行插件侧的析构代码
}

AST_NAMESPACE_END
