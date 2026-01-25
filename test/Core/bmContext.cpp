///
/// @file      bmContext.cpp
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

#include "AstMock/MockContext.hpp"
#include "benchmark/benchmark.h"

AST_USING_NAMESPACE


void bmMockGetEOP_Extern(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_Extern();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_Extern);

void bmMockGetEOP_Static(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_Static();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_Static);


void bmMockGetEOP_Lazy(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_Lazy();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_Lazy);



void bmMockGetEOP_ContextExtern(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_ContextExtern();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_ContextExtern);


void bmMockGetEOP_ContextStatic(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_ContextStatic();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_ContextStatic);



void bmMockGetEOP_ContextExternTL(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_ContextExternTL();
        benchmark::DoNotOptimize(eop);
    }
}

void bmMockGetEOP_ContextStaticTL(benchmark::State& state)
{
    for (auto _ : state)
    {
        auto eop = aMockGetEOP_ContextStaticTL();
        benchmark::DoNotOptimize(eop);
    }
}

BENCHMARK(bmMockGetEOP_ContextStaticTL);



BENCHMARK_MAIN();
