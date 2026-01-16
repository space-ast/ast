///
/// @file      bmGravityCalculator.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-15
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

#include "AstCore/GravityCalculator.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/String.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE

const int degree = 30;
const int order = 30;
const Vector3d position{ 30000e3, 20000e3, 10e3 };
const char* grvFile = "/Test/CentralBodies/Earth/WGS84.grv";
// const char* grvFile = "/Test/CentralBodies/Earth/egm2008.grv";

#ifdef _AST_ENABLE_GRAVITY_CALCULATOR_1

static void bmCalcPertAcceleration1(benchmark::State& state)
{
    GravityField gf;
    gf.load(aDataDirGet() + grvFile);
    GravityCalculator1 gc1(gf, degree, order);
    Vector3d pertAccel1;
    for (auto _ : state) {
        gc1.calcPertAcceleration(position, pertAccel1);
        benchmark::DoNotOptimize(pertAccel1);
    }
}

BENCHMARK(bmCalcPertAcceleration1);

#endif


static void bmCalcPertAcceleration2(benchmark::State& state)
{
    GravityField gf;
    gf.load(aDataDirGet() + grvFile);
    GravityCalculator2 gc2(gf, degree, order);
    Vector3d pertAccel2;
    for (auto _ : state) {
        gc2.calcPertAcceleration(position, pertAccel2);
        benchmark::DoNotOptimize(pertAccel2);
    }
}

BENCHMARK(bmCalcPertAcceleration2);


static void bmCalcPertAcceleration3(benchmark::State& state)
{
    GravityField gf;
    gf.load(aDataDirGet() + grvFile);
    GravityCalculator3 gc3(gf, degree, order);
    Vector3d pertAccel3;
    for (auto _ : state) {
        gc3.calcPertAcceleration(position, pertAccel3);
        benchmark::DoNotOptimize(pertAccel3);
    }
}

BENCHMARK(bmCalcPertAcceleration3);


BENCHMARK_MAIN();