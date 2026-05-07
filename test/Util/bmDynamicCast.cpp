///
/// @file      bmDynamicCast.cpp
/// @brief     aobject_cast与dynamic_cast的性能对比
/// @details   
/// @author    axel
/// @date      2026-04-29
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstSim/MotionTwoBody.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/RunTime.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE

static MotionTwoBody* motion = new MotionTwoBody();

void bm_dynamic_cast_incorrect(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        MotionTwoBody* motion = dynamic_cast<MotionTwoBody*>(obj);
        benchmark::DoNotOptimize(motion);
    }
}

BENCHMARK(bm_dynamic_cast_incorrect);


void bm_aobject_cast_incorrect(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        MotionTwoBody* motion = aobject_cast<MotionTwoBody*>(obj);
        benchmark::DoNotOptimize(motion);
    }
}

BENCHMARK(bm_aobject_cast_incorrect);

void bm_dynamic_cast_correct(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        StateCartesian* state = dynamic_cast<StateCartesian*>(obj);
        benchmark::DoNotOptimize(state);
    }
}

BENCHMARK(bm_dynamic_cast_correct);


void bm_aobject_cast_correct(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        StateCartesian* state = aobject_cast<StateCartesian*>(obj);
        benchmark::DoNotOptimize(state);
    }
}

BENCHMARK(bm_aobject_cast_correct);


void bm_dynamic_cast_superclass(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        State* state = dynamic_cast<State*>(obj);
        benchmark::DoNotOptimize(state);
    }
}


BENCHMARK(bm_dynamic_cast_superclass);


void bm_aobject_cast_superclass(benchmark::State& state)
{
    Object* obj = aNewObject("StateCartesian");
    for(auto _ : state)
    {
        State* state = aobject_cast<State*>(obj);
        benchmark::DoNotOptimize(state);
    }
}

BENCHMARK(bm_aobject_cast_superclass);

void bm_dynamic_cast_childclass(benchmark::State& state)
{
    StateCartesian* obj = (StateCartesian*)aNewObject("StateCartesian");
    for(auto _ : state)
    {
        Object* o = dynamic_cast<Object*>(obj);
        benchmark::DoNotOptimize(o);
    }
}

BENCHMARK(bm_dynamic_cast_childclass);

void bm_aobject_cast_childclass(benchmark::State& state)
{
    StateCartesian* obj = (StateCartesian*)aNewObject("StateCartesian");
    for(auto _ : state)
    {
        Object* o = aobject_cast<Object*>(obj);
        benchmark::DoNotOptimize(o);
    }
}

BENCHMARK(bm_aobject_cast_childclass);

BENCHMARK_MAIN();


