///
/// @file      bmNutationSeries.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-12
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

#include <benchmark/benchmark.h>
#include "AstCore/NutationSeries.hpp"
#include "AstCore/RunTime.hpp"

AST_USING_NAMESPACE


static void LoadSTK_S(benchmark::State& state)
{
    NutationSeries series;
    std::string stkFile = aDataDirGet() + "/Test/ICRF/IAU2006_S.dat";
    for (auto _ : state) {
        err_t ret = series.loadSTK(stkFile);
        benchmark::DoNotOptimize(ret);
    }
}

BENCHMARK(LoadSTK_S);


static void LoadIERS_S(benchmark::State& state)
{
    NutationSeries series;
    std::string iersFile = aDataDirGet() + "/IERS-conventions/2010/tab5.2d.txt";
    for (auto _ : state) {
        err_t ret = series.loadIERS(iersFile);
        benchmark::DoNotOptimize(ret);
    }
}
BENCHMARK(LoadIERS_S);

static void LoadSTK_X(benchmark::State& state)
{
    NutationSeries series;
    std::string stkFile = aDataDirGet() + "/Test/ICRF/IAU2006_X.dat";
    for (auto _ : state) {
        err_t ret = series.loadSTK(stkFile);
        benchmark::DoNotOptimize(ret);
    }
}

BENCHMARK(LoadSTK_X);


static void LoadIERS_X(benchmark::State& state)
{
    NutationSeries series;
    std::string iersFile = aDataDirGet() + "/IERS-conventions/2010/tab5.2a.txt";
    for (auto _ : state) {
        err_t ret = series.loadIERS(iersFile);
        benchmark::DoNotOptimize(ret);
    }
}
BENCHMARK(LoadIERS_X);

BENCHMARK_MAIN();
