/// @file      bmStringView.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      2.12.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstMock/MockStringView.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE

//const char* string_value = "2010-09-10 12:00:00";
const char* string_value = "2010-09-10";
//const char* string_value = "123";



static void setStringValue(benchmark::State& state)
{
    for (auto _ : state) {
        auto value = aMockSetStringValue(string_value);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(setStringValue);

static void setStringConstRef(benchmark::State& state)
{
    for (auto _ : state) {
        auto value = aMockSetStringConstRef(string_value);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(setStringConstRef);

static void setStringView(benchmark::State& state)
{
    for (auto _ : state) {
        auto value = aMockSetStringView(string_value);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(setStringView);


#if _HAS_CXX17
static void setStringViewStd(benchmark::State& state)
{
    for (auto _ : state) {
        auto value = aMockSetStringViewStd(string_value);
        benchmark::DoNotOptimize(value);
    }
}
BENCHMARK(setStringViewStd);
#endif


BENCHMARK_MAIN();

