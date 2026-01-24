///
/// @file      bmCache.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-24
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


#include "AstMock/MockCache.hpp"
#include "benchmark/benchmark.h"

AST_USING_NAMESPACE

void bmCacheStatic(benchmark::State& state)
{
    int k = 1;
    for (auto _ : state)
    {
        int v = aMockCacheStatic(k);
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(bmCacheStatic);


void bmCacheExtern(benchmark::State& state)
{
    int k = 1;
    for (auto _ : state)
    {
        int v = aMockCacheExtern(k);
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(bmCacheExtern);


void bmCacheStaticTL(benchmark::State& state)
{
    int k = 1;
    for (auto _ : state)
    {
        int v = aMockCacheStaticTL(k);
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(bmCacheStaticTL);


void bmCacheExternTL(benchmark::State& state)
{
    int k = 1;
    for (auto _ : state)
    {
        int v = aMockCacheExternTL(k);
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(bmCacheExternTL);


BENCHMARK_MAIN();

