///
/// @file      bmEphemeris.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-12
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

#include "AstUtil/DAFParser.hpp"
#include "AstUtil/SPKParser.hpp"
#include "AstUtil/Environment.hpp"
#include "AstCore/JplDe.hpp"
#include "AstCore/CelestialBody.hpp"
#include "AstCore/TimePoint.hpp"
#include "AstMath/Vector.hpp"
#include "AstCore/SpiceApi.hpp"
#include <benchmark/benchmark.h>


AST_USING_NAMESPACE

void bmEphemerisDE(benchmark::State& state)
{
    JplDe de;
    de.openDefault();
    TimePoint tp = TimePoint::FromUTC(2026, 3, 12, 0, 0, 0.0);
    for(auto _ : state)
    {
        Vector3d pos;
        de.getPosICRF(tp, JplDe::eMars, JplDe::eSSBarycenter, pos);
        benchmark::DoNotOptimize(pos);
    }
}

BENCHMARK(bmEphemerisDE);


void bmEphemerisSPKParser(benchmark::State& state)
{
    if(aIsCI()) return;
    SPKParser spk("data/Test/kernels/spk/de430.bsp");
    double et = 0;
    Vector3d pos;
    Vector3d vel;
    for(auto _ : state)
    {
        spk.getPosVelNative(et, ESpiceId::eMarsBarycenter, pos, vel);
        benchmark::DoNotOptimize(pos);
    }
}

BENCHMARK(bmEphemerisSPKParser);


void bmEphemerisSpice(benchmark::State& state)
{
    if(aIsCI()) return;
    SpiceApi api(true);
    api.furnsh("data/Test/kernels/spk/de430.bsp");
    double et = 0;
    double lt;
    array6d posvel;
    for(auto _ : state)
    {
        api.spkgeo(ESpiceId::eMarsBarycenter, et, "J2000", 0, posvel.data(), &lt);
        benchmark::DoNotOptimize(posvel);
    }
}

BENCHMARK(bmEphemerisSpice);

BENCHMARK_MAIN();
