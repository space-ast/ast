///
/// @file      bmLagrangeInterpolate.cpp
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

#include "AstMath/LagrangeInterpolator.hpp"
#include "benchmark/benchmark.h"



const double x[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
const double y[12][3] = {{1,2,3},{4,5,6},{7,8,9},{10,11,12},{13,14,15},{16,17,18},{19,20,21},{22,23,24},{25,26,27},{28,29,30},{31,32,33},{34,35,36}};

AST_USING_NAMESPACE

const int n = 9;
const int dimy = 3;
const int x_interp = 3.5;

void bmLagrangeInterpolate(benchmark::State& state) {
    for (auto _ : state) {
        double y_interp[3] = {0.0};
        aLagrangeInterpolate(x, (double*)y, n, dimy, x_interp, y_interp);
        benchmark::DoNotOptimize(y_interp);
    }
}

BENCHMARK(bmLagrangeInterpolate);


void bmLagrangeInterpolate2(benchmark::State& state) {
    for (auto _ : state) {
        double y_interp[3];
        aLagrangeInterpolate2(x, (double*)y, n, dimy, x_interp, y_interp);
        benchmark::DoNotOptimize(y_interp);
    }
}

BENCHMARK(bmLagrangeInterpolate2);


void bmLagrangeInterpolateFixedStep(benchmark::State& state) {
    double denomArray[n];
    aLagrangeInterpolateDenomArray(n, x[1] - x[0], denomArray);
    for (auto _ : state) {
        double y_interp[3];
        aLagrangeInterpolateFixedStep(x, (double*)y, n, dimy, x_interp, y_interp, denomArray);
        benchmark::DoNotOptimize(y_interp);
    }
}

BENCHMARK(bmLagrangeInterpolateFixedStep);


BENCHMARK_MAIN();

