///
/// @file      bmAxesTransform.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-04
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


#include "AstCore/AxesTransform.hpp"
#include "AstCore/Axes.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstCore/EarthFrame.hpp"
#include "AstCore/RunTime.hpp"
#include "AstMath/Matrix.hpp"
#include "AstMath/Rotation.hpp"
#include "AstMath/KinematicRotation.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE


void bmICRFToECF_General(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    HAxes icrf = aAxesICRF();
    HAxes ecf = aAxesECF();
    for (auto _ : state)
    {
        aAxesTransform(icrf, ecf, tp, rotation);
        benchmark::DoNotOptimize(rotation);
    }
}

BENCHMARK(bmICRFToECF_General);

void bmICRFToECF_Special(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    for (auto _ : state)
    {
        aICRFToECFTransform(tp, rotation);
        benchmark::DoNotOptimize(rotation);
    }
}

BENCHMARK(bmICRFToECF_Special);



void bmJ2000ToECF_General(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    HAxes j2000 = aAxesJ2000();
    HAxes ecf = aAxesECF();
    for (auto _ : state)
    {
        aAxesTransform(j2000, ecf, tp, rotation);
        benchmark::DoNotOptimize(rotation);
    }
}

BENCHMARK(bmJ2000ToECF_General);

void bmJ2000ToECF_Special(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    for (auto _ : state)
    {
        aJ2000ToECFTransform(tp, rotation);
        benchmark::DoNotOptimize(rotation);
    }
}

BENCHMARK(bmJ2000ToECF_Special);


void bmJ2000ToGTOD_General(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    HAxes j2000 = aAxesJ2000();
    HAxes gtod = aAxesGTOD();
    for (auto _ : state)
    {
        aAxesTransform(j2000, gtod, tp, rotation);
        benchmark::DoNotOptimize(rotation);
    }
}

BENCHMARK(bmJ2000ToGTOD_General);


void bmJ2000ToGTOD_Special(benchmark::State& state)
{
    aInitialize();
    auto tp = TimePoint::FromUTC(2016, 3, 4, 0, 0, 0);
    Rotation rotation;
    for (auto _ : state)
    {
        {
            Rotation temp1, temp2, temp3;
            aJ2000ToMODTransform(tp, temp1);
            aMODToTODTransform(tp, temp2);
            aTODToGTODTransform(tp, temp3);
            rotation = temp1 * temp2 * temp3;
            benchmark::DoNotOptimize(rotation);
        }
    }
}

BENCHMARK(bmJ2000ToGTOD_Special);


BENCHMARK_MAIN();

