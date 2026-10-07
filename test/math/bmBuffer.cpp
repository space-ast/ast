///
/// @file      bmBuffer.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-23
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

#include "ast/MockBuffer.hpp"
#include "benchmark/benchmark.h"


AST_USING_NAMESPACE

const int size = 6;

void bmBufferMalloc(benchmark::State& state)
{
    for (auto _ : state)
    {
        aMockBufferMalloc(size);
    }
}

BENCHMARK(bmBufferMalloc);




void bmBufferNew(benchmark::State& state)
{
    for (auto _ : state)
    {
        aMockBufferNew(size);
    }
}

BENCHMARK(bmBufferNew);

void bmBufferStack(benchmark::State& state)
{
    for (auto _ : state)
    {
        aMockBufferStack(size);
    }
}

BENCHMARK(bmBufferStack);


void bmBufferAlloca(benchmark::State& state)
{
    for (auto _ : state)
    {
        aMockBufferAlloca(size);
    }
}

BENCHMARK(bmBufferAlloca);

BENCHMARK_MAIN();
