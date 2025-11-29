#include <benchmark/benchmark.h>
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE


static void getStatic(benchmark::State& state)
{
    for (auto _ : state) {
        auto context = aGlobalContext_GetDefault();
        benchmark::DoNotOptimize(context);
    }
}
BENCHMARK(getStatic);

static void getThreadLoad(benchmark::State& state)
{
    aGlobalContext_Ensure();
    for (auto _ : state) {
        auto context = aGlobalContext_GetCurrent();
        benchmark::DoNotOptimize(context);
    }
}
BENCHMARK(getThreadLoad);







BENCHMARK_MAIN();
