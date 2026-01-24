///
/// @file      bmNutation.cpp
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

#include "AstCore/SOFA.hpp"
#include "benchmark/benchmark.h"

AST_USING_NAMESPACE

void bmNutation_Repeat(benchmark::State& state)
{
    double t = 1.23456789;
    double dpsi = 0.0;
    double deps = 0.0;
    for (auto _ : state)
    {
        aNutation_IAU1980(t, dpsi, deps);
        benchmark::DoNotOptimize(dpsi);
        benchmark::DoNotOptimize(deps);
    }
}

BENCHMARK(bmNutation_Repeat);


void bmNutation_Cache_Repeat(benchmark::State& state)
{
    double t = 1.23456789;
    double dpsi = 0.0;
    double deps = 0.0;
    for (auto _ : state)
    {
        aNutation_IAU1980_Cache(t, dpsi, deps);
        benchmark::DoNotOptimize(dpsi);
        benchmark::DoNotOptimize(deps);
    }
}

BENCHMARK(bmNutation_Cache_Repeat);



void bmNutation(benchmark::State& state)
{
    double t = 1.23456789;
    double dpsi = 0.0;
    double deps = 0.0;
    for (auto _ : state)
    {
        t += 1./16.;
        aNutation_IAU1980(t, dpsi, deps);
        benchmark::DoNotOptimize(dpsi);
        benchmark::DoNotOptimize(deps);
    }
}

BENCHMARK(bmNutation);


void bmNutation_Cache(benchmark::State& state)
{
    double t = 1.23456789;
    double dpsi = 0.0;
    double deps = 0.0;
    for (auto _ : state)
    {
        t += 1./16.;
        aNutation_IAU1980_Cache(t, dpsi, deps);
        benchmark::DoNotOptimize(dpsi);
        benchmark::DoNotOptimize(deps);
    }
}

BENCHMARK(bmNutation_Cache);

BENCHMARK_MAIN();
