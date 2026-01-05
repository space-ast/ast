///
/// @file      bmSinCos.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "AstUtil/Math.hpp"
#include "AstUtil/Constants.h"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE


static void bmSinCos(benchmark::State& state)
{
    for (auto _ : state)
    {
        double sinVal, cosVal;
        sincos(state.range(0), &sinVal, &cosVal);
        benchmark::DoNotOptimize(sinVal);
        benchmark::DoNotOptimize(cosVal);
    }
}
BENCHMARK(bmSinCos)->Range(0, 2 * kPI);


static void bmSinCosSeparate(benchmark::State& state)
{
    for (auto _ : state)
    {
        double sinVal, cosVal;
        sinVal = sin(state.range(0));
        cosVal = cos(state.range(0));
        benchmark::DoNotOptimize(sinVal);
        benchmark::DoNotOptimize(cosVal);
    }
}
BENCHMARK(bmSinCosSeparate)->Range(0, 2 * kPI);



static void bmSin(benchmark::State& state)
{
    for (auto _ : state)
    {
        double sinVal;
        sinVal = sin(state.range(0));
        benchmark::DoNotOptimize(sinVal);
    }
}
BENCHMARK(bmSin)->Range(0, 2 * kPI);


static void bmCos(benchmark::State& state)
{
    for (auto _ : state)
    {
        double cosVal;
        cosVal = cos(state.range(0));
        benchmark::DoNotOptimize(cosVal);
    }
}
BENCHMARK(bmCos)->Range(0, 2 * kPI);



BENCHMARK_MAIN();
