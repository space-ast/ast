#include "AstUtil/Unit.hpp"
#include "AstUtil/UnitManager.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE

void bm_convert_by_str(benchmark::State& state)
{
    std::string unitstr = "km";
    double si = 1.0;
    for(auto _ : state)
    {
        Unit* unit = aUnitGet(unitstr);
        double value = unit->fromSI(si);
        benchmark::DoNotOptimize(value);
    }
}

BENCHMARK(bm_convert_by_str);



void bm_convert_by_unit(benchmark::State& state)
{
    std::string unitstr = "km";
    Unit* unit = aUnitGet(unitstr);
    double si = 1.0;
    for(auto _ : state)
    {
        double value = unit->fromSI(si);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(bm_convert_by_unit);

void bm_convert_direct(benchmark::State& state)
{
    double si = 1.0;
    double scale = 1000.0;
    for(auto _ : state)
    {
        double value = si / scale;
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(bm_convert_direct);


void bm_convert_by_unit2(benchmark::State& state)
{
    std::string unitstr = "km";
    Unit* unit = aUnitGet(unitstr);
    double si = 1.0;
    for(auto _ : state)
    {
        double value = unit->fromSI(si);
        (void) value;
    }
}
BENCHMARK(bm_convert_by_unit2);

void bm_convert_direct2(benchmark::State& state)
{
    double si = 1.0;
    double scale = 1000.0;
    for(auto _ : state)
    {
        double value = si / scale;
    }
}
BENCHMARK(bm_convert_direct2);


BENCHMARK_MAIN();

