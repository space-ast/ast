///
/// @file      bmXYS.cpp
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

#include "AstCore/RunTime.hpp"
#include "benchmark/benchmark.h"

AST_USING_NAMESPACE

void bmTheoreticalXYS_IERS2010_Cache_Repeat(benchmark::State& state)
{
    aInitialize();
    double t = 1.2345678;
    array3d xys;
    for (auto _ : state) {
        aTheoreticalXYS_IERS2010_Cache(t, xys);
        benchmark::DoNotOptimize(xys);
    }
}
BENCHMARK(bmTheoreticalXYS_IERS2010_Cache_Repeat);

void bmTheoreticalXYS_IERS2010_Repeat(benchmark::State& state)
{
    aInitialize();
    double t = 1.2345678;
    array3d xys;
    for (auto _ : state) {
        aTheoreticalXYS_IERS2010(t, xys);
        benchmark::DoNotOptimize(xys);
    }
}
BENCHMARK(bmTheoreticalXYS_IERS2010_Repeat);


void bmTheoreticalXYS_IERS2010(benchmark::State& state)
{
    aInitialize();
    double t = 1.2345678;
    array3d xys;
    for (auto _ : state) {
        t += 1./16.;
        aTheoreticalXYS_IERS2010(t, xys);
        benchmark::DoNotOptimize(xys);
    }
}
BENCHMARK(bmTheoreticalXYS_IERS2010);

void bmTheoreticalXYS_IERS2010_Cache(benchmark::State& state)
{
    aInitialize();
    double t = 1.2345678;
    array3d xys;
    for (auto _ : state) {
        t += 1./16.;
        aTheoreticalXYS_IERS2010_Cache(t, xys);
        benchmark::DoNotOptimize(xys);
    }
}
BENCHMARK(bmTheoreticalXYS_IERS2010_Cache);

BENCHMARK_MAIN();

