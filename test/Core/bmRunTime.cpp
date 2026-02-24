///
/// @file      bmRunTime.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-24
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
#include "AstCore/DataContext.hpp"
#include "benchmark/benchmark.h"

AST_USING_NAMESPACE

void bmInitialize(benchmark::State& state)
{
    for (auto _ : state)
    {
        aInitialize();
        aUninitialize();
    }
}

BENCHMARK(bmInitialize);

void bmInitialize_LeapSecond(benchmark::State& state)
{
    LeapSecond leapSecond;
    for (auto _ : state)
    {
        err_t rc = leapSecond.loadDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_LeapSecond);

void bmInitialize_JplDe(benchmark::State& state)
{
    JplDe jplDe;
    for (auto _ : state)
    {
        err_t rc = jplDe.openDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_JplDe);


void bmInitialize_EOP(benchmark::State& state)
{
    EOP eop;
    for (auto _ : state)
    {
        err_t rc = eop.loadDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_EOP);


void bmInitialize_SpaceWeather(benchmark::State& state)
{
    SpaceWeather spaceWeather;
    for (auto _ : state)
    {
        err_t rc = spaceWeather.loadDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_SpaceWeather);


void bmInitialize_IAUXYSPrecomputed(benchmark::State& state)
{
    IAUXYSPrecomputed iauXYSData;
    for (auto _ : state)
    {
        err_t rc = iauXYSData.loadDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_IAUXYSPrecomputed);


void bmInitialize_SolarSystem(benchmark::State& state)
{
    SolarSystem solarSystem;
    for (auto _ : state)
    {
        err_t rc = solarSystem.loadDefault();
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_SolarSystem);


void bmInitialize_Earth(benchmark::State& state)
{
    CelestialBody earth;
    std::string filepath = aDataDirGet() + "/SolarSystem/Earth/Earth.cb";
    for (auto _ : state)
    {
        err_t rc = earth.load(filepath);
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_Earth);


void bmInitialize_Moon(benchmark::State& state)
{
    CelestialBody moon;
    std::string filepath = aDataDirGet() + "/SolarSystem/Moon/Moon.cb";
    for (auto _ : state)
    {
        err_t rc = moon.load(filepath);
        benchmark::DoNotOptimize(rc);
    }
}

BENCHMARK(bmInitialize_Moon);


BENCHMARK_MAIN();
