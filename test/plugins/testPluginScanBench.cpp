///
/// @file      testPluginScanBench.cpp
/// @brief
/// @details   插件扫描耗时测试
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


#include "ast/Test.hpp"
#include "ast/PluginRegistry.hpp"
#include <chrono>
#include <cstdio>

AST_USING_NAMESPACE

int main()
{
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const errc_t rc = aScanPlugins();
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    const double ms = std::chrono::duration<double, std::milli>(end - begin).count();
    std::printf("SCAN_MS %.4f RC %d PLUGINS %d\n", ms, static_cast<int>(rc),
                static_cast<int>(PluginRegistry::Instance()->pluginCount()));
    return 0;
}

